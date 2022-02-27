#!/bin/sh

set -e

. /usr/share/openmediavault/scripts/helper-functions

xpath="/config/services/fail2ban/jails/jail"

jail="${xpath}[uuid='59650e01-5e07-4076-9b15-ce352f4b4356']"
if omv_config_exists "${jail}"; then
  echo "Updating sshd-dos jail..."
  omv_config_update "${jail}/name" "sshd-ddos"
  omv_config_update "${jail}/filter" "sshd"
fi

jail="${xpath}[uuid='645ae684-0950-4fcf-92fc-eba1b88775b1']"
if omv_config_exists "${jail}"; then
  echo "Removing apache-noscript jail..."
  omv_config_delete "${jail}"
fi

jail="${xpath}[uuid='6e3a7d25-326c-4dc8-bc05-63f303a62b60']"
if omv_config_exists "${jail}"; then
  echo "Removing apache-404 jail..."
  omv_config_delete "${jail}"
fi

exit 0
