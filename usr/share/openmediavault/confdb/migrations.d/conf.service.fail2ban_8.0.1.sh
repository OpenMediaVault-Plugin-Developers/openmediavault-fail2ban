#!/bin/sh

set -e

. /usr/share/openmediavault/scripts/helper-functions

xpath="/config/services/fail2ban/jails/jail"

xmlstarlet sel -t -m "${xpath}" -v "uuid" -n ${OMV_CONFIG_FILE} |
    xmlstarlet unesc |
    while read uuid; do
        if ! omv_config_exists "${xpath}[uuid='${uuid}']/chain"; then
            omv_config_add_key "${xpath}[uuid='${uuid}']" "chain" ""
        fi
    done

exit 0
