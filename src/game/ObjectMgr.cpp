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

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "Database/SQLStorage.h"

#include "Log.h"
#include "WorldLog.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "NoxMap.h"
#include "NoxThinglib.h"
#include "UpdateMask.h"
#include "World.h"
#include "WorldSession.h"
#include "ProgressBar.h"
#include "Policies/SingletonImp.h"

/// Collisions
#include "flatland/flatland.hpp"

INSTANTIATE_SINGLETON_1(ObjectMgr);

ObjectMgr::ObjectMgr()
{
	memset(objectExtents, 0, MAX_EXTENT);
	World.SetContactMaxCorrectingVel(0.0f);

	// Build Collide Table
	collideTable[ COLLIDE_DAMAGE ]	= CollideHandler( &CollisionResponse::DamageCollideCallback );

	// Build Update Table
	updateTable[ UPDATE_ONESECONDDIE ] = UpdateHandler( &Object::OneSecondDieUpdate );

	// Build Enchant Table, frame value came from gamedata.bin
	enchantTable[ ENCHANT_INVISIBLE ] = EnchantEntry("ENCHANT_INVISIBLE", 1800);
	enchantTable[ ENCHANT_MOONGLOW ] = EnchantEntry("ENCHANT_MOONGLOW", 9000);
	enchantTable[ ENCHANT_BLINDED ] = EnchantEntry("ENCHANT_BLINDED");
	enchantTable[ ENCHANT_CONFUSED ] = EnchantEntry("ENCHANT_CONFUSED", 90);
	enchantTable[ ENCHANT_SLOWED ] = EnchantEntry("ENCHANT_SLOWED", 120);
	enchantTable[ ENCHANT_HELD ] = EnchantEntry("ENCHANT_HELD", 60);
	enchantTable[ ENCHANT_DETECTING ] = EnchantEntry("ENCHANT_DETECTING");
	enchantTable[ ENCHANT_ETHEREAL ] = EnchantEntry("ENCHANT_ETHEREAL");
	enchantTable[ ENCHANT_RUN ] = EnchantEntry("ENCHANT_RUN");
	enchantTable[ ENCHANT_HASTED ] = EnchantEntry("ENCHANT_HASTED", 600);
	enchantTable[ ENCHANT_VILLAIN ] = EnchantEntry("ENCHANT_VILLAIN");
	enchantTable[ ENCHANT_AFRAID ] = EnchantEntry("ENCHANT_AFRAID");
	enchantTable[ ENCHANT_BURNING ] = EnchantEntry("ENCHANT_BURNING");
	enchantTable[ ENCHANT_VAMPIRISM ] = EnchantEntry("ENCHANT_VAMPIRISM");
	enchantTable[ ENCHANT_ANCHORED ] = EnchantEntry("ENCHANT_ANCHORED", 300);
	enchantTable[ ENCHANT_LIGHT ] = EnchantEntry("ENCHANT_LIGHT", 9000);
	enchantTable[ ENCHANT_DEATH ] = EnchantEntry("ENCHANT_DEATH");
	enchantTable[ ENCHANT_PROTECT_FROM_FIRE ] = EnchantEntry("ENCHANT_PROTECT_FROM_FIRE", 1800);
	enchantTable[ ENCHANT_PROTECT_FROM_POISON ] = EnchantEntry("ENCHANT_PROTECT_FROM_POISON", 1800);
	enchantTable[ ENCHANT_PROTECT_FROM_MAGIC ] = EnchantEntry("ENCHANT_PROTECT_FROM_MAGIC", 1800);
	enchantTable[ ENCHANT_PROTECT_FROM_ELECTRICITY ] = EnchantEntry("ENCHANT_PROTECT_FROM_ELECTRICITY", 1800);
	enchantTable[ ENCHANT_INFRAVISION ] = EnchantEntry("ENCHANT_INFRAVISION", 900);
	enchantTable[ ENCHANT_SHOCK ] = EnchantEntry("ENCHANT_SHOCK", 600);
	enchantTable[ ENCHANT_INVULNERABLE ] = EnchantEntry("ENCHANT_INVULNERABLE", 150);
	enchantTable[ ENCHANT_TELEKINESIS ] = EnchantEntry("ENCHANT_TELEKINESIS");
	enchantTable[ ENCHANT_FREEZE ] = EnchantEntry("ENCHANT_FREEZE");
	enchantTable[ ENCHANT_SHIELD ] = EnchantEntry("ENCHANT_SHIELD");
	enchantTable[ ENCHANT_REFLECTIVE_SHIELD ] = EnchantEntry("ENCHANT_REFLECTIVE_SHIELD");
	enchantTable[ ENCHANT_CHARMING ] = EnchantEntry("ENCHANT_CHARMING");
	enchantTable[ ENCHANT_ANTI_MAGIC ] = EnchantEntry("ENCHANT_ANTI_MAGIC");
	enchantTable[ ENCHANT_CROWN ] = EnchantEntry("ENCHANT_CROWN");
	enchantTable[ ENCHANT_SNEAK ] = EnchantEntry("ENCHANT_SNEAK");

	// Build Use Table
	useTable[ USE_POTION ] = UseHandler( &WorldObject::PotionUse );
	useTable[ USE_CONSUME ] = UseHandler( &WorldObject::ConsumeUse );
	useTable[ USE_CONSUMECONFUSE ] = UseHandler( &WorldObject::ConsumeConfuseUse );
	useTable[ USE_MUSHROOM ] = UseHandler( &WorldObject::MushroomUse );

	// Build Xfer Table
	xferTable[ XFER_DEFAULT ] = XferHandler( &ObjectMgr::DefaultXferHandler );
	xferTable[ XFER_SPELLPAGEPEDESTAL ] = XferHandler( &ObjectMgr::DefaultXferHandler );
	xferTable[ XFER_SPELLREWARD ] = XferHandler( &ObjectMgr::SpellRewardXferHandler );
	xferTable[ XFER_ABILITYREWARD ] = XferHandler( &ObjectMgr::AbilityRewardXferHandler );
	xferTable[ XFER_FIELDGUIDE ] = XferHandler( &ObjectMgr::FieldGuideXferHandler );
	xferTable[ XFER_READABLE ] = XferHandler( &ObjectMgr::ReadableXferHandler );
	xferTable[ XFER_EXIT ] = XferHandler( &ObjectMgr::ExitXferHandler );
	xferTable[ XFER_DOOR ] = XferHandler( &ObjectMgr::DoorXferHandler );
	xferTable[ XFER_TRIGGER ] = XferHandler( &ObjectMgr::TriggerXferHandler );
	xferTable[ XFER_MONSTER ] = XferHandler( &ObjectMgr::DefaultXferHandler );
	xferTable[ XFER_HOLE ] = XferHandler( &ObjectMgr::DefaultXferHandler );
	xferTable[ XFER_TRANSPORTER ] = XferHandler( &ObjectMgr::DefaultXferHandler );
	xferTable[ XFER_ELEVATOR ] = XferHandler( &ObjectMgr::DefaultXferHandler );
	xferTable[ XFER_ELEVATORSHAFT ] = XferHandler( &ObjectMgr::DefaultXferHandler );
	xferTable[ XFER_MOVER ] = XferHandler( &ObjectMgr::DefaultXferHandler );
	xferTable[ XFER_GLYPH ] = XferHandler( &ObjectMgr::DefaultXferHandler );
	xferTable[ XFER_INVISIBLELIGHT ] = XferHandler( &ObjectMgr::DefaultXferHandler );
	xferTable[ XFER_SENTRY ] = XferHandler( &ObjectMgr::DefaultXferHandler );
	xferTable[ XFER_WEAPON ] = XferHandler( &ObjectMgr::DefaultXferHandler );
	xferTable[ XFER_ARMOR ] = XferHandler( &ObjectMgr::DefaultXferHandler );
	xferTable[ XFER_TEAM ] = XferHandler( &ObjectMgr::DefaultXferHandler );
	xferTable[ XFER_GOLD ] = XferHandler( &ObjectMgr::GoldXferHandler );
	xferTable[ XFER_AMMO ] = XferHandler( &ObjectMgr::AmmoXferHandler );
	xferTable[ XFER_NPC ] = XferHandler( &ObjectMgr::DefaultXferHandler );
	xferTable[ XFER_OBELISK ] = XferHandler( &ObjectMgr::ObeliskXferHandler );
	xferTable[ XFER_TOXICCLOUD ] = XferHandler( &ObjectMgr::ToxicCloudXferHandler );
	xferTable[ XFER_MONSTERGENERATOR ] = XferHandler( &ObjectMgr::DefaultXferHandler );
	xferTable[ XFER_REWARDMARKER ] = XferHandler( &ObjectMgr::RewardMarkerXferHandler );
}

ObjectMgr::~ObjectMgr()
{
}

Object* ObjectMgr::CreateObjectFromFile(NoxBuffer* rdr, NoxObjectTOC* toc)
{
	uint8 inventory = 0;

	uint16 type = rdr->read<uint16>(), type2 = 0;
	if(toc->find(type) == toc->end())
		return NULL;
	rdr->skip();
	
	type = sThingBin.Thing.Object.GetIndex((*toc)[type].c_str(), (*toc)[type].size());
	ASSERT(type != 0);

	Object* obj = NULL;
	if(sThingBin.Thing.Object.Objects.Get(type)->classes & CLASS_IMMOBILE)
		obj = new Object(type, GridPair());
	else
		obj = new WorldObject(type, GridPair());

	size_t finish = rdr->read<int64>() + rdr->rpos();

	/*XferTableMap::iterator iter = xferTable.find(obj->GetObjectInfo()->xfer);
	if(iter != xferTable.end())
		inventory = (this->*iter->second.handler)(obj, rdr);*/
	inventory = DefaultXferHandler(obj, rdr);
	rdr->rpos(finish);

	ASSERT(rdr->rpos() == finish);
	
	for(;inventory > 0; inventory--)
	{
		WorldObject* child = (WorldObject*)CreateObjectFromFile(rdr, toc);
		if(child != NULL)
			obj->m_inventory.insert(child);
		else
			break;
	}

	obj->AddToWorld();
	return obj;
}

std::vector<WorldObject*> ObjectMgr::GetObjectsInRect(GridPair leftTop, GridPair rightBottom)
{
	std::vector<WorldObject*> v;
	for(ObjectTableMap::iterator iter = objectTable.begin(); iter != objectTable.end(); ++iter)
	{
		if(!(*iter)->IsImmobile() && (*iter)->GetPosition().in(leftTop, rightBottom))
			v.push_back((WorldObject*)*iter);
	}
	return v;
}

void ObjectMgr::AddObject(Object *obj)
{
	ASSERT(obj);
	if(GetObj(obj->GetExtent()) == obj)
		return;
	if(!obj->GetExtent() || !IsExtentAvailable(obj->GetExtent()))
		obj->m_extent = RequestExtent();
	objectExtents[obj->GetExtent()] = obj;
	objectTable.insert(obj);

	if(obj->GetType() == 0x2C9)
		playerList.insert(obj);
}
bool ObjectMgr::RemoveObject(Object *obj)
{
	ObjectTableMap::iterator i = objectTable.find(obj);
	if(i == objectTable.end())
		return false;
	objectTable.erase(i);
	if(obj->GetType() == 0x2C9)
		playerList.erase(obj);
	objectExtents[obj->GetExtent()] = 0;
	return true;
}

void ObjectMgr::Update(float diff)
{
	if(!objectTable.size())
		return;

	//Having our own list of dynamic players cuts time by ALOT
	World.GenerateContacts(objectTable, playerList);
	World.QuickStep(diff);
	//ASSERT(!World.IsCorrupt(objectTable));
}

NoxWallObject::NoxWallObject() : Object()
{
	shape = new Flatland::Composite(Flatland::vec2(0.0f,0.0f));
	body = new Flatland::Static<Flatland::Composite>(shape);

	body->Property().bounceVelocity = 0.0f;
	body->Property().collisionMask = 0x00000001;
	body->SetUserPointer(0);

}
NoxWallObject::~NoxWallObject()
{
	delete body;
	delete shape;
}
void NoxWallObject::AddWall(CoordPair<255> pos, uint8 facing)
{
	switch(facing)
	{
		case WALL_NORTH:
			shape->push_back(CreateWall(pos, WALL_NORTH_FLAG | WALL_SOUTH_FLAG));
		break;
		case WALL_WEST:
			shape->push_back(CreateWall(pos, WALL_EAST_FLAG | WALL_WEST_FLAG));
		break;
		case WALL_SW_CORNER:
			shape->push_back(CreateWall(pos, WALL_WEST_FLAG | WALL_NORTH_FLAG));
		break;
		case WALL_NW_CORNER:
			shape->push_back(CreateWall(pos, WALL_NORTH_FLAG | WALL_EAST_FLAG));
		break;
		case WALL_NE_CORNER:
			shape->push_back(CreateWall(pos, WALL_SOUTH_FLAG | WALL_EAST_FLAG));
		break;
		case WALL_SE_CORNER:
			shape->push_back(CreateWall(pos, WALL_WEST_FLAG | WALL_SOUTH_FLAG));
		break;
		case WALL_SOUTH_T:
			shape->push_back(CreateWall(pos, WALL_WEST_FLAG | WALL_NORTH_FLAG | WALL_SOUTH_FLAG));
			break;
		case WALL_EAST_T:
			shape->push_back(CreateWall(pos, WALL_NORTH_FLAG | WALL_EAST_FLAG | WALL_WEST_FLAG));
			break;
		case WALL_NORTH_T:
			shape->push_back(CreateWall(pos, WALL_SOUTH_FLAG | WALL_EAST_FLAG | WALL_NORTH_FLAG));
			break;
		case WALL_WEST_T:
			shape->push_back(CreateWall(pos, WALL_WEST_FLAG | WALL_EAST_FLAG | WALL_SOUTH_FLAG));
			break;
		default:
			shape->push_back(CreateWall(pos, WALL_NORTH_FLAG | WALL_SOUTH_FLAG));
			break;
	}
}
Flatland::Composite* NoxWallObject::CreateWall(CoordPair<255> pos, uint8 flags)
{
	const float wallSize = 23.0;
	const float halfSize = 11.5;
	Flatland::vec2 center(pos.x_coord * wallSize + halfSize, pos.y_coord * wallSize + halfSize);
	Flatland::Composite* comp = new Flatland::Composite(center);
	NoxWallLine* line;
	if(flags & WALL_NORTH_FLAG)
	{
		line = new NoxWallLine(center, Flatland::vec2(center.x + halfSize, center.y - halfSize));
		comp->push_back(line);
	}
	if(flags & WALL_SOUTH_FLAG)
	{
		line = new NoxWallLine(center, Flatland::vec2(center.x - halfSize, center.y + halfSize));
		comp->push_back(line);
	}
	if(flags & WALL_EAST_FLAG)
	{
		line = new NoxWallLine(center, Flatland::vec2(center.x + halfSize, center.y + halfSize));
		comp->push_back(line);
	}
	if(flags & WALL_WEST_FLAG)
	{
		line = new NoxWallLine(center, Flatland::vec2(center.x - halfSize, center.y - halfSize));
		comp->push_back(line);
	}

	return comp;
}

NoxWallLine::NoxWallLine(Flatland::vec2 a, Flatland::vec2 b) : Flatland::Line(a, b)
{
	extent[1] = 5;
}

int ObjectMgr::DefaultXferHandler(Object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	uint8 inventory = 0;

	obj->m_props1 = rdr->read<uint16>(); // properties
	obj->m_props2 = rdr->read<uint16>(); // type2
	obj->m_extent = (uint16)rdr->read<uint32>(); // extent
	rdr->read<uint32>();

	float x = rdr->read<float>();
	float y = rdr->read<float>();
	obj->body->SetCenter(Flatland::vec2(x, y));

	if(rdr->read<uint8>() == 0xFF)
	{
		obj->m_flags = rdr->read<uint32>();
		rdr->readstring<uint8>(buffer, 256); //script name
		rdr->read<uint8>();
		inventory = rdr->read<uint8>();
		for (int i = rdr->read<uint16>(); i > 0; i--)
			rdr->read<uint32>();
		rdr->read<uint16>();
		rdr->read<uint32>();
		if (obj->m_props2 == 0x40)
		{
			rdr->readstring<uint32>(buffer, 256);
		}
		rdr->read<uint64>();
	}

	return inventory;
}

int ObjectMgr::GoldXferHandler(Object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);
	obj->m_worth = rdr->read<uint32>();

	return inventory;
}

int ObjectMgr::ObeliskXferHandler(Object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);
	if(obj->m_props1 < 0x3D)
		return inventory;

	rdr->read<uint32>();
	rdr->read<uint8>();

	return inventory;
}
int ObjectMgr::ToxicCloudXferHandler(Object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->read<uint32>();

	return inventory;
}

int ObjectMgr::RewardMarkerXferHandler(Object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->read<uint32>();
	rdr->read<uint32>();
	int strings = rdr->read<uint16>();
	for(; strings; strings--)
		rdr->readstring<uint8>(buffer, 256);
	strings = rdr->read<uint16>();
	for(; strings; strings--)
		rdr->readstring<uint8>(buffer, 256);
	strings = rdr->read<uint16>();
	for(; strings; strings--)
		rdr->readstring<uint8>(buffer, 256);

	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();

	if(obj->m_props1 > 0x3E)
		rdr->read<uint32>();
	if(obj->m_props2 > 0x3F)
		rdr->read<uint8>();

	return inventory;
}

int ObjectMgr::AmmoXferHandler(Object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	int strings = 4;
	for(; strings; strings--)
		rdr->readstring<uint8>(buffer, 256);

	rdr->read<uint8>();
	rdr->read<uint8>();

	return inventory;
}
int ObjectMgr::SpellPagePedestalXferHandler(Object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->read<uint32>();

	return inventory;
}
int ObjectMgr::SpellRewardXferHandler(Object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->readstring<uint8>(buffer, 256);

	return inventory;
}

int ObjectMgr::AbilityRewardXferHandler(Object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->readstring<uint8>(buffer, 256);

	return inventory;
}
int ObjectMgr::FieldGuideXferHandler(Object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->readstring<uint8>(buffer, 256); // i think this is correct

	return inventory;
}
int ObjectMgr::ReadableXferHandler(Object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->readstring<uint32>(buffer, 256); // might want to make this dynamic to allow for bigger text

	return inventory;
}
int ObjectMgr::ExitXferHandler(Object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->readstring<uint32>(buffer, 256); // might want to make this dynamic to allow for bigger text
	rdr->read<uint32>();
	rdr->read<uint32>();

	return inventory;
}
int ObjectMgr::DoorXferHandler(Object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->read<uint32>(); // direction, see for map editor code for details
	rdr->read<uint32>(); // lock
	rdr->read<uint32>(); // if obj->props1 > 0x29, i'll be shocked if we find one that isn't

	return inventory;
}
int ObjectMgr::TriggerXferHandler(Object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->read<uint32>(); // x
	rdr->read<uint32>(); // y
	if(obj->m_props1 > 0x29)
	{
		rdr->read<uint8>();
		rdr->read<uint8>();
		rdr->read<uint8>();
		rdr->read<uint8>();
		rdr->read<uint8>();
		rdr->read<uint8>();
	}
	else
	{
		/// exe has this has three sets of three, but they don't seem to be important
		rdr->read<uint32>();
		rdr->read<uint32>();
		rdr->read<uint8>();
	}
	rdr->read<uint32>();
	// these calls need to be moved to a separate function
	rdr->read<uint16>(); // should be <= 1
	rdr->readstring<uint32>(buffer, 256); // on enter
	rdr->read<uint32>();

	rdr->read<uint16>(); // should be <= 1
	rdr->readstring<uint32>(buffer, 256);
	rdr->read<uint32>();

	rdr->read<uint16>(); // should be <= 1
	rdr->readstring<uint32>(buffer, 256);
	rdr->read<uint32>();

	rdr->read<uint32>();

	// these four only if props1 > 0x1F
	rdr->read<uint8>();
	rdr->read<uint8>();
	rdr->read<uint8>();
	rdr->read<uint8>();

	rdr->read<uint32>();
	rdr->read<uint32>();

	return inventory;
}
int ObjectMgr::HoleXferHandler(Object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	// if props1 > 0x29
	rdr->read<uint32>();
	
	// same as the calls in trigger xfer
	rdr->read<uint16>(); // should be <= 1
	rdr->readstring<uint32>(buffer, 256);
	rdr->read<uint32>();

	rdr->read<uint32>(); //x
	rdr->read<uint32>(); //y
	// if props1 > 0x28
	rdr->read<uint32>(); //default 0
	rdr->read<uint16>(); //default 0
	
	return inventory;
}
int ObjectMgr::TransporterXferHandler(Object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->read<uint32>();

	return inventory;
}
int ObjectMgr::ElevatorXferHandler(Object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->read<uint32>();
	rdr->read<uint32>(); // if props1 > 0x28
	rdr->read<uint8>(); // if props1 > 0x3C

	return inventory;
}
int ObjectMgr::ElevatorShaftXferHandler(Object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->read<uint32>();

	return inventory;
}
