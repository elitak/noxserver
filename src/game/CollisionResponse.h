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

#ifndef _COLLISIONRESPONSE_H
#define _COLLISIONRESPONSE_H

#include "Common.h"

#include "flatland/flatland.hpp"

class CollisionResponse
{
public:
	static void DamageCollideCallback(Flatland::ContactList &contacts);
     static void ManaDrainCollideCallback(Flatland::ContactList &contacts);
	 static void SpellProjectileCollideCallback(Flatland::ContactList &contacts);
protected:
};

#endif
