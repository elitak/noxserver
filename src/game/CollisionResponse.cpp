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

#include "CollisionResponse.h"
#include "Object.h"
#include "Player.h"

#include "flatland/flatland.hpp"

void CollisionResponse::DamageCollideCallback(Flatland::ContactList &contacts)
{
	Object* me, *other;
	if( !(me = (Object*)contacts.Self()->GetUserPointer()) || !( other = (Object*)contacts.Other()->GetUserPointer()))
		return;
	if( other->GetType() != 0x2C9)
		return;
	Player* player = (Player*)other;

	float damage = atoi(me->GetObjectInfo()->collide_args[0]);
	// damagetype = me->GetObjectInfo()->collide_args[1] // convert from string to enum?

	player->Damage(damage/3, NULL);
}
void CollisionResponse::ManaDrainCollideCallback(Flatland::ContactList &contacts)
{
     
	Object* me, *other;
	if( !(me = (Object*)contacts.Self()->GetUserPointer()) || !( other = (Object*)contacts.Other()->GetUserPointer()))
		return;
	if( other->GetType() != 0x2C9)
		return;
	Player* player = (Player*)other;

	float damage = atoi(me->GetObjectInfo()->collide_args[0]);
	// damagetype = me->GetObjectInfo()->collide_args[1] // convert from string to enum?

	if(player->ManaDrain(damage/3))
		me->EmitSound(SOUND_BLUEFIREDAMAGE);
}
void CollisionResponse::SpellProjectileCollideCallback(Flatland::ContactList &contacts)
{
     
	Object* me, *other;
	if( !(me = (Object*)contacts.Self()->GetUserPointer()) || !( other = (Object*)contacts.Other()->GetUserPointer()))
		return;
	if( other->GetType() != 0x2C9)
		return;
	Player* player = (Player*)other;
	Magic* magic = (Magic*)me; // This better be a magic projectile
	magic->Collide(player);
//	magic->Destroy();
}
