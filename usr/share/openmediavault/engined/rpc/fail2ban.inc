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
    /**
     * Get the RPC service name.
     */
    public function getName()
    {
        return "Fail2Ban";
    }

    /**
     * Initialize the RPC service.
     */
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

    /**
     * Get the configurations settings.
     * @param params The method parameters.
     * @param context The context of the caller.
     * @return The requested configuration object.
     */
    public function getSettings($params, $context)
    {
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
    }
   
    /**
     * Set the configuration settings.
     * @param params The method parameters.
     * @param context The context of the coller.
     * @return The stored configuration object.
     */
    public function setSettings($params, $context)
    {

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
	 // Validate the RPC caller context.
	 $this->validateMethodContext($context, [
                "role" => OMV_ROLE_ADMINISTRATOR
	 ]);

	// Validate the parameters of the RPC service method.
	$this->validateMethodParams($params, "rpc.common.getlist");

	// Get the configuration object.
	$db = \OMV\Config\Database::getInstance();
	$objects = $db->getAssoc("conf.service.fail2ban.jail");

	// Filter the result.
	return $this->applyFilter($objects,$params['start'],$params['limit'],$params['sortfield'],$params['sortdir']);
	
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
        // Validate the RPC caller context.
        $this->validateMethodContext($context, [
	        "role" => OMV_ROLE_ADMINISTRATOR
	    ]);

	// Validate the parameters of the RPC service method.
	$this->validateMethodParams($params, "rpc.common.objectuuid");
		
	// Get the configuration object.
	$db = \OMV\Config\Database::getInstance();
	
        return $db->getAssoc("conf.service.fail2ban.jail", $params['uuid']);
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
		
	if (TRUE === $object->isNew())
        {
		// Check uniqueness:
		// - uuid
		$db->assertIsUnique($object, "uuid");
	}
		
	$db->set($object);
		
	// Return the configuration object.
	return $object->getAssoc();
	
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
	    // Validate the RPC caller context.
	    $this->validateMethodContext($context, [
	        "role" => OMV_ROLE_ADMINISTRATOR
	    ]);

		// Init Variables
		$stats = "";

	    	// Is service enabled?
		$db = \OMV\Config\Database::getInstance();
		$object = $db->get("conf.service.fail2ban");
		
		if (FALSE === $object->get("enable"))
		{
			$stats = gettext("Service disabled");
		}
		else
		{
			$cmdArgs = "status";
			$cmd = new \OMV\System\Process("fail2ban-client", $cmdArgs);
			$cmd->setRedirect2to1();
			
			$result = $cmd->execute($output,$exitStatus);

			if(0 !== $exitStatus)
			{
				throw new \OMV\ExecException($cmd->getCommandLine()."->".implode("\n", $output)." and 
result=".$result." and exitStatus=".$exitStatus);			
                        }
			
			$stats .= implode("\n", $output);
			$stats .= sprintf("\n\n");

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
				//$stats = "";

                		// Create array
                		$jails = explode(",", $jailsTmp);   
                        
                		foreach ($jails as $jail)
				{                        
                    			//$stats .= sprintf("\n\n");
					$output2 = [];
		    			$cmd2Args = [];
		    			$cmd2Args[] = "status ".$jail;
		    			$cmd2 = new \OMV\System\Process("fail2ban-client", $cmd2Args);
                    			$cmd2->setRedirect2to1();

                                        $result2 = $cmd2->execute($output2,$exitStatus2); 
                    			if(0 !== $exitStatus2)
                    			{                
						throw new \OMV\ExecException($cmd->getCommandLine()." ".implode("\n", $output)." and result=".$result2." exitStatus=".$exitStatus2);
                    			}
                    
                    			$stats .= implode("\n", $output2);
					$stats .= sprintf("\n\n");
                		}
            		}
 	
		}
		return $stats;
      }
}
