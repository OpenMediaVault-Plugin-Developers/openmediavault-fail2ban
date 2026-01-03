# @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
# @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
# @copyright Copyright (c) 2019-2026 openmediavault plugin developers
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

{% set config = salt['omv_conf.get']('conf.service.fail2ban') %}
{% set script_prefix = salt['pillar.get']('default:OMV_FAIL2BAN_JAIL_PREFIX', 'openmediavault-') %}
{% set scripts_dir = salt['pillar.get']('default:OMV_FAIL2BAN_JAIL_DIR', '/etc/fail2ban/jail.d') %}

{% if config.enable | to_bool %}

remove_default_fail2ban_config:
  file.absent:
    - name: "/etc/fail2ban/jail.d/defaults-debian.conf"

configure_fail2ban:
  file.managed:
    - name: "/etc/fail2ban/jail.conf"
    - source:
      - salt://{{ tpldir }}/files/etc-fail2ban-jail_conf.j2
    - template: jinja
    - context:
        config: {{ config | json }}
    - user: root
    - group: root
    - mode: 644

remove_jail_conf_files:
  module.run:
    - file.find:
      - path: "{{ scripts_dir }}"
      - iname: "{{ script_prefix }}*"
      - delete: "f"

{% for jail in config.jails.jail %}

configure_fail2ban_jail_{{ jail.uuid }}:
  file.managed:
    - name: "{{ scripts_dir | path_join(script_prefix ~ jail.uuid) }}.conf{{ '' if jail.enable else '.disabled' }}"
    - source:
      - salt://{{ tpldir }}/files/etc-fail2ban-jail_d-jail_conf.j2
    - context:
        jail: {{ jail | json }}
    - template: jinja
    - user: root
    - group: root
    - mode: 644

{% endfor %}

start_fail2ban_service:
  service.running:
    - name: fail2ban
    - enable: True
    - watch:
      - file: configure_fail2ban
      - file: configure_fail2ban_jail_*

{% else %}

stop_fail2ban_service:
  service.dead:
    - name: fail2ban
    - enable: False

{% endif %}
