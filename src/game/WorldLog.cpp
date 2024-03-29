/* 
 * Copyright (C) 2005,2006,2007 MaNGOS <http://www.mangosproject.org/>
 * Copyright (C) 2007 NoxForum.net <http://www.noxforum.net/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/** \file
    \ingroup u2w
*/

#include "WorldLog.h"
#include "Policies/SingletonImp.h"
#include "Config/ConfigEnv.h"

#define CLASS_LOCK MaNGOS::ClassLevelLockable<WorldLog, ZThread::FastMutex>
INSTANTIATE_SINGLETON_2(WorldLog, CLASS_LOCK);
INSTANTIATE_CLASS_MUTEX(WorldLog, ZThread::FastMutex);

#define WORLD_LOG_FILE_STRING   "world.log"

/// Open the log file (if specified so in the configuration file)
void WorldLog::Initialize()
{
    if( sConfig.GetBoolDefault("LogWorld", false) )
    {
        i_file = fopen(WORLD_LOG_FILE_STRING, "w");
    }
}

#define sWorldLog WorldLog::Instance()
