<?php

/**
 * Copyright (C) 2014-2018 OpenMediaVault Plugin Developers
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
    private function getName()
    {
        return 'fail2ban';
    }

    public function getStatus()
    {
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get('conf.service.fail2ban');
        $systemCtl = new \OMV\System\SystemCtl('fail2ban');
        return [
            'name' => $this->getName(),
            'title' => gettext("fail2ban"),
            'enabled' => $object->get('enable'),
            'running' => $systemCtl->isActive()
        ];
    }

    public function applyConfig()
    {
        $cmd = new \OMV\System\Process('omv-mkconf', 'fail2ban');
        $cmd->setRedirect2to1();
        $cmd->execute();
    }

    public function startService()
    {
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get('conf.service.fail2ban');
        if (TRUE !== $object->get('enable'))
            return;
        // Start this service and enable the unit file.
        $systemCtl = new \OMV\System\SystemCtl('fail2ban');
        $systemCtl->enable(TRUE);
    }

    public function stopService()
    {
        $systemCtl = new \OMV\System\SystemCtl('fail2ban');
        $systemCtl->disable(TRUE);
    }

    public function bindListeners(\OMV\Engine\Notify\Dispatcher $dispatcher)
    {
        $dispatcher->addListener(
            OMV_NOTIFY_MODIFY,
            'org.openmediavault.conf.service.fail2ban',
            [ $this, 'setDirty' ]
        );
        $dispatcher->addListener(
            OMV_NOTIFY_CREATE | OMV_NOTIFY_MODIFY | OMV_NOTIFY_DELETE,
            'org.openmediavault.conf.service.fail2ban.jail',
            [ $this, 'setDirty' ]
        );
    }
}
