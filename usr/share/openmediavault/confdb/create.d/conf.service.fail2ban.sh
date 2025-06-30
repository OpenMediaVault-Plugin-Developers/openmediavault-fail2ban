#!/bin/sh
#
# @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
# @author    Volker Theile <volker.theile@openmediavault.org>
# @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
# @copyright Copyright (c) 2009-2013 Volker Theile
# @copyright Copyright (c) 2013-2025 openmediavault plugin developers
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

set -e

. /usr/share/openmediavault/scripts/helper-functions

SERVICE_XPATH_NAME="fail2ban"
SERVICE_XPATH="/config/services/${SERVICE_XPATH_NAME}"

if ! omv_config_exists "${SERVICE_XPATH}"; then
    omv_config_add_node "/config/services" "${SERVICE_XPATH_NAME}"
    omv_config_add_key "${SERVICE_XPATH}" "enable" "0"
    omv_config_add_key "${SERVICE_XPATH}" "ignoreip" "127.0.0.1"
    omv_config_add_key "${SERVICE_XPATH}" "findtime" "604800"
    omv_config_add_key "${SERVICE_XPATH}" "bantime" "604800"
    omv_config_add_key "${SERVICE_XPATH}" "maxretry" "3"
    omv_config_add_key "${SERVICE_XPATH}" "destemail" "root@localhost"
    omv_config_add_key "${SERVICE_XPATH}" "action" "action_mwl"
    omv_config_add_node "${SERVICE_XPATH}" "jails"
fi

if ! omv_config_exists "${SERVICE_XPATH}/jails/jail[uuid='36b96e6c-9187-4b93-b0c6-05c6d3e29dc3']"; then
     object="<uuid>36b96e6c-9187-4b93-b0c6-05c6d3e29dc3</uuid>"
     object="${object}<enable>0</enable>"
     object="${object}<name>ssh</name>"
     object="${object}<port>ssh</port>"
     object="${object}<maxretry>3</maxretry>"
     object="${object}<bantime>-1</bantime>"
     object="${object}<filter>sshd</filter>"
     object="${object}<logpath></logpath>"
     omv_config_add_node_data "${SERVICE_XPATH}/jails" "jail" "${object}"
 fi

 if ! omv_config_exists "${SERVICE_XPATH}/jails/jail[uuid='59650e01-5e07-4076-9b15-ce352f4b4356']"; then
     object="<uuid>59650e01-5e07-4076-9b15-ce352f4b4356</uuid>"
     object="${object}<enable>0</enable>"
     object="${object}<name>sshd-ddos</name>"
     object="${object}<port>ssh</port>"
     object="${object}<maxretry>3</maxretry>"
     object="${object}<bantime>-1</bantime>"
     object="${object}<filter>sshd</filter>"
     object="${object}<logpath></logpath>"
     omv_config_add_node_data "${SERVICE_XPATH}/jails" "jail" "${object}"
 fi

if ! omv_config_exists "${SERVICE_XPATH}/jails/jail[uuid='6e3a7d25-326c-4dc8-bc05-63f303a62b21']"; then
    object="<uuid>6e3a7d25-326c-4dc8-bc05-63f303a62b21</uuid>"
    object="${object}<enable>0</enable>"
    object="${object}<name>proftp</name>"
    object="${object}<port>ftp,ftp-data,ftps,ftps-data</port>"
    object="${object}<maxretry>3</maxretry>"
    object="${object}<bantime>-1</bantime>"
    object="${object}<filter>proftpd</filter>"
    object="${object}<logpath>/var/log/proftpd/proftpd.log</logpath>"
    omv_config_add_node_data "${SERVICE_XPATH}/jails" "jail" "${object}"
fi

if ! omv_config_exists "${SERVICE_XPATH}/jails/jail[uuid='7e9a7d35-326c-4dc8-bc05-35f308a62b78']"; then
    object="<uuid>7e9a7d35-326c-4dc8-bc05-35f308a62b78</uuid>"
    object="${object}<enable>0</enable>"
    object="${object}<name>owncloud</name>"
    object="${object}<port>http,https,8443</port>"
    object="${object}<maxretry>3</maxretry>"
    object="${object}<bantime>-1</bantime>"
    object="${object}<filter>owncloud</filter>"
    object="${object}<logpath>/MyPath/owncloud.log</logpath>"
    omv_config_add_node_data "${SERVICE_XPATH}/jails" "jail" "${object}"
fi

if ! omv_config_exists "${SERVICE_XPATH}/jails/jail[uuid='4e3a2d25-326c-4dc8-bc05-22f303a62b75']"; then
    object="<uuid>4e3a2d25-326c-4dc8-bc05-22f303a62b75</uuid>"
    object="${object}<enable>0</enable>"
    object="${object}<name>nginx-404</name>"
    object="${object}<port>http,https</port>"
    object="${object}<maxretry>3</maxretry>"
    object="${object}<bantime>-1</bantime>"
    object="${object}<filter>nginx-404</filter>"
    object="${object}<logpath>/var/log/nginx*/*access*.log</logpath>"
    omv_config_add_node_data "${SERVICE_XPATH}/jails" "jail" "${object}"
fi

if ! omv_config_exists "${SERVICE_XPATH}/jails/jail[uuid='5f2b2d25-726c-5dc8-ac05-79f303a62b35']"; then
    object="<uuid>5f2b2d25-726c-5dc8-ac05-79f303a62b35</uuid>"
    object="${object}<enable>0</enable>"
    object="${object}<name>omv-webgui</name>"
    object="${object}<port>http,https</port>"
    object="${object}<maxretry>3</maxretry>"
    object="${object}<bantime>-1</bantime>"
    object="${object}<filter>omv-webgui</filter>"
    object="${object}<logpath></logpath>"
    omv_config_add_node_data "${SERVICE_XPATH}/jails" "jail" "${object}"
fi

if ! omv_config_exists "${SERVICE_XPATH}/jails/jail[uuid='6f0cb653-d022-4edb-b68b-a8be4fe2e484']"; then
    object="<uuid>6f0cb653-d022-4edb-b68b-a8be4fe2e484</uuid>"
    object="${object}<enable>0</enable>"
    object="${object}<name>samba</name>"
    object="${object}<port>139,445</port>"
    object="${object}<maxretry>3</maxretry>"
    object="${object}<bantime>-1</bantime>"
    object="${object}<filter>samba</filter>"
    object="${object}<logpath>/var/log/samba/auth_json_audit.log</logpath>"
# logpath depends on the samba's extra option as documented in samba filter file
    omv_config_add_node_data "${SERVICE_XPATH}/jails" "jail" "${object}"
fi

exit 0
