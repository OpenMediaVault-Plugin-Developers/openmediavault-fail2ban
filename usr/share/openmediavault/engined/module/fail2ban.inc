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

class OMVModuleFail2ban extends \OMV\Engine\Module\ServiceAbstract
    implements \OMV\Engine\Notify\IListener, \OMV\Engine\Module\IServiceStatus
{
    private function getPluginName()
    {
        return strtolower($this->getName());
    }

    private function getEventMessagePath()
    {
		// "org.openmediavault.conf.service.fail2ban"
        return sprintf("org.openmediavault.conf.service.%s",$this->getPluginName());
    }

/*
    private function getXPath()
    {
        return sprintf("//services/%s",$this->getPluginName());
    }
*/
	
    /**
     * Get the module name.
     */
    public function getName()
    {
       return "fail2ban";
    }


    /**
     * Get the status of the service
     */
    public function getStatus()
    {
		$db = \OMV\Config\Database::getInstance();
		$object = $db->get("conf.service.fail2ban");
		$systemCtl = new \OMV\System\SystemCtl($this->getName());
		
		return [
			"name" => $this->getName(),
			"title" => gettext($this->getName()),
			"enabled" => $object->get("enable"),
			"running" => $systemCtl->isActive()
		];		
		
/*		
        global $xmlConfig;

        $object = $xmlConfig->get($this->getXPath());

        if (is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPath());

        $initd = new OMVSysVInitScript($this->getName());

        return array(
            "name"    => $this->getName(),
            "title"   => gettext($this->getName()),
            "enabled" => boolvalEx($object["enable"]),
            "running" => $initd->status() == 0
        );
*/
    }

    /**
     * Create and update the configuration
     */
    public function applyConfig()
    {
		$cmd = new \OMV\System\Process("omv-mkconf", $this->getName());
		$cmd->setRedirect2to1();
		$cmd->execute();
/*		
        $cmd = sprintf("sudo omv-mkconf %s 2>&1", $this->getName());

        if (0 !== $this->exec($cmd, $output))
            throw new OMVException(OMVErrorMsg::E_EXEC_FAILED,
                $cmd, implode("\n", $output));
*/
    }

    /**
     * Start the service
     */
    public function startService()
    {
		$db = \OMV\Config\Database::getInstance();
		$object = $db->get("conf.service.fail2ban");
		
		if (TRUE !== $object->get("enable"))
		{
			return;
		}
		
		$systemCtl = new \OMV\System\SystemCtl($this->getName());
		
		if ($systemCtl->isEnabled() && $systemCtl->isActive())
		{
			// running -> reload config
			$systemCtl->reload(); 
		}
		else
		{
			$systemCtl->enable(TRUE);
		}
/*		
        global $xmlConfig;

        $object = $xmlConfig->get($this->getXPath());

        if (is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPath());

        if (!$object["enable"])
            return;

        $initd = new OMVSysVInitScript($this->getName(), true);
        $initd->exec();
*/
    }

    /**
     * Stop the service
     */
    public function stopService()
    {
		$systemCtl = new \OMV\System\SystemCtl($this->getName());
		$systemCtl->disable(TRUE);
/*		
        $initd = new OMVSysVInitScript($this->getName());
        $initd->exec();
*/
    }

    /**
     * Bind listeners.
     */
    public function bindListeners(\OMV\Engine\Notify\Dispatcher $dispatcher)
    {
	$dispatcher->addListener(
            OMV_NOTIFY_MODIFY,
			$this->getEventMessagePath(),
			[ $this, "setDirty" ]);
/*			
             array($this, "setDirty"));
*/
			
        $dispatcher->addListener(
	    OMV_NOTIFY_CREATE | OMV_NOTIFY_MODIFY | OMV_NOTIFY_DELETE,
	    "org.openmediavault.services.fail2ban.jails.jail",
		[ $this, "setDirty" ]);
		
//	    array($this, "setDirty"));
    }
}

?>
