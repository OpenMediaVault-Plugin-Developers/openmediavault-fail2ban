/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2014-2018 OpenMediaVault Plugin Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/form/Panel.js")

Ext.define("OMV.module.admin.service.fail2ban.Settings", {
    extend: "OMV.workspace.form.Panel",

    rpcService: "Fail2ban",
    rpcGetMethod: "getSettings",
    rpcSetMethod: "setSettings",

    getFormItems: function() {
        return [{
            xtype: "fieldset",
            title: _("General settings"),
            fieldDefaults: {
                labelSeparator: ""
            },
            items: [{
                xtype: "checkbox",
                name: "enable",
                fieldLabel: _("Enable"),
                checked: false
            },{
                xtype: "textfield",
                name: "ignoreip",
                fieldLabel: _("Ignoreip"),
                allowBlank: false,
                value: "127.0.0.1",
                plugins: [{
                    ptype: "fieldinfo",
                    text: _("Can be an IP address, a CIDR mask or a DNS host, separe with space for multiple ip.")
                }]
            },{
                xtype: "textfield",
                name: "findtime",
                fieldLabel: _("Findtime"),
                allowBlank: false,
                value: "604800",
                plugins: [{
                    ptype: "fieldinfo",
                    text: _("The counter is set to zero if no match is found within 'findtime' seconds. (604800 = 1 week)")
                }]
            },{
                xtype: "textfield",
                name: "bantime",
                fieldLabel: _("Bantime"),
                allowBlank: false,
                value: "604800",
                plugins: [{
                    ptype: "fieldinfo",
                    text: _("Duration (in seconds) for IP to be banned for. Negative number for 'permanent' ban. (604800 = 1 week)")
                }]
            },{
                xtype: "textfield",
                name: "maxretry",
                fieldLabel: _("Maxretry"),
                allowBlank: false,
                value: "3",
                plugins: [{
                    ptype: "fieldinfo",
                    text: _("Number of matches (i.e. value of the counter) which triggers ban action on the IP.")
                }]
            },{
                xtype: "textfield",
                name: "destemail",
                fieldLabel: _("Destemail"),
                allowBlank: false,
                value: "root@localhost",
                plugins: [{
                    ptype: "fieldinfo",
                    text: _("An email address to receive ban messages.")
                }]
            },{
                xtype: "textfield",
                name: "action",
                fieldLabel: _("Action"),
                allowBlank: false,
                value: "action_mwl",
                plugins: [{
                    ptype: "fieldinfo",
                    text: _("action_= ban only / action_mw=ban & send an email / action_mwl=ban & send an email with whois report and log lines.")
                }]
            }]
        }];
    }
});

OMV.WorkspaceManager.registerPanel({
    id: "settings",
    path: "/service/fail2ban",
    text: _("Settings"),
    position: 10,
    className: "OMV.module.admin.service.fail2ban.Settings"
});
