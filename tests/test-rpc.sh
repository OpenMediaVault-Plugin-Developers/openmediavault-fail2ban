#!/usr/bin/env bash
# test-rpc.sh — Integration tests for openmediavault-fail2ban RPC methods.
#
# Usage: sudo ./tests/test-rpc.sh
#
# Exercises all Fail2Ban RPC methods: settings read/write and validation,
# jail CRUD, jail name uniqueness, the per-jail chain option, large ban
# times, getStats, and (when the service is enabled) the salt deploy output.
#
# Test jails are always created disabled and are removed on exit.  Original
# settings are restored on exit.
#
# WARNING: when the fail2ban service is enabled, the "Deploy" section runs
#          'omv-salt deploy run fail2ban', which rewrites the jail files and
#          restarts fail2ban.  Run on a test system or during a maintenance
#          window.

set -uo pipefail

# ---------------------------------------------------------------------------
# Colours / counters  (display → stderr; $() captures only JSON)
# ---------------------------------------------------------------------------
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

PASS=0
FAIL=0
SKIP=0
declare -a FAILED_TESTS=()

section() { echo -e "\n${CYAN}${BOLD}=== $* ===${NC}" >&2; }
info()    { echo -e "  ${YELLOW}»${NC} $*" >&2; }

_pass() {
    echo -e "  ${GREEN}PASS${NC}  $1" >&2
    ((PASS++)) || true
}
_fail() {
    echo -e "  ${RED}FAIL${NC}  $1" >&2
    [ -n "${2:-}" ] && echo -e "         ${RED}→${NC} $2" >&2
    ((FAIL++)) || true
    FAILED_TESTS+=("$1")
}
_skip() {
    echo -e "  ${YELLOW}SKIP${NC}  $1${2:+  ($2)}" >&2
    ((SKIP++)) || true
}

# ---------------------------------------------------------------------------
# RPC helpers
# ---------------------------------------------------------------------------
rpc() {
    local svc=$1 method=$2 params=${3:-'{}'}
    omv-rpc -u admin "$svc" "$method" "$params"
}

# Output of the last successful assert_rpc is left in $RPC_OUT.  Never call
# assert_rpc inside $() — PASS/FAIL counters would not propagate.
RPC_OUT=""
assert_rpc() {
    local desc=$1 svc=$2 method=$3 params=${4:-'{}'} pattern=${5:-}
    local out ec=0
    RPC_OUT=""
    out=$(omv-rpc -u admin "$svc" "$method" "$params" 2>&1) || ec=$?
    if [ $ec -ne 0 ]; then
        _fail "$desc" "$(echo "$out" | tail -3)"
        return 1
    fi
    if [ -n "$pattern" ] && ! echo "$out" | grep -q -- "$pattern"; then
        _fail "$desc" "Pattern '$pattern' not found in: ${out:0:200}"
        return 1
    fi
    _pass "$desc"
    RPC_OUT="$out"
    return 0
}

assert_rpc_fails() {
    local desc=$1 svc=$2 method=$3 params=${4:-'{}'}
    local out ec=0
    out=$(omv-rpc -u admin "$svc" "$method" "$params" 2>&1) || ec=$?
    if [ $ec -eq 0 ] && ! echo "$out" | grep -qi "exception"; then
        _fail "$desc" "Expected failure but RPC succeeded: ${out:0:200}"
        return 1
    fi
    _pass "$desc"
    return 0
}

# Assert a jq expression is true for the given JSON.
assert_jq() {
    local desc=$1 json=$2 expr=$3
    if echo "$json" | jq -e "$expr" &>/dev/null; then
        _pass "$desc"
        return 0
    fi
    _fail "$desc" "jq '$expr' false for: ${json:0:200}"
    return 1
}

# Build a setJail payload.
# Usage: jail_json <uuid> <name> [chain] [bantime] [logpath]
jail_json() {
    jq -nc \
        --arg uuid "$1" --arg name "$2" --arg chain "${3:-}" \
        --argjson bantime "${4:-3600}" --arg logpath "${5:-}" \
        '{uuid:$uuid, enable:false, name:$name, port:"ssh", maxretry:5,
          bantime:$bantime, filter:"sshd", logpath:$logpath, chain:$chain}'
}

# ---------------------------------------------------------------------------
# State
# ---------------------------------------------------------------------------
JAIL1_UUID=""
JAIL2_UUID=""
ORIG_SETTINGS=""
DEPLOYED=0

JAIL1_NAME="omvtest_jail1_$$"
JAIL2_NAME="omvtest_jail2_$$"

JAIL_DIR="/etc/fail2ban/jail.d"
JAIL_PREFIX="openmediavault-"

LIST_PARAMS='{"start":0,"limit":null,"sortfield":null,"sortdir":null}'
OMV_NEW_UUID=$(. /etc/default/openmediavault 2>/dev/null; \
    echo "${OMV_CONFIGOBJECT_NEW_UUID:-fa4b1c66-ef79-11e5-87a0-0002b3a176b4}")

# ---------------------------------------------------------------------------
# Cleanup — always runs on exit
# ---------------------------------------------------------------------------
cleanup() {
    section "Cleanup"

    for uuid in "$JAIL1_UUID" "$JAIL2_UUID"; do
        [ -z "$uuid" ] && continue
        info "Deleting test jail $uuid"
        rpc "Fail2Ban" "deleteJail" "{\"uuid\":\"$uuid\"}" &>/dev/null || true
    done
    JAIL1_UUID=""
    JAIL2_UUID=""

    if [ -n "$ORIG_SETTINGS" ]; then
        info "Restoring original fail2ban settings"
        rpc "Fail2Ban" "setSettings" "$ORIG_SETTINGS" &>/dev/null || true
    fi

    # Re-deploy so the test jail files written by the Deploy section are
    # removed from $JAIL_DIR.
    if [ $DEPLOYED -eq 1 ]; then
        info "Re-deploying fail2ban to remove test jail files"
        omv-salt deploy run fail2ban &>/dev/null || true
    fi

    info "Done."
}
trap cleanup EXIT

# ---------------------------------------------------------------------------
# Pre-flight
# ---------------------------------------------------------------------------
section "Pre-flight"

if [ "$(id -u)" -ne 0 ]; then
    echo -e "${RED}Must be run as root.${NC}" >&2
    exit 1
fi

for cmd in omv-rpc jq; do
    if command -v "$cmd" &>/dev/null; then
        _pass "command available: $cmd"
    else
        echo -e "\n${RED}$cmd not found in PATH — aborting.${NC}" >&2
        exit 1
    fi
done

if ! omv-rpc -u admin "Config" "isDirty" '{}' &>/dev/null; then
    echo -e "\n${RED}omv-rpc not functional — aborting.${NC}" >&2
    exit 1
fi
_pass "omv-rpc functional"

# ===========================================================================
section "Settings — read"
# ===========================================================================

assert_rpc "getSettings" "Fail2Ban" "getSettings" && ORIG_SETTINGS=$RPC_OUT || {
    echo -e "\n${RED}getSettings failed — aborting.${NC}" >&2
    exit 1
}

for field in enable ignoreip findtime bantime maxretry destemail action; do
    assert_jq "getSettings — field '$field' present" "$ORIG_SETTINGS" "has(\"$field\")"
done
assert_jq "getSettings — 'jails' removed" "$ORIG_SETTINGS" 'has("jails") | not'

ORIG_ENABLE=$(echo "$ORIG_SETTINGS" | jq -r '.enable')
info "Service enabled: $ORIG_ENABLE"

# ===========================================================================
section "Settings — write"
# ===========================================================================

# Keep 'enable' as-is so these tests never change the service state.
NEW_SETTINGS=$(echo "$ORIG_SETTINGS" | jq -c '
    .ignoreip = "127.0.0.1/8 ::1 192.0.2.10"
    | .findtime = 1200 | .bantime = 315360000 | .maxretry = 7
    | .destemail = "omvtest@localhost" | .action = "action_mw"')

assert_rpc "setSettings — modified values" "Fail2Ban" "setSettings" \
    "$NEW_SETTINGS" && OUT=$RPC_OUT && {
    assert_jq "setSettings — returns no 'jails'" "$OUT" 'has("jails") | not'
}

assert_rpc "getSettings — after write" "Fail2Ban" "getSettings" && OUT=$RPC_OUT && {
    assert_jq "getSettings — ignoreip stored" "$OUT" '.ignoreip == "127.0.0.1/8 ::1 192.0.2.10"'
    assert_jq "getSettings — findtime stored" "$OUT" '.findtime == 1200'
    assert_jq "getSettings — large bantime stored" "$OUT" '.bantime == 315360000'
    assert_jq "getSettings — maxretry stored" "$OUT" '.maxretry == 7'
    assert_jq "getSettings — destemail stored" "$OUT" '.destemail == "omvtest@localhost"'
    assert_jq "getSettings — action stored" "$OUT" '.action == "action_mw"'
}

for action in action_ action_mwl; do
    assert_rpc "setSettings — action=$action" "Fail2Ban" "setSettings" \
        "$(echo "$NEW_SETTINGS" | jq -c --arg a "$action" '.action = $a')" \
        "\"action\":\"$action\""
done

rpc "Fail2Ban" "setSettings" "$ORIG_SETTINGS" &>/dev/null || true

# ===========================================================================
section "Settings — negative tests"
# ===========================================================================

assert_rpc_fails "setSettings — invalid action" "Fail2Ban" "setSettings" \
    "$(echo "$ORIG_SETTINGS" | jq -c '.action = "action_bogus"')"

assert_rpc_fails "setSettings — missing enable" "Fail2Ban" "setSettings" \
    "$(echo "$ORIG_SETTINGS" | jq -c 'del(.enable)')"

assert_rpc_fails "setSettings — missing ignoreip" "Fail2Ban" "setSettings" \
    "$(echo "$ORIG_SETTINGS" | jq -c 'del(.ignoreip)')"

assert_rpc_fails "setSettings — non-integer bantime" "Fail2Ban" "setSettings" \
    "$(echo "$ORIG_SETTINGS" | jq -c '.bantime = "forever"')"

assert_rpc_fails "setSettings — non-integer findtime" "Fail2Ban" "setSettings" \
    "$(echo "$ORIG_SETTINGS" | jq -c '.findtime = "1h"')"

# ===========================================================================
section "Jail — CRUD"
# ===========================================================================

assert_rpc "getJailList (initial)" "Fail2Ban" "getJailList" "$LIST_PARAMS" \
    '"total"'

J1_RESULT=$(rpc "Fail2Ban" "setJail" "$(jail_json "$OMV_NEW_UUID" "$JAIL1_NAME")" 2>&1) \
    && J1_EC=0 || J1_EC=$?
JAIL1_UUID=$(echo "$J1_RESULT" | jq -r '.uuid // ""' 2>/dev/null || echo "")

if [ $J1_EC -eq 0 ] && [ -n "$JAIL1_UUID" ] && [ "$JAIL1_UUID" != "$OMV_NEW_UUID" ]; then
    _pass "setJail (create $JAIL1_NAME) — UUID: $JAIL1_UUID"
else
    _fail "setJail (create $JAIL1_NAME)" "${J1_RESULT:0:200}"
    JAIL1_UUID=""
fi

if [ -n "$JAIL1_UUID" ]; then
    assert_rpc "getJail ($JAIL1_NAME)" "Fail2Ban" "getJail" \
        "{\"uuid\":\"$JAIL1_UUID\"}" "\"uuid\":\"$JAIL1_UUID\"" && OUT=$RPC_OUT && {
        assert_jq "getJail — name" "$OUT" ".name == \"$JAIL1_NAME\""
        assert_jq "getJail — enable=false" "$OUT" '.enable == false'
        assert_jq "getJail — port" "$OUT" '.port == "ssh"'
        assert_jq "getJail — filter" "$OUT" '.filter == "sshd"'
        assert_jq "getJail — maxretry" "$OUT" '.maxretry == 5'
        assert_jq "getJail — bantime" "$OUT" '.bantime == 3600'
        assert_jq "getJail — logpath empty" "$OUT" '.logpath == ""'
        assert_jq "getJail — chain empty" "$OUT" '.chain == ""'
    }

    assert_rpc "getJailList — $JAIL1_NAME present" "Fail2Ban" "getJailList" \
        "$LIST_PARAMS" "$JAIL1_NAME" && OUT=$RPC_OUT && {
        assert_jq "getJailList — total >= 1" "$OUT" '.total >= 1'
    }

    # Edit — chain and a large ban time (10 years)
    assert_rpc "setJail (edit — chain=DOCKER-USER, bantime=315360000)" \
        "Fail2Ban" "setJail" \
        "$(jail_json "$JAIL1_UUID" "$JAIL1_NAME" "DOCKER-USER" 315360000 "/var/log/auth.log")" \
        '"chain":"DOCKER-USER"'

    assert_rpc "getJail — after edit" "Fail2Ban" "getJail" \
        "{\"uuid\":\"$JAIL1_UUID\"}" && OUT=$RPC_OUT && {
        assert_jq "getJail — chain stored" "$OUT" '.chain == "DOCKER-USER"'
        assert_jq "getJail — large bantime stored" "$OUT" '.bantime == 315360000'
        assert_jq "getJail — logpath stored" "$OUT" '.logpath == "/var/log/auth.log"'
    }

    # Re-saving with the same name must not trip the uniqueness check.
    assert_rpc "setJail (edit — same name, unchanged)" "Fail2Ban" "setJail" \
        "$(jail_json "$JAIL1_UUID" "$JAIL1_NAME" "DOCKER-USER")"

    # Chain at the 28 character limit
    CHAIN28=$(printf 'C%.0s' {1..28})
    assert_rpc "setJail (edit — 28 char chain)" "Fail2Ban" "setJail" \
        "$(jail_json "$JAIL1_UUID" "$JAIL1_NAME" "$CHAIN28")" \
        "\"chain\":\"$CHAIN28\""
fi

# ===========================================================================
section "Jail — name uniqueness"
# ===========================================================================

if [ -n "$JAIL1_UUID" ]; then
    assert_rpc_fails "setJail (create — duplicate name)" "Fail2Ban" "setJail" \
        "$(jail_json "$OMV_NEW_UUID" "$JAIL1_NAME")"

    J2_RESULT=$(rpc "Fail2Ban" "setJail" "$(jail_json "$OMV_NEW_UUID" "$JAIL2_NAME")" 2>&1) \
        && J2_EC=0 || J2_EC=$?
    JAIL2_UUID=$(echo "$J2_RESULT" | jq -r '.uuid // ""' 2>/dev/null || echo "")
    if [ $J2_EC -eq 0 ] && [ -n "$JAIL2_UUID" ] && [ "$JAIL2_UUID" != "$OMV_NEW_UUID" ]; then
        _pass "setJail (create $JAIL2_NAME) — UUID: $JAIL2_UUID"
    else
        _fail "setJail (create $JAIL2_NAME)" "${J2_RESULT:0:200}"
        JAIL2_UUID=""
    fi

    if [ -n "$JAIL2_UUID" ]; then
        assert_rpc_fails "setJail (edit — rename to existing name)" "Fail2Ban" "setJail" \
            "$(jail_json "$JAIL2_UUID" "$JAIL1_NAME")"

        OUT=$(rpc "Fail2Ban" "getJail" "{\"uuid\":\"$JAIL2_UUID\"}" 2>/dev/null || echo '{}')
        assert_jq "getJail — failed rename left name unchanged" "$OUT" \
            ".name == \"$JAIL2_NAME\""
    fi
else
    _skip "name uniqueness tests" "no test jail"
fi

# ===========================================================================
section "Jail — negative tests"
# ===========================================================================

if [ -n "$JAIL1_UUID" ]; then
    CHAIN29=$(printf 'C%.0s' {1..29})
    assert_rpc_fails "setJail — chain too long (29 chars)" "Fail2Ban" "setJail" \
        "$(jail_json "$JAIL1_UUID" "$JAIL1_NAME" "$CHAIN29")"

    for bad in "BAD CHAIN" "chain;rm" "chain.dot" 'a$b'; do
        assert_rpc_fails "setJail — invalid chain '$bad'" "Fail2Ban" "setJail" \
            "$(jail_json "$JAIL1_UUID" "$JAIL1_NAME" "$bad")"
    done
fi

assert_rpc_fails "setJail — missing chain" "Fail2Ban" "setJail" \
    "$(jail_json "$OMV_NEW_UUID" "omvtest_neg_$$" | jq -c 'del(.chain)')"

assert_rpc_fails "setJail — missing name" "Fail2Ban" "setJail" \
    "$(jail_json "$OMV_NEW_UUID" "omvtest_neg_$$" | jq -c 'del(.name)')"

assert_rpc_fails "setJail — non-integer maxretry" "Fail2Ban" "setJail" \
    "$(jail_json "$OMV_NEW_UUID" "omvtest_neg_$$" | jq -c '.maxretry = "five"')"

assert_rpc_fails "setJail — invalid uuid" "Fail2Ban" "setJail" \
    "$(jail_json "not-a-uuid" "omvtest_neg_$$")"

assert_rpc_fails "getJail — unknown uuid" "Fail2Ban" "getJail" \
    '{"uuid":"00000000-0000-4000-8000-000000000000"}'

assert_rpc_fails "deleteJail — unknown uuid" "Fail2Ban" "deleteJail" \
    '{"uuid":"00000000-0000-4000-8000-000000000000"}'

assert_rpc_fails "getJailList — missing params" "Fail2Ban" "getJailList" '{}'

# ===========================================================================
section "getStats"
# ===========================================================================

if [ "$ORIG_ENABLE" = "true" ]; then
    if systemctl is-active --quiet fail2ban; then
        assert_rpc "getStats — service enabled" "Fail2Ban" "getStats" '{}' \
            "Number of jail"
    else
        _skip "getStats" "service enabled in config but fail2ban is not running"
    fi
else
    assert_rpc "getStats — service disabled" "Fail2Ban" "getStats" '{}' \
        "Service disabled"
fi

# ===========================================================================
section "Deploy"
# ===========================================================================

if ! command -v omv-salt &>/dev/null; then
    _skip "deploy tests" "omv-salt not available"
elif [ "$ORIG_ENABLE" != "true" ]; then
    _skip "deploy tests" "fail2ban disabled — salt only writes jail files when enabled"
elif [ -z "$JAIL1_UUID" ] || [ -z "$JAIL2_UUID" ]; then
    _skip "deploy tests" "test jails not created"
else
    # Jail 1: chain + logpath.  Jail 2: no chain, empty logpath → systemd.
    rpc "Fail2Ban" "setJail" \
        "$(jail_json "$JAIL1_UUID" "$JAIL1_NAME" "DOCKER-USER" 315360000 "/var/log/auth.log")" \
        &>/dev/null || true
    rpc "Fail2Ban" "setJail" "$(jail_json "$JAIL2_UUID" "$JAIL2_NAME")" &>/dev/null || true

    DEPLOYED=1
    if DEPLOY_OUT=$(omv-salt deploy run fail2ban 2>&1); then
        _pass "omv-salt deploy run fail2ban"
    else
        _fail "omv-salt deploy run fail2ban" "$(echo "$DEPLOY_OUT" | tail -5)"
    fi

    F1="$JAIL_DIR/${JAIL_PREFIX}${JAIL1_UUID}.conf.disabled"
    F2="$JAIL_DIR/${JAIL_PREFIX}${JAIL2_UUID}.conf.disabled"

    if [ -f "$F1" ]; then
        _pass "jail 1 file written with .disabled suffix"
        grep -qx "\[$JAIL1_NAME\]" "$F1" && _pass "jail 1 — section header" \
            || _fail "jail 1 — section header" "$(cat "$F1")"
        grep -qx "enabled = no" "$F1" && _pass "jail 1 — enabled = no" \
            || _fail "jail 1 — enabled = no" "$(cat "$F1")"
        grep -qx "chain = DOCKER-USER" "$F1" && _pass "jail 1 — chain line" \
            || _fail "jail 1 — chain line" "$(cat "$F1")"
        grep -qx "logpath = /var/log/auth.log" "$F1" && _pass "jail 1 — logpath line" \
            || _fail "jail 1 — logpath line" "$(cat "$F1")"
        grep -qx "bantime = 315360000" "$F1" && _pass "jail 1 — large bantime" \
            || _fail "jail 1 — large bantime" "$(cat "$F1")"
    else
        _fail "jail 1 file written with .disabled suffix" "$F1 not found"
    fi

    if [ -f "$F2" ]; then
        _pass "jail 2 file written with .disabled suffix"
        grep -qx "backend = systemd" "$F2" && _pass "jail 2 — empty logpath uses systemd" \
            || _fail "jail 2 — empty logpath uses systemd" "$(cat "$F2")"
        grep -q "^chain" "$F2" && _fail "jail 2 — no chain line when chain empty" "$(cat "$F2")" \
            || _pass "jail 2 — no chain line when chain empty"
    else
        _fail "jail 2 file written with .disabled suffix" "$F2 not found"
    fi

    if command -v fail2ban-client &>/dev/null; then
        if T_OUT=$(fail2ban-client -t 2>&1); then
            _pass "fail2ban-client -t (config test)"
        else
            _fail "fail2ban-client -t (config test)" "$(echo "$T_OUT" | tail -3)"
        fi
    fi

    if systemctl is-active --quiet fail2ban; then
        _pass "fail2ban service active after deploy"
    else
        _fail "fail2ban service active after deploy"
    fi
fi

# ===========================================================================
section "Jail — delete"
# ===========================================================================

for var in JAIL1_UUID JAIL2_UUID; do
    uuid=${!var}
    [ -z "$uuid" ] && continue
    if assert_rpc "deleteJail ($uuid)" "Fail2Ban" "deleteJail" \
            "{\"uuid\":\"$uuid\"}" "\"uuid\":\"$uuid\"" >/dev/null; then
        printf -v "$var" '%s' ""
        assert_rpc_fails "getJail — deleted jail gone" "Fail2Ban" "getJail" \
            "{\"uuid\":\"$uuid\"}"
    fi
done

OUT=$(rpc "Fail2Ban" "getJailList" "$LIST_PARAMS" 2>/dev/null || echo '{"data":[]}')
assert_jq "getJailList — no test jails remain" "$OUT" \
    '[.data[].name | select(startswith("omvtest_"))] | length == 0'

# ===========================================================================
section "Summary"
# ===========================================================================
TOTAL=$((PASS + FAIL))
echo >&2
echo -e "  Results: ${GREEN}${PASS} passed${NC}, ${RED}${FAIL} failed${NC}, ${YELLOW}${SKIP} skipped${NC} (${TOTAL} total)" >&2
if [ ${#FAILED_TESTS[@]} -gt 0 ]; then
    echo -e "\n  ${RED}Failed tests:${NC}" >&2
    for t in "${FAILED_TESTS[@]}"; do
        echo -e "    ${RED}✗${NC} $t" >&2
    done
fi
echo >&2

[ $FAIL -eq 0 ] && exit 0 || exit 1
