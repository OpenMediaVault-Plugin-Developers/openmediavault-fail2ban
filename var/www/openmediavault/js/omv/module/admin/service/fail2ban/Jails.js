/**
 * Copyright (C) 2013-2014 OpenMediaVault Plugin Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/grid/Panel.js")
// require("js/omv/workspace/window/Form.js")
// require("js/omv/workspace/window/plugin/ConfigObject.js")
// require("js/omv/Rpc.js")
// require("js/omv/data/Store.js")
// require("js/omv/data/Model.js")
// require("js/omv/data/proxy/Rpc.js")
// require("js/omv/form/field/SharedFolderComboBox.js")
// require("js/omv/window/Execute.js")

Ext.define("OMV.module.admin.service.fail2ban.Jail", {
	extend: "OMV.workspace.window.Form",
	requires: [
	    "OMV.workspace.window.plugin.ConfigObject",
		"OMV.window.Execute"
	],

	rpcService: "Fail2ban",
	rpcGetMethod: "getJail",
	rpcSetMethod: "setJail",
	plugins: [{
		ptype: "configobject"
	}],
	width: 700,
	height: 400,

	/**
	 * The class constructor.
	 * @fn constructor
	 * @param uuid The UUID of the database/configuration object. Required.
	 */

	getFormItems: function() {
		var me = this;
		return [{
			xtype: "checkbox",
			name: "enable",
			fieldLabel: _("Enable"),
			checked: true
		},{
			xtype: "textfield",
			name: "name",
			fieldLabel: _("Name"),
			allowBlank: false,
			vtype: "name",
			plugins: [{
				ptype: "fieldinfo",
				text: _("The name of the jail.")
			}]
		},{
			xtype: "textfield",
			name: "port",
			fieldLabel: _("Port"),
			allowBlank: false,
			vtype: "port",
			plugins: [{
				ptype: "fieldinfo",
				text: _("The port of the jail.")
			}]
		},{
			xtype: "textfield",
			name: "maxretry",
			fieldLabel: _("Max Retry"),
                        allowBlank: false,
                        vtype: "maxretry",
                        plugins: [{
                                ptype: "fieldinfo",
                                text: _("Max Retry in seconds.")
                        }]
                },{
                        xtype: "textfield",
                        name: "bantime",
                        fieldLabel: _("Ban Time"),
                        allowBlank: false,
                        vtype: "bantime",
                        plugins: [{
                                ptype: "fieldinfo",
                                text: _("Ban time in seconds.")
                        }]

                },{
                        xtype: "textfield",
                        name: "filter",
                        fieldLabel: _("Filter"),
                        allowBlank: false,
                        vtype: "filter",
                        plugins: [{
                                ptype: "fieldinfo",
                                text: _("A filter defines a regular expression which must match a pattern corresponding to a log-in failure or any other expression.")
                        }]
                },{
                        xtype: "textfield",
                        name: "logpath", 
                        fieldLabel: _("Log Path"), 
                        allowBlank: false,
                        vtype: "logpath", 
                        plugins: [{
                                ptype: "fieldinfo",
                                text: _("The log file to be scanned by the filter.") 
                        }]
		}];
	}
});



/**
 * @class OMV.module.admin.service.fail2ban.Jails
 * @derived OMV.workspace.grid.Panel
 */
Ext.define("OMV.module.admin.service.fail2ban.Jails", {
    extend   : "OMV.workspace.grid.Panel",
    requires : [
        "OMV.Rpc",
        "OMV.data.Store",
        "OMV.data.Model",
        "OMV.data.proxy.Rpc"
    ],
    uses : [
        "OMV.modules.admin.service.fail2ban.Jail"
    ],

    hideAddButton       : false,
    hideEditButton      : false,
    hideUpButton        : true,
    hideDownButton      : true,
    hideApplyButton     : false,
    hideRefreshButton   : false,

    hidePagingToolbar : false,
    stateful          : true,
    stateId           : "a982a76d-e8a5-4a65-d841-5748da327abd",
    columns         : [{
        text      : _("Jail Name"),
        sortable  : true,
        dataIndex : "name",
        stateId   : "name"
    },{
        xtype     : "booleaniconcolumn",
        text      : _("Enabled"),
        sortable  : false,
        dataIndex : "enabled",
        stateId   : "enabled",
        align     : "center",
        with      : 80,
        resizable : false,
        trueIcon  : "switch_on.png",
        falseIcon: "switch_off.png"
    },{
        text      : _("Port(s)"),
        sortable  : true,
        dataIndex : "port",
        stateId   : "port"        
    },{
        text      : _("Max Retry"),
        sortable  : false,
        dataIndex : "maxretry",
        stateId   : "maxretry"
    },{
        text      : _("Ban Time"),
        sortable  : false,
        dataIndex : "bantime",
        stateId   : "bantime"
    },{
        text      : _("Filter"),
        sortable  : false,
        dataIndex : "filter",
        stateId   : "filter"
    },{
        text      : _("Log Path"),
        sortable  : false,
        dataIndex : "logpath",
        stateId   : "logpath"
    }],

    initComponent : function () {
        var me = this;

        Ext.apply(me, {
            store : Ext.create("OMV.data.Store", {
                autoLoad : true,
                model    : OMV.data.Model.createImplicit({
                    idProperty : "uuid",
                    fields     : [{
                        name : "uuid",
                        type : "string"
                    },{
                        name : "name",
                        type : "string"
                    },{
                        name : "enabled",
                        type : "string"
                    },{
                        name : "port",
                        type : "string"
                    },{
                        name : "maxretry",
                        type : "string"
                    },{
                        name : "bantime",
                        type : "string"
                    },{
                        name : "filter",
                        type : "string"
                    },{
                        name : "logpath",
                        type : "string"
                    }]

                }),
                proxy    : {
                    type    : "rpc",
                    rpcData : {
                        service : "Fail2ban",
                        method  : "getJailList"
                    }
                },
                remoteSort : true,
                sorters    : [{
                               direction : "ASC",
                               property  : "name"
                }]
            })
        });

        me.callParent(arguments);
    },

    onAddButton: function() {
        var me = this;
        Ext.create("OMV.module.admin.service.fail2ban.Jail", {
            title: _("Add jail"),
            uuid: OMV.UUID_UNDEFINED,
            listeners: { 
                scope: me,
                submit: function() {
                    this.doReload();
                }
            }
        }).show();
    },

    onEditButton: function() {
        var me = this;
        var record = me.getSelected();
        Ext.create("OMV.module.admin.service.fail2ban.Jail", {
            title: _("Edit jail"),
            uuid: record.get("uuid"),
            listeners: {
                scope: me,
                submit: function() {
                    this.doReload();
                }
            }
        }).show();
    },

    doDeletion: function(record) {
        var me = this;
        OMV.Rpc.request({
            scope: me,
            callback: me.onDeletion,
            rpcData: {
                service: "Fail2Ban",
                method: "deleteJail",
                params: {
                    uuid: record.get("uuid")
                }
            }
        });
    } 

});


OMV.WorkspaceManager.registerPanel({
    id        : "jails",
    path      : "/service/fail2ban",
    text      : _("Jails"),
    position  : 20,
    className : "OMV.module.admin.service.fail2ban.Jails"
});
