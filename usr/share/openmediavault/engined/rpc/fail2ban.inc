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

class OMVRpcServiceFail2ban extends \OMV\Rpc\ServiceAbstract
{
    public function getName()
    {
        return "Fail2Ban";
    }

    public function initialize()
    {
        $this->registerMethod("getSettings");
        $this->registerMethod("setSettings");
        $this->registerMethod("getJailList");
	$this->registerMethod("setJail");
        $this->registerMethod("getJail");
        $this->registerMethod("deleteJail");
	$this->registerMethod("getStats");
    }

    public function getSettings($params, $context)
    {
	// OLD
        //global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, [
            "role" => OMV_ROLE_ADMINISTRATOR
        ]);

        // Get configuration object
	$db = \OMV\Config\Database::getInstance();
	$object = $db->get("conf.service.fail2ban");
		
	// Remove useless properties from the object.
	$object->remove("jails");
		
	return $object->getAssoc();
		
/*		
	// OLD
        $xpath = "//services/fail2ban";
        $object = $xmlConfig->get($xpath);

        if (is_null($object))
	{
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $xpath);
        }

        // Modify result data        
        $object["enable"] = boolvalEx($object["enable"]);
        $object["findtime"] = intval($object["findtime"]);
        $object["bantime"] = intval($object["bantime"]);
        $object["maxretry"] = intval($object["maxretry"]);

        unset($object['jails']);

        return $object;
*/
    }

    public function setSettings($params, $context)
   {
	// OLD
	//global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, [
            "role" => OMV_ROLE_ADMINISTRATOR
        ]);

	// Validate the parameters of the RPC service method.
	$this->validateMethodParams($params, "rpc.fail2ban.setsettings");
	
	// Get the existing configuration object.
	$db = \OMV\Config\Database::getInstance();
	$object = $db->get("conf.service.fail2ban");
	$object->setAssoc($params);
	// Set the configuration object.
	$db->set($object);
	// Remove useless properties from the object.
	$object->remove("jails");

	// Return the configuration object.
	return $object->getAssoc();		
/*		
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "enable"    : { "type" : "boolean"},
                "ignoreip"  : { "type" : "string"},
                "findtime"  : { "type" : "string"},
                "bantime"   : { "type" : "string"},
                "maxretry"  : { "type" : "string"},
                "destemail" : { "type" : "string"},
                "action"    : { "type" : "string"}
            }
        }');

	// Get the existing configuration object.
	$xpath = "//services/fail2ban";
	$object = $xmlConfig->get($xpath);
	
        if(is_null($object))
        {
	     throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                                    $xpath);
	}

	// Update the configuration object.
	$object = array_merge($object, array(
            "enable" => array_boolval($params, "enable"),
            "ignoreip" => $params['ignoreip'],
            "findtime" => $params['findtime'],
            "bantime" => $params['bantime'],
            "maxretry" => $params['maxretry'],
            "destemail" => $params['destemail'],
            "action" => $params['action']
        ));

        // Set configuration object
        if (false === $xmlConfig->replace($xpath, $object))
        {   
            throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED,
                                   $xpath);
        }

        // Notify configuration changes
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(OMV_NOTIFY_MODIFY, 
                            "org.openmediavault.services.fail2ban", 
                            $object);

        return $object;
*/
    }

    /**
     * Get list of Jail config objects.
     * @param params An array containing the Jails object:
     * @param context The context of the caller.
     * @return An array containing the requested objects.
     * An exception will be thrown in case of an error.
     * @throw E_CONFIG_GET_OBJECT_FAILED
     */
     public function getJailList($params, $context)
     {
//	 global $xmlConfig;

	 // Validate the RPC caller context.
	 $this->validateMethodContext($context, [
			  "role" => OMV_ROLE_ADMINISTRATOR
	 ]);

	 // Validate the parameters of the RPC service method.
	$this->validateMethodParams($params, "rpc.common.getlist");
	// Get the configuration object.
	$db = \OMV\Config\Database::getInstance();
	$objects = $db->get("conf.service.fail2ban.jail");
	
	
/*	 
	 // Validate the parameters of the RPC service method.
	 $this->validateMethodParams($params, '{
			  "type":"object",
			  "properties":{
				  "start"    :{"type":"integer"},
				  "limit"    :{'.$GLOBALS['OMV_JSONSCHEMA_COUNTFIELD'].'},
				  "sortfield":{'.$GLOBALS['OMV_JSONSCHEMA_SORTFIELD'].'},
				  "sortdir"  :{'.$GLOBALS['OMV_JSONSCHEMA_SORTDIR'].'}
			  }
	 }');

	 // Get the list of configuration objects.
	 $xpath = "//services/fail2ban/jails/jail";
	 $objects = $xmlConfig->getList($xpath);

	 if(is_null($objects))
         {
	       throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
			  $xpath);
	  }

	  // Modify the result objects.
	  foreach($objects as $objectk => &$objectv) {
			$objectv['enable']   = boolvalEx($objectv['enable']);
			$objectv['maxretry'] = intval($objectv['maxretry']);
 			$objectv['bantime']  = intval($objectv['bantime']);
	   }

	   // Filter the result objects.
	   return $this->applyFilter($objects, $params['start'],
		  $params['limit'], $params['sortfield'], $params['sortdir']);
*/
    }

	/**
	 * Get a ban rule configuration object.
	 * @param params An array containing the following fields:
	 *   \em uuid The UUID of the configuration object.
	 * @param context The context of the caller.
	 * @return The requested configuration object.
	 * @throw E_CONFIG_GET_OBJECT_FAILED
	 */
	public function getJail($params, $context)
    {
//	    global $xmlConfig;

	    // Validate the RPC caller context.
	    $this->validateMethodContext($context, [
	        "role" => OMV_ROLE_ADMINISTRATOR
	    ]);

		// Validate the parameters of the RPC service method.
		$this->validateMethodParams($params, "rpc.common.objectuuid");
		
		// Get the configuration object.
		$db = \OMV\Config\Database::getInstance();
		return $db->getAssoc("conf.service.fail2ban.jail", $params['uuid']);
/*		
	    // Validate the parameters of the RPC service method.
	    $this->validateMethodParams($params, '{
	         "type":"object",
		 "properties":{
		    "uuid":{'.$GLOBALS['OMV_JSONSCHEMA_UUID'].'}
		 }
	     }');

	     // Get the configuration object.
	     $xpath = sprintf("//services/fail2ban/jails/jail[uuid='%s']",
		  $params['uuid']);
	     $object = $xmlConfig->get($xpath);

	     if(is_null($object))
             {
	         throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
			  $xpath);
	     }

	     // Modify the result object.
             $object['enable']   = boolvalEx($object['enable']); 
             $object['maxretry'] = intval($object['maxretry']);
             $object['bantime']  = intval($object['bantime']);

	     return $object;
*/
	}
	
	/**
	 * Set a ban rule configuration object.
	 * @param params The method parameters.
	 * @param context The context of the caller.
	 * @return The configuration object.
	 * @throw E_CONFIG_SET_OBJECT_FAILED
	 */
	public function setJail($params, $context)
    {
//	     global $xmlConfig;
		 
	     // Validate the RPC caller context.
	     $this->validateMethodContext($context, [
			  "role" => OMV_ROLE_ADMINISTRATOR
	     ]);
		 
		// Validate the parameters of the RPC service method.
		$this->validateMethodParams($params, "rpc.fail2ban.setjail");
		
		// Prepare the configuration object.
		$object = new \OMV\Config\ConfigObject("conf.service.fail2ban.jail");
		$object->setAssoc($params);
		
		// Set the configuration object.
		$db = \OMV\Config\Database::getInstance();
		
		if (TRUE === $object->isNew()) {
			// Check uniqueness:
			// - uuid
			$db->assertIsUnique($object, "uuid");
		}
		
		$db->set($object);
		
		// Return the configuration object.
		return $object->getAssoc();
		
/*		 
	     // Validate the parameters of the RPC service method.
	     $this->validateMethodParams($params, '{
			  "type":"object",
			  "properties":{
				"uuid"     :{'.$GLOBALS['OMV_JSONSCHEMA_UUID_UNDEFINED'].'},
				"enable"   :{"type":"boolean"},
				"name"     :{"type":"string"},
				"port"     :{"type":"string"},
				"maxretry" :{"type":"string"},
				"bantime"  :{"type":"string"},
				"filter"   :{"type":"string"},
				"logpath"  :{"type":"string"}
			  }
	     }');

	     // Prepare the configuration object.
	     $object = array(
			"uuid" => ($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) ?
			  OMVUtil::uuid() : $params['uuid'],
			"enable"   => array_boolval($params,'enable'),
			"name"     => $params['name'],
                        "port"     => $params['port'],
			"maxretry" => $params['maxretry'],
			"bantime"  => $params['bantime'],
                        "filter"   => $params['filter'],
                        "logpath"  => $params['logpath'] 
	     );

	     // Set the configuration object.
	     $success = FALSE;
	     if($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED'])
             {
	          // Append object to configuration.
		  $success = $xmlConfig->set("//services/fail2ban/jails",
	              array("jail" => $object));
	     }
             else
             {
	          // Update existing configuration object.
		  $success = $xmlConfig->replace("//services/fail2ban/jails" .
		      "/jail[uuid='{$params['uuid']}']", $object);
	     }

	     if(FALSE === $success)
             {
	         throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED);
	     }

	     // Notify configuration changes.
	     $dispatcher = &OMVNotifyDispatcher::getInstance();
	     $dispatcher->notify(($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) ?
	         OMV_NOTIFY_CREATE : OMV_NOTIFY_MODIFY,
		 "org.openmediavault.services.fail2ban.jails.jail", $object);
	     
             // Return the configuration object.
	     return $object;
*/
	}

	/**
	 * Delete a jail config object.
	 * @param params An array containing the following fields:
	 *   \em uuid The UUID of the configuration object.
	 * @param context The context of the caller.
	 * @return The deleted configuration object.
	 */
	public function deleteJail($params, $context)
        {
	    // Validate the RPC caller context.
	    $this->validateMethodContext($context, [
	         "role" => OMV_ROLE_ADMINISTRATOR
	    ]);

		// Validate the parameters of the RPC service method.
		$this->validateMethodParams($params, "rpc.common.objectuuid");
		
		// Delete the configuration object.
		$db = \OMV\Config\Database::getInstance();
		$object = $db->get("conf.service.fail2ban.jail", $params['uuid']);
		$db->delete($object);
		
		// Return the deleted configuration object.
		return $object->getAssoc();

/*		
	    // Validate the parameters of the RPC service method.
	    $this->validateMethodParams($params, '{
		"type":"object",
		"properties":{
		"uuid":{'.$GLOBALS['OMV_JSONSCHEMA_UUID'].'}
		}
	     }');

	     // Delete the configuration object.
	     $xpath = sprintf("//services/fail2ban/jails/jail[uuid='%s']",
		  $params['uuid']);

	     return $this->deleteConfigObjectByPath($xpath,
		  "org.openmediavault.services.fail2ban/jails/jail");
*/
	}

	/**
	 * Get statistics, e.g. who's online.
	 * @param params The method parameters.
	 * @param context The context of the caller.
	 * @return A string containing the requested statistics.
	 * @throw E_CONFIG_GET_OBJECT_FAILED
	 * @throw E_EXEC_FAILED
	 */
	public function getStats($params, $context)
        {
//	    global $xmlConfig;

	    // Validate the RPC caller context.
	    $this->validateMethodContext($context, [
	        "role" => OMV_ROLE_ADMINISTRATOR
	    ]);

	    	// Is service enabled?
		$db = \OMV\Config\Database::getInstance();
		$object = $db->get("conf.service.fail2ban");
		
		if (FALSE === $object->get("enable"))
		{
			$stats = gettext("Service disabled");
		}
		else
		{
			//$cmdArgs = [];
			$cmdArgs = "status";
			$cmd = new \OMV\System\Process("fail2ban-client", $cmdArgs);
			$cmd->setRedirect2to1();
			//$cmd->execute($output);
			
			$result = $cmd->execute($output,$exitStatus);

			if(0 !== $exitStatus)
			{
				throw new \OMV\ExecException($cmd->getCommandLine()."->".implode("\n", $output)." and 
result=".$result." and exitStatus=".$exitStatus);			}
			
			$stats .= implode("\n", $output);

            		// Delete "`- Jail list:"
            		$jailsTmp = str_replace("`- Jail list:", "", $output[2]);

          		// Delete spaces
            		$jailsTmp = str_replace(" ", "", $jailsTmp);

			if(strlen($jailsTmp) == 0)
			{
                     		$stats .= implode("\n", "No jail enabled");
            		}
            		else
            		{
                		// Create array
                		$jails = explode(",", $jailsTmp);   
                        
                		foreach ($jails as $jail)
				{                        
                    			$stats .= sprintf("\n\n");

		    			$cmdArgs = [];
		    			$cmdArgs[] = "status";
		    			$cmd = new \OMV\System\Process("fail2ban-client", $cmdArgs);
                    			$cmd->setRedirect2to1();

                    			if(0 !== $cmd->execute($output))
                    			{                
						throw new \OMV\ExecException($cmd." ".implode("\n", $output));
                    			}
                    
                    			$stats .= implode("\n", $output);
                		}
            		}
 	
			return $stats;
		}
/*		
	    // Is service enabled?
	    $xpath = "//services/fail2ban";
	    $object = $xmlConfig->get($xpath);

	    if(is_null($object))
            {
	        throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
		    $xpath);
	    }

	    if (!boolvalEx($object['enable']))
            {
	        $stats = gettext("Service disabled");
	    }
            else
            {
		$cmd = "fail2ban-client status 2>&1";

		if(0 !== $this->exec($cmd, $output))
                {
		     throw new OMVException(OMVErrorMsg::E_EXEC_FAILED,
		         $cmd, implode("\n", $output));
		}   
                      
                $stats .= implode("\n", $output);


                // Delete "`- Jail list:"
                $jailsTmp = str_replace("`- Jail list:", "", $output[2]);

                // Delete spaces
                $jailsTmp = str_replace(" ", "", $jailsTmp);

		if(strlen($jailsTmp) == 0)
		{
                     $stats .= implode("\n", "No jail enabled");
                }
                else
                {
                     // Create array
                     $jails = explode(",", $jailsTmp);   
                        
                     foreach ($jails as $jail)
	             {                        
                         $stats .= sprintf("\n\n");

                          $cmd = "fail2ban-client status $jail 2>&1";

                          if(0 !== $this->exec($cmd, $output))
                          {                
                              throw new OMVException(OMVErrorMsg::E_EXEC_FAILED,
                                   $cmd, implode("\n", $output));
                          }
                    
                           $stats .= implode("\n", $output);
                     }
                }
            }

            return $stats;
*/
      }
}
