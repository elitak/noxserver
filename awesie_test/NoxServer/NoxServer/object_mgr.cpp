#include "world.h"
#include "object_mgr.h"

object_mgr::object_mgr(boost::restricted)
{
	memset(m_extents, 0, MAX_EXTENT);

	// Build Collide Table
	//collideTable[ COLLIDE_DAMAGE ]	= CollideHandler( &CollisionResponse::DamageCollideCallback );
    //collideTable[ COLLIDE_MANADRAIN ]  = CollideHandler( &CollisionResponse::ManaDrainCollideCallback );
	//collideTable[ COLLIDE_SPELLPROJECTILE ] = CollideHandler( &CollisionResponse::SpellProjectileCollideCallback );
    //collideTable[ COLLIDE_PLAYER ]     = CollideHandler( &Player::PlayerCollideCallback );

	// Build Update Table
	//updateTable[ UPDATE_ONESECONDDIE ] = UpdateHandler( &Object::OneSecondDieUpdate );
	//updateTable[ UPDATE_SPELLPROJECTILE ] = UpdateHandler( &Object::SpellProjectileUpdate );

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
	//useTable[ USE_POTION ] = UseHandler( &WorldObject::PotionUse );
	//useTable[ USE_CONSUME ] = UseHandler( &WorldObject::ConsumeUse );
	//useTable[ USE_CONSUMECONFUSE ] = UseHandler( &WorldObject::ConsumeConfuseUse );
	//useTable[ USE_MUSHROOM ] = UseHandler( &WorldObject::MushroomUse );

     //Build Pickup Table
    //pickupTable[ PICKUP_FOOD ] = PickupHandler( &WorldObject::FoodPickup );
    //pickupTable[ PICKUP_ARMOR ] = PickupHandler( &WorldObject::ArmorPickup );
    //pickupTable[ PICKUP_WEAPON ] = PickupHandler( &WorldObject::WeaponPickup );

	// Build Xfer Table
	xferTable[ XFER_DEFAULT ] = XferHandler( &object_mgr::DefaultXferHandler );
	xferTable[ XFER_SPELLPAGEPEDESTAL ] = XferHandler( &object_mgr::SpellPagePedestalXferHandler );
	xferTable[ XFER_SPELLREWARD ] = XferHandler( &object_mgr::SpellRewardXferHandler );
	xferTable[ XFER_ABILITYREWARD ] = XferHandler( &object_mgr::AbilityRewardXferHandler );
	xferTable[ XFER_FIELDGUIDE ] = XferHandler( &object_mgr::FieldGuideXferHandler );
	xferTable[ XFER_READABLE ] = XferHandler( &object_mgr::ReadableXferHandler );
	xferTable[ XFER_EXIT ] = XferHandler( &object_mgr::ExitXferHandler );
	xferTable[ XFER_DOOR ] = XferHandler( &object_mgr::DoorXferHandler );
	xferTable[ XFER_TRIGGER ] = XferHandler( &object_mgr::TriggerXferHandler );
	xferTable[ XFER_MONSTER ] = XferHandler( &object_mgr::DefaultXferHandler );
	xferTable[ XFER_HOLE ] = XferHandler( &object_mgr::HoleXferHandler );
	xferTable[ XFER_TRANSPORTER ] = XferHandler( &object_mgr::TransporterXferHandler );
	xferTable[ XFER_ELEVATOR ] = XferHandler( &object_mgr::ElevatorXferHandler );
	xferTable[ XFER_ELEVATORSHAFT ] = XferHandler( &object_mgr::ElevatorShaftXferHandler );
	xferTable[ XFER_MOVER ] = XferHandler( &object_mgr::MoverXferHandler );
	xferTable[ XFER_GLYPH ] = XferHandler( &object_mgr::GlyphXferHandler );
	xferTable[ XFER_INVISIBLELIGHT ] = XferHandler( &object_mgr::InvisibleLightXferHandler );
	xferTable[ XFER_SENTRY ] = XferHandler( &object_mgr::SentryXferHandler );
	xferTable[ XFER_WEAPON ] = XferHandler( &object_mgr::WeaponXferHandler );
	xferTable[ XFER_ARMOR ] = XferHandler( &object_mgr::ArmorXferHandler );
	xferTable[ XFER_TEAM ] = XferHandler( &object_mgr::TeamXferHandler );
	xferTable[ XFER_GOLD ] = XferHandler( &object_mgr::GoldXferHandler );
	xferTable[ XFER_AMMO ] = XferHandler( &object_mgr::AmmoXferHandler );
	xferTable[ XFER_NPC ] = XferHandler( &object_mgr::DefaultXferHandler );
	xferTable[ XFER_OBELISK ] = XferHandler( &object_mgr::ObeliskXferHandler );
	xferTable[ XFER_TOXICCLOUD ] = XferHandler( &object_mgr::ToxicCloudXferHandler );
	xferTable[ XFER_MONSTERGENERATOR ] = XferHandler( &object_mgr::DefaultXferHandler );
	xferTable[ XFER_REWARDMARKER ] = XferHandler( &object_mgr::RewardMarkerXferHandler );

	// physics
	b2BodyDef bodyDef;
	// we set the position to zero, because we use one static body for all static objects
	// these objects are then defined by shapes which use localPosition
	bodyDef.position.Set(0.0f, 0.0f);
	m_wall_body = world::instance->get_the_world().CreateBody(&bodyDef);
	m_static_body = world::instance->get_the_world().CreateBody(&bodyDef);
}

void object_mgr::update(uint32 diff)
{
	BOOST_FOREACH(object* o, m_objects)
	{
		o->update(diff);
	}
}

void object_mgr::add_object(object * obj)
{
	// dupe
	if(get_object(obj->m_extent) == obj)
		return;

	if(!is_extent_available(obj->m_extent))
		obj->m_extent = request_extent();
	m_extents[obj->m_extent] = obj;
	m_objects.insert(obj);
}

void object_mgr::remove_object(object * obj)
{
	std::set<object*>::iterator itr = m_objects.find(obj);
	if(itr == m_objects.end())
		return;
	m_objects.erase(itr);
	m_extents[obj->m_extent] = NULL;
}

object* object_mgr::create_object_from_file(NoxBuffer *rdr, NoxObjectTOC *toc)
{
	uint8 inventory = 0;

	uint16 type = rdr->read<uint16>(), type2 = 0;
	if(toc->find(type) == toc->end())
		return NULL;
	rdr->skip();
	
	type = ThingBin::instance->Thing.Object.GetIndex((*toc)[type].c_str(), (*toc)[type].size());
	//ASSERT(type != 0);

	object* obj = NULL;
	if(ThingBin::instance->Thing.Object.Objects.Get(type)->classes & CLASS_IMMOBILE)
		obj = new object(type);
	else
		obj = new world_object(type);

	size_t finish = rdr->read<int64>() + rdr->rpos();

	/*XferTableMap::iterator iter = xferTable.find(obj->GetObjectInfo()->xfer);
	if(iter != xferTable.end())
		inventory = (this->*iter->second.handler)(obj, rdr);*/
	inventory = DefaultXferHandler(obj, rdr);
	rdr->rpos(finish);

	//ASSERT(rdr->rpos() == finish);
	
	for(;inventory > 0; inventory--)
	{
		object* child = create_object_from_file(rdr, toc);
		if(child != NULL)
			obj->m_inventory.insert(child);
		else
			break;
	}

	object_mgr::instance->add_object(obj);
	return obj;
}

const CollideHandler* object_mgr::get_collide_handler(uint32 id)
{
	CollideTableMap::const_iterator iter = collideTable.find(id);
	if(iter != collideTable.end())
	{
		return &iter->second;
	}

	return NULL;
}
int object_mgr::DefaultXferHandler(object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	uint8 inventory = 0;

	obj->m_props1 = rdr->read<uint16>(); // properties
	obj->m_props2 = rdr->read<uint16>(); // type2
	obj->m_extent = (uint16)rdr->read<uint32>(); // extent
	rdr->read<uint32>();

	float x = rdr->read<float>();
	float y = rdr->read<float>();
	// since this adds this to the current world, we should save
	// these values someplace else, then have it "respawn" when
	// we want it to, TODO
	obj->set_position(x, y);

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

int object_mgr::GoldXferHandler(object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);
	obj->m_worth = rdr->read<uint32>();

	return inventory;
}

int object_mgr::ObeliskXferHandler(object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);
	if(obj->m_props1 < 0x3D)
		return inventory;

	rdr->read<uint32>();
	rdr->read<uint8>();

	return inventory;
}
int object_mgr::ToxicCloudXferHandler(object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->read<uint32>();

	return inventory;
}

int object_mgr::RewardMarkerXferHandler(object* obj, NoxBuffer* rdr)
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

int object_mgr::AmmoXferHandler(object* obj, NoxBuffer* rdr)
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
int object_mgr::SpellPagePedestalXferHandler(object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->read<uint32>();

	return inventory;
}
int object_mgr::SpellRewardXferHandler(object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->readstring<uint8>(buffer, 256);

	return inventory;
}

int object_mgr::AbilityRewardXferHandler(object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->readstring<uint8>(buffer, 256);

	return inventory;
}
int object_mgr::FieldGuideXferHandler(object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->readstring<uint8>(buffer, 256); // i think this is correct

	return inventory;
}
int object_mgr::ReadableXferHandler(object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->readstring<uint32>(buffer, 256); // might want to make this dynamic to allow for bigger text

	return inventory;
}
int object_mgr::ExitXferHandler(object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->readstring<uint32>(buffer, 256); // might want to make this dynamic to allow for bigger text
	rdr->read<uint32>();
	rdr->read<uint32>();

	return inventory;
}
int object_mgr::DoorXferHandler(object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->read<uint32>(); // direction, see for map editor code for details
	rdr->read<uint32>(); // lock
	rdr->read<uint32>(); // if obj->props1 > 0x29, i'll be shocked if we find one that isn't

	return inventory;
}
int object_mgr::TriggerXferHandler(object* obj, NoxBuffer* rdr)
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
int object_mgr::HoleXferHandler(object* obj, NoxBuffer* rdr)
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
int object_mgr::TransporterXferHandler(object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->read<uint32>();

	return inventory;
}
int object_mgr::ElevatorXferHandler(object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->read<uint32>();
	rdr->read<uint32>(); // if props1 > 0x28
	rdr->read<uint8>(); // if props1 > 0x3C

	return inventory;
}
int object_mgr::ElevatorShaftXferHandler(object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->read<uint32>();

	return inventory;
}
int object_mgr::MoverXferHandler(object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>(); //extent to be moved
	//if props1 > 0x28
	rdr->read<uint8>();
	rdr->read<uint32>();
	rdr->read<uint32>();

	rdr->read<float>();
	rdr->read<float>();

	return inventory;
}
int object_mgr::GlyphXferHandler(object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	//rdr->read<uint32>(); if props1 < 0x29
	rdr->read<uint8>();
	rdr->read<uint32>();
	rdr->read<uint32>();
	uint8 num_spells = rdr->read<uint8>();
	//if props < 0x1F read 0x14 bytes
	for(; num_spells; num_spells--)
		rdr->readstring<uint8>(buffer, 256);
	
	return inventory;
}

int object_mgr::InvisibleLightXferHandler(object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->read<uint32>();
	rdr->read<float>();
	rdr->read<uint32>();
	rdr->read<uint32>();

	//0xC
	rdr->read<uint32>(); 
	rdr->read<uint32>(); 
	rdr->read<uint32>(); 

	rdr->read<uint16>();
	rdr->read<uint16>();
	rdr->read<uint32>();
	rdr->read<uint16>();

	//0x30
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();
	
	//0x10
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();

	//0x10
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();

	rdr->read<uint16>();
	rdr->read<uint16>();
	rdr->read<uint16>();
	rdr->read<uint32>();
	rdr->read<uint16>();
	rdr->read<uint16>();
	rdr->read<uint8>();
	rdr->read<uint32>();
	
	return inventory;
}
int object_mgr::SentryXferHandler(object* obj, NoxBuffer* rdr)
{
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->read<float>();
	rdr->read<float>();
	if(obj->m_props1 > 0x3C)
		rdr->read<float>();

	return inventory;
}
int object_mgr::WeaponXferHandler(object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->readstring<uint8>(buffer, 256);
	rdr->readstring<uint8>(buffer, 256);
	rdr->readstring<uint8>(buffer, 256);
	rdr->readstring<uint8>(buffer, 256);

	rdr->read<uint16>(); //durability
	//rdr->read<uint8>() if props = 0x3F
	//rdr->read<uint32>() if props = 0x40
	
	return inventory;
}
int object_mgr::ArmorXferHandler(object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->readstring<uint8>(buffer, 256);
	rdr->readstring<uint8>(buffer, 256);
	rdr->readstring<uint8>(buffer, 256);
	rdr->readstring<uint8>(buffer, 256);

	rdr->read<uint16>(); //durability
	//rdr->read<uint8>() if props = 0x3F
	//rdr->read<uint32>() if props = 0x40
	
	return inventory;
}
int object_mgr::TeamXferHandler(object* obj, NoxBuffer* rdr)
{
	char buffer[256];
	int inventory = DefaultXferHandler(obj, rdr);

	rdr->readstring<uint8>(buffer, 256);
	rdr->readstring<uint8>(buffer, 256);
	rdr->readstring<uint8>(buffer, 256);
	rdr->readstring<uint8>(buffer, 256);
	
	return inventory;
}
