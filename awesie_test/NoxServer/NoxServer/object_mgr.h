#pragma once

#include "global.h"
#include "object.h"
#include "player.h"
#include "NoxBuffer.h"
#include "NoxMap.h"

#include <set>

#define INVALID_EXTENT	0x0000
#define MAX_EXTENT		0x7FFF //extents are actually signed, lol

/// Player state
enum session_status
{
    STATUS_AUTHED = 0,                                      ///< Player authenticated
	STATUS_CONFIRMED,
    STATUS_LOGGEDIN                                         ///< Player in game
};

struct OpcodeHandler
{
    //OpcodeHandler() : status(STATUS_AUTHED), handler(NULL) {};
    //OpcodeHandler( SessionStatus _status, void (WorldSession::*_handler)(WorldPacket& recvPacket) ) : status(_status), handler(_handler) {};

    //SessionStatus status;
    //void (WorldSession::*handler)(WorldPacket& recvPacket);
};
struct CollideHandler
{
	CollideHandler() : mask(0xFFFFFFFF), handler(NULL) {};
	//CollideHandler( Flatland::Callback _handler, uint32 _mask = 0xFFFFFFFF ) : mask(_mask), handler(_handler) {};

	uint32 mask;
	int32 handler;
};
struct UpdateHandler
{
	UpdateHandler() : handler(NULL) {};
	UpdateHandler( void (object::*_handler)(time_t diff) ) : handler(_handler) {};

	void (object::*handler)(time_t diff);
};
struct UseHandler
{
	UseHandler() : handler(NULL) {};
	UseHandler( void (object::*_handler)(player* plr) ) : handler(_handler) {};

	void (object::*handler)(player* plr);
};
struct PickupHandler
{
	PickupHandler() : handler(NULL) {};
	PickupHandler( bool (object::*_handler)(player* plr) ) : handler(_handler) {};

	bool (object::*handler)(player* plr);
};
struct XferHandler
{
	XferHandler() : handler(NULL) {};
	XferHandler( int (object_mgr::*_handler)(object* obj, NoxBuffer* rdr) ) : handler(_handler) {};

	int (object_mgr::*handler)(object* obj, NoxBuffer* rdr);
};
struct EnchantEntry
{
	EnchantEntry() : name(NULL), frames(0) {};
	EnchantEntry( char* _name, int16 _frames = 0 ) : name(_name), frames(_frames) {};

	char* name;
	int16 frames;
};

typedef std::map< uint16 , OpcodeHandler > OpcodeTableMap;
typedef std::map< uint16 , CollideHandler > CollideTableMap;
typedef std::map< uint16 , UpdateHandler > UpdateTableMap;
typedef std::map< uint16 , UseHandler > UseTableMap;
typedef std::map< uint16 , XferHandler > XferTableMap;
typedef std::map< uint16 , PickupHandler > PickupTableMap;
typedef std::map< uint32 , EnchantEntry > EnchantTableMap;

class object_mgr : public boost::singleton<object_mgr>
{
public:
	object_mgr(boost::restricted);
	
	void add_object(object* obj);
	void remove_object(object* obj);
	object* get_object(int16 extent)
	{
		return m_extents[extent];
	}
	uint16 request_extent()
	{
		uint32* ptr = (uint32*)m_extents;
		do {
			ptr++;
		} while(ptr < (uint32*)(m_extents+MAX_EXTENT+1) && *ptr != 0);
		if(ptr >= (uint32*)(m_extents+MAX_EXTENT+1))
			return INVALID_EXTENT;
		
		uint16 extent = (uint16)((ptr - (uint32*)m_extents));

		return extent;
	}
	bool is_extent_available(uint16 extent)
	{
		return extent <= MAX_EXTENT && extent != INVALID_EXTENT && m_extents[extent] == NULL;
	}

	object* create_object_from_file(NoxBuffer* rdr, NoxObjectTOC* toc);

	// physics
	b2Body* get_static_body();
	b2Body* get_walls_body();
protected:
	std::set<object*> m_objects;
	object* m_extents[MAX_EXTENT+1];

	// physics
	b2Body* m_wall_body;
	b2Body* m_static_body;

	OpcodeTableMap opcodeTable;
	CollideTableMap collideTable;
	UpdateTableMap updateTable;
	EnchantTableMap enchantTable;
	UseTableMap useTable;
	PickupTableMap pickupTable;
	XferTableMap xferTable;

	/// XFER Handlers (used to create object from map file)
	int DefaultXferHandler(object* obj, NoxBuffer* rdr);
	int GoldXferHandler(object* obj, NoxBuffer* rdr);
	int ObeliskXferHandler(object* obj, NoxBuffer* rdr);
	int ToxicCloudXferHandler(object* obj, NoxBuffer* rdr);
	int RewardMarkerXferHandler(object* obj, NoxBuffer* rdr);
	int AmmoXferHandler(object* obj, NoxBuffer* rdr);
	int AbilityRewardXferHandler(object* obj, NoxBuffer* rdr);
	int SpellPagePedestalXferHandler(object* obj, NoxBuffer* rdr);
	int SpellRewardXferHandler(object* obj, NoxBuffer* rdr);
	int FieldGuideXferHandler(object* obj, NoxBuffer* rdr);
	int ReadableXferHandler(object* obj, NoxBuffer* rdr);
	int ExitXferHandler(object* obj, NoxBuffer* rdr);
	int DoorXferHandler(object* obj, NoxBuffer* rdr);
	int TriggerXferHandler(object* obj, NoxBuffer* rdr);
	int HoleXferHandler(object* obj, NoxBuffer* rdr);
	int TransporterXferHandler(object* obj, NoxBuffer* rdr);
	int ElevatorXferHandler(object* obj, NoxBuffer* rdr);
	int ElevatorShaftXferHandler(object* obj, NoxBuffer* rdr);
	int MoverXferHandler(object* obj, NoxBuffer* rdr);
	int GlyphXferHandler(object* obj, NoxBuffer* rdr);
	int InvisibleLightXferHandler(object* obj, NoxBuffer* rdr);
	int SentryXferHandler(object* obj, NoxBuffer* rdr);
	int WeaponXferHandler(object* obj, NoxBuffer* rdr);
	int ArmorXferHandler(object* obj, NoxBuffer* rdr);
	int TeamXferHandler(object* obj, NoxBuffer* rdr);
};