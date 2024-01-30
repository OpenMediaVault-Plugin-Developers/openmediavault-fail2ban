#!/bin/sh

set -e

. /usr/share/openmediavault/scripts/helper-functions

xpath="/config/services/fail2ban/jails/jail"

jail="${xpath}[uuid='36b96e6c-9187-4b93-b0c6-05c6d3e29dc3']"
if omv_config_exists "${jail}"; then
  echo "Updating ssh jail..."
  omv_config_update "${jail}/logpath" ""
fi

jail="${xpath}[uuid='59650e01-5e07-4076-9b15-ce352f4b4356']"
if omv_config_exists "${jail}"; then
  echo "Updating sshd-dos jail..."
  omv_config_update "${jail}/logpath" ""
fi

jail="${xpath}[uuid='5f2b2d25-726c-5dc8-ac05-79f303a62b35']"
if omv_config_exists "${jail}"; then
  echo "Updating omv-webgui jail..."
  omv_config_update "${jail}/logpath" ""
fi

exit 0
