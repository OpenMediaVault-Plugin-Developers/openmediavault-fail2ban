<?php
/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @copyright Copyright (c) 2009-2016 Volker Theile
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
use OMV\System\LogFileSpec;

// /var/log/fail2ban.log
// 2016-06-19 07:35:17,537 fail2ban.filter : INFO   Log rotation detected for /var/log/auth.log
LogFileSpec::registerSpecification('fail2ban', [
    'filename' => 'fail2ban.log',
    'filepath' => '/var/log/fail2ban.log',
    'regex' => '/^(\S+\s+\S+)(,\S+) (.*)$/',
    'columns' => [
        'ts' => [
            'index' => 1,
            'func' => function($v){return strpdate($v,'Y-m-d G:i:s');}
        ],
        'date' => [
            'index' => 1,
            'func' => function($v){return strftime('%c', strpdate($v,'Y-m-d G:i:s'));}
        ],
        'message' => 3
    ]
]);