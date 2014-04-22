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
// require("js/omv/Rpc.js")
// require("js/omv/data/Store.js")
// require("js/omv/data/Model.js")

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
        "OMV.window.MessageBox"
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
        dataIndex : "jailName",
        stateId   : "jailName"
    },{
        text      : _("Enabled"),
        sortable  : false,
        dataIndex : "jailEnabled",
        stateId   : "jailEnabled",
        renderer  : function(value) {
            return value ? "Yes" : "No";
        }
    },{
        text      : _("Port(s)"),
        sortable  : false,
        dataIndex : "jailPort",
        stateId   : "jailPort"        
    },{
        text      : _("Max Retry"),
        sortable  : false,
        dataIndex : "jailMaxRetry",
        stateId   : "jailMaxRetry"
    },{
        text      : _("Ban Time"),
        sortable  : false,
        dataIndex : "jailBanTime",
        stateId   : "jailBanTime"
    },{
        text      : _("Action"),
        sortable  : false,
        dataIndex : "jailAction",
        stateId   : "jailAction"
    },{
        text      : _("Ban Action"),
        sortable  : false,
        dataIndex : "jailBanAction",
        stateId   : "jailBanAction"
    },{
        text      : _("Destination Email"),
        sortable  : false,
        dataIndex : "jailDesteMail",
        stateId   : "jailDesteMail"
    },{
        text      : _("Ignore IP"),
        sortable  : false,
        dataIndex : "jailIgnoreIP",
        stateId   : "jailIgnoreIP"
    },{
        text      : _("Filter"),
        sortable  : false,
        dataIndex : "jailFilter",
        stateId   : "jailFilter"
    },{
        text      : _("Log Path"),
        sortable  : false,
        dataIndex : "jailLogPath",
        stateId   : "jailLogPath"
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
                        name : "jailName",
                        type : "string"
                    },{
                        name : "jailEnabled",
                        type : "string"
                    },{
                        name : "jailPort",
                        type : "string"
                    },{
                        name : "jailMaxRetry",
                        type : "string"
                    },{
                        name : "jailBanTime",
                        type : "string"
                    },{
                        name : "jailAction",
                        type : "string"
                    },{
                        name : "jailBanAction",
                        type : "string"
                    },{
                        name : "jailDesteMail",
                        type : "string"
                    },{
                        name : "jailIgnoreIP",
                        type : "string"
                    },{
                        name : "jailFilter",
                        type : "string"
                    },{
                        name : "jailLogPath",
                        type : "string"
                    }]

                }),
                proxy    : {
                    type    : "rpc",
                    rpcData : {
                        service : "Fail2ban",
                        method  : "getJails"
                    }
                }
            })
        });

        me.callParent(arguments);
    },


    doReload : function() {
        var me = this;

        me.store.reload();
    }

});


OMV.WorkspaceManager.registerPanel({
    id        : "jails",
    path      : "/service/fail2ban",
    text      : _("Jails"),
    position  : 20,
    className : "OMV.module.admin.service.fail2ban.Jails"
});
