<?php

/**
 * Copyright (C) 2014 OpenMediaVault Plugin Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

require_once "openmediavault/object.inc";
require_once "openmediavault/error.inc";
require_once "openmediavault/util.inc";
require_once "openmediavault/rpc.inc";
require_once "openmediavault/notify.inc";

class OMVRpcServiceFail2ban extends OMVRpcServiceAbstract
{

    private function getPluginName()
    {
        return strtolower($this->getName());
    }

    private function getEventMessagePath()
    {
        return sprintf("org.openmediavault.service.%s", $this->getPluginName());
    }

    private function getXPath()
    {
        return sprintf("//services/%s", $this->getPluginName());
    }

    public function getName()
    {
        return "Fail2ban";
    }

    public function initialize()
    {
        $this->registerMethod("getSettings");
        $this->registerMethod("setSettings");
        $this->registerMethod("getJailList");
        $this->registerMethod("getJail");
 	$this->registerMethod("setJail");
        $this->registerMethod("deleteJail");
    }

    public function getSettings($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));

        // Get configuration object
        $object = $xmlConfig->get($this->getXPath());

        if (is_null($object))
	{
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPath());
        }

        // Modify result data        
        $object["enable"] = boolvalEx($object["enable"]);
        $object["findtime"] = intval($object["findtime"]);
        $object["bantime"] = intval($object["bantime"]);
        $object["maxretry"] = intval($object["maxretry"]);

        return $object;
    }

    public function setSettings($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "enable"    : { "type" : "boolean" },
                "ignoreip"  : { "type" : "string"},
                "findtime"  : { "type" : "string"},
                "bantime"   : { "type" : "string"},
                "maxretry"  : { "type" : "string"},
                "destemail" : { "type" : "string"},
                "action"    : { "type" : "string"}
            }
        }');

        // Prepare configuration data
        $object = array(
            "enable" => array_boolval($params, "enable"),
            "ignoreip" => $params['ignoreip'],
            "findtime" => $params['findtime'],
            "bantime" => $params['bantime'],
            "maxretry" => $params['maxretry'],
            "destemail" => $params['destemail'],
            "action" => $params['action']
        );

        // Set configuration object
        if (false === $xmlConfig->replace($this->getXPath(), $object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED,
                $this->getXPath());

        // Notify configuration changes
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(OMV_NOTIFY_MODIFY, $this->getEventMessagePath(), $object);

        return $object;
    }


    /**
     * Get list of jail configuration objects.
     * @param params An array containing the following fields:
     *   \em start The index where to start.
     *   \em limit The number of objects to process.
     *   \em sortfield The name of the column used to sort.
     *   \em sortdir The sort direction, ASC or DESC.
     * @param context The context of the caller.
     * @return An array containing the requested objects. The field \em total
     *   contains the total number of objects, \em data contains the object
     *   array. An exception will be thrown in case of an error.
     * @throw E_CONFIG_GET_OBJECT_FAILED
     */
    public function getJailList($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));

        // Validate the parameters of the RPC service method.       
        $this->validateMethodParams($params, '{
            "type":"object",
            "properties":{
                        "start":{"type":"integer"},
                        "limit":{'.$GLOBALS['OMV_JSONSCHEMA_COUNTFIELD'].'},
                        "sortfield":{'.$GLOBALS['OMV_JSONSCHEMA_SORTFIELD'].'},
                        "sortdir":{'.$GLOBALS['OMV_JSONSCHEMA_SORTDIR'].'}
                         }
        }');

        // Get the configuration objects.
        $xpath = "//services/fail2ban/jails/jail";           
        $objects = $xmlConfig->getList($xpath);

        if(is_null($objects)) {
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,$xpath);
        }

        // Add additional share informations.
        foreach($objects as $objectk => &$objectv) {
                // Modify the result object.
                $objectv['enable'] = boolvalEx($objectv['enable']);
                $objectv['port'] = intval($objectv['port']);
                $objectv['maxretry'] = intval($objectv['maxretry']);
                $objectv['bantime'] = intval($objectv['bantime']);

//                // Get the shared folder configuration object.
//                $sfobject = $xmlConfig->get("//system/fail2ban/jails[" .
//                        "uuid='{$objectv['jailref']}']");
//                if(!is_null($sfobject) && !empty($sfobject)) {
//                      $objectv['sharedfoldername'] = $sfobject['name'];  
//                }                  
        }

        // Filter the result object list.
        return $this->applyFilter($objects, $params['start'],$params['limit'], $params['sortfield'], $params['sortdir']);
    }


    /**
     * Get a jail configuration object.
     * @param params An array containing the following fields:
     * \em uuid The UUID of the configuration object.
     * @param context The context of the caller.
     * @return The requested configuration object.
     * @throw E_CONFIG_GET_OBJECT_FAILED
     */
    public function getJail($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
           "type":"object",
           "properties":{
               "uuid":{'.$GLOBALS['OMV_JSONSCHEMA_UUID'].'}
           }
        }');

        // Get the configuration object.
        $xpath = sprintf("//services/fail2ban/jails/jail[uuid='%s']",$params['uuid']);
        $object = $xmlConfig->get($xpath);

        if(is_null($object)) {
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,$xpath);
        }

        // Modify the result object.
	$object['enable'] = boolvalEx($object['enable']);
        return $object;
    }

    /**
     * Set a jail configuration object.
     * @param params The method parameters.
     * @param context The context of the caller.
     * @return The stored configuration object.
     * @throw E_CONFIG_OBJECT_UNIQUENESS
     * @throw E_CONFIG_SET_OBJECT_FAILED
     */
    public function setJail($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
             "type":"object",
             "properties":{
                  "uuid":{'.$GLOBALS['OMV_JSONSCHEMA_UUID_UNDEFINED'].'},
                  "enable":{"type":"boolean"},
                  "name":{"type":"string"},
                  "port":{"type":"string"},
                  "maxretry":{"type":"string"},
                  "bantime":{"type":"string"},
                  "filter":{"type":"string"},
                  "logpath":{"type":"string"}
              }
        }');

        // Prepare the configuration object.
        $object = array(
             "uuid" => ($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) ? OMVUtil::uuid() : $params['uuid'],
             "enable" => array_boolval($params, 'enable'),
             "name" => $params['name'],
             "port" => $params['port'],
             "maxretry" => $params['maxretry'],
             "bantime" => $params['bantime'],
             "filter" => $params['filter'],
             "logpath" => $params['logpath'] 
        );

        // Set the configuration object.
        $success = FALSE;
        if ($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) {
             // Check uniqueness:
             // - Share name
             // - Shared folder
             $xpath = sprintf("//services/fail2ban/jails/jail[name='%s']",
             $params['name']);

             if (TRUE === $xmlConfig->exists($xpath)) {
                 throw new OMVException(OMVErrorMsg::E_CONFIG_OBJECT_UNIQUENESS,
                 sprintf(gettext("A jail with the name '%s' already exists"),
                 $params['name']));
             }

             // Append object to the configuration.
             $success = $xmlConfig->set("//services/fail2ban/jails",array("jail" => $object));

        }
        else
        {
             // Update existing configuration object.
             $success = $xmlConfig->replace("//services/fail2ban/jails/jail[" .
                 "uuid='{$params['uuid']}']", $object);
        }

        if (FALSE === $success) {
            throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED);
        }

        // Notify configuration changes.
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) ?
            OMV_NOTIFY_CREATE : OMV_NOTIFY_MODIFY,
            "org.openmediavault.services.fail2ban.jails.jail", $object);

        // Return the configuration object.
        return $object;
}

    /**
     * Delete a share configuration object.
     * @param params An array containing the following fields:
     * \em uuid The UUID of the configuration object. 
     * @param context The context of the caller.
     * @return The deleted configuration object.
     */
     public function deleteJail($params, $context) {

         // Validate the RPC caller context.
         $this->validateMethodContext($context, array(
             "role" => OMV_ROLE_ADMINISTRATOR
         ));

         // Validate the parameters of the RPC service method.
         $this->validateMethodParams($params, '{
             "type":"object",
             "properties":{
                 "uuid":{'.$GLOBALS['OMV_JSONSCHEMA_UUID'].'}
             }
         }');

         // Delete the configuration object.
         $xpath = sprintf("//services/fail2ban/jails/jail[uuid='%s']",$params['uuid']);
         return $this->deleteConfigObjectByPath($xpath,"org.openmediavault.services.fail2ban.jails.jail");
    }

    /**
     * Get statistics.
     * @param params The method parameters.
     * @param context The context of the caller.
     * @return A string containing the requested statistics.
     * @throw E_CONFIG_GET_OBJECT_FAILED
     * @throw E_EXEC_FAILED
     */
    public function getStats($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
	$this->validateMethodContext($context, array(
		"role" => OMV_ROLE_ADMINISTRATOR
	));

	// Is service enabled?
	$xpath = "//services/fail2ban";
	$object = $xmlConfig->get($xpath);

	if(is_null($object)) {
		throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,$xpath);
	}
	
        if (!boolvalEx($object['enable'])) {
	     $stats = gettext("Service disabled");
	} else {
	     $cmd = "fail2ban-client status 2>&1";

	     if(0 !== $this->exec($cmd, $output)) {
	          throw new OMVException(OMVErrorMsg::E_EXEC_FAILED,
		  $cmd, implode("\n", $output));
	     }

	     array_shift($output);
	     array_shift($output);
	     $stats = implode("\n", $output);
	}
	
        return $stats;
    }

}

// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance();
$rpcServiceMgr->registerService(new OMVRpcServiceFail2ban());
