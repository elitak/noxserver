#ifndef THING_LIB
#define THING_LIB


#include "NoxThingClassDefs.h"
#include "Policies/Singleton.h"
using namespace std;


//////
// Properties store object strings and information 
//////
  struct Property
   {
	   unsigned char ValueLen;
	   char Value[255]; // Changed to 255 to save memory
   };

//
// DUA: Alias AUD, stores sound properties.
//
struct DUA
{
  // DUA ID
  unsigned int Unknown; //Type id? Section len if not 01?
  BYTE nameLen;
  BYTE Name[25];
  BYTE Unknowns[9];
  MLIST<Property> Sounds;

   DUA()
   {
      memset(this,0x00,sizeof(*this));
   }
};
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
struct TNVA
{
  //TNVA ID
  unsigned int Unknown; //Type id? Section len if not 01?
  BYTE nameLen;
  BYTE Name[50];
  BYTE Unknowns[9];
  MLIST<Property> Sounds;

   TNVA()
   {
      memset(this,0x00,sizeof(*this));
   }
};
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
struct GAMI
{
  BYTE Name[50];
  BYTE aniName[50];
  BYTE nameLen;
  unsigned int ImageCode;//[70];
  MLIST<unsigned int> Images;

   GAMI()
   {
      memset(this,0x00,sizeof(*this));
   }
};

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class LOOP
{
	private:
    public:
	unsigned char Name[50];
    MLIST<unsigned int> Images;
	//Image_List List;
};

///////////////////////////////////////////////////////////
   class TATS
   {	
	 public:
           unsigned int SubCat;
           BYTE loopName[50];
		   MLIST<unsigned int> Images;
           BYTE nullString[50];
     TATS()
     {
        SubCat = 0;
     }
	 ~TATS()
	 {
		 Images.Clear();
	 }
   };

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

enum NoxClassType
{
	CLASS_NULL = 0x00000000,
	CLASS_MONSTER = 0x00000001,
	CLASS_PLAYER = 0x00000002,
	CLASS_OBSTACLE = 0x00000004,
	CLASS_FOOD = 0x00000008,
	CLASS_EXIT = 0x00000010,
	CLASS_KEY = 0x00000020,
	CLASS_DOOR = 0x00000040,
	CLASS_INFO_BOOK = 0x000080,
	CLASS_TRIGGER = 0x00000100,
	CLASS_TRANSPORTER = 0x00000200,
	CLASS_HOLE = 0x00000400,
	CLASS_WAND = 0x00000800,
	CLASS_FIRE = 0x00001000,
	CLASS_ELEVATOR = 0x00002000,
	CLASS_ELEVATOR_SHAFT = 0x00004000,
	CLASS_DANGEROUS = 0x00008000,
	CLASS_MONSTERGENERATOR = 0x00010000,
	CLASS_READABLE = 0x00020000,
	CLASS_LIGHT = 0x00040000,
	CLASS_SIMPLE = 0x00080000,
	CLASS_COMPLEX = 0x00100000,
	CLASS_IMMOBILE = 0x00200000,
	CLASS_VISIBLE_ENABLE = 0x00400000,
	CLASS_WEAPON = 0x00800000,
	CLASS_ARMOR = 0x01000000,
	CLASS_NOT_STACKABLE = 0x02000000,
	CLASS_TREASURE = 0x04000000,
	CLASS_FLAG = 0x08000000,
	CLASS_CLIENT_PERSIST = 0x10000000,
	CLASS_CLIENT_PREDICT = 0x20000000,
	CLASS_PICKUP = 0x40000000,
	CLASS_MISSILE = 0x80000000, //nox has this as 0x00000001, if you are bored fix it
};
enum NoxSubclassType
{
	// everything in 0x0 - 0xF range is reserved for use by Unit::UnitSlotType
	SUBCLASS_HELMET = 0x0,
	SUBCLASS_BREASTPLATE = 0x1,
	SUBCLASS_ARM_ARMOR = 0x2,
	SUBCLASS_PANTS = 0x3,
	SUBCLASS_BOOTS = 0x4,
	SUBCLASS_SHIRT = 0x5,
	SUBCLASS_LEG_ARMOR = 0x6,
	SUBCLASS_BACK = 0x7,
	SUBCLASS_SHIELD = 0x8,
	SUBCLASS_WEAPON = 0x20, //
	SUBCLASS_FLAG = 0x20,
	SUBCLASS_QUIVER = 0x21,
	SUBCLASS_BOW = 0x22,
	SUBCLASS_CROSSBOW = 0x23,
	SUBCLASS_ARROW = 0x24,
	SUBCLASS_BOLT = 0x25,
	SUBCLASS_CHAKRAM = 0x26,
	SUBCLASS_SHURIKEN = 0x27,
	SUBCLASS_SWORD = 0x28,
	SUBCLASS_LONG_SWORD = 0x29,
	SUBCLASS_GREAT_SWORD = 0x2A,
	SUBCLASS_MACE = 0x2B,
	SUBCLASS_AXE = 0x2C,
	SUBCLASS_OGRE_AXE = 0x2D,
	SUBCLASS_HAMMER = 0x2E,
	SUBCLASS_STAFF = 0x2F,
	SUBCLASS_STAFF_SULPHOROUS_FLARE = 0x30,
	SUBCLASS_STAFF_SULPHOROUS_SHOWER = 0x31,
	SUBCLASS_STAFF_LIGHTNING = 0x32,
	SUBCLASS_STAFF_FIREBALL = 0x33,
	SUBCLASS_STAFF_TRIPLE_FIREBALL = 0x34,
	SUBCLASS_STAFF_FORCE_OF_NATURE = 0x35,
	SUBCLASS_STAFF_DEATH_RAY = 0x36,
	SUBCLASS_STAFF_OBLIVION_HALBERD = 0x37,
	SUBCLASS_STAFF_OBLIVION_HEART = 0x38,
	SUBCLASS_STAFF_OBLIVION_WIERDLING = 0x39,
	SUBCLASS_STAFF_OBLIVION_ORB = 0x3A
};
enum NoxFlagType
{
	FLAG_NULL = 0x00000000,
	FLAG_BELOW = 0x00000001,
	FLAG_NO_UPDATE = 0x00000002,
	FLAG_ACTIVE = 0x00000004,
	FLAG_ALLOW_OVERLAP = 0x00000008,
	FLAG_SHORT = 0x00000010,
	FLAG_DESTROYED = 0x00000020,
	FLAG_NO_COLLIDE = 0x00000040,
	FLAG_MISSILE_HIT = 0x00000080,
	FLAG_EQUIPPED = 0x00000100,
	FLAG_PARTITIONED = 0x00000200,
	FLAG_NO_COLLIDE_OWNER = 0x00000400,
	FLAG_OWNER_VISIBLE = 0x00000800,
	FLAG_EDIT_VISIBLE = 0x00001000,
	FLAG_NO_PUSH_CHARACTERS = 0x00002000,
	FLAG_AIRBORNE = 0x00004000,
	FLAG_DEAD = 0x00008000,
	FLAG_SHADOW = 0x00010000,
	FLAG_FALLING = 0x00020000,
	FLAG_IN_HOLE = 0x00040000,
	FLAG_RESPAWN = 0x00080000,
	FLAG_ON_OBJECT = 0x00100000,
	FLAG_SIGHT_DESTROY = 0x00200000,
	FLAG_TRANSIENT = 0x00400000,
	FLAG_BOUNCY = 0x00800000,
	FLAG_ENABLED = 0x01000000,
	FLAG_PENDING = 0x02000000,
	FLAG_TRANSLUCENT = 0x04000000,
	FLAG_STILL = 0x08000000,
	FLAG_NO_AUTO_DROP = 0x10000000,
	FLAG_FLICKER = 0x20000000,
	FLAG_SELECTED = 0x40000000,
	FLAG_MARKED = 0x80000000
};

enum NoxCollideType
{
	COLLIDE_NONE,
	COLLIDE_DEFAULT,
	COLLIDE_MONSTER,
	COLLIDE_PLAYER,
	COLLIDE_PROJECTILE,
	COLLIDE_PROJECTILESPARK,
	COLLIDE_DOOR,
	COLLIDE_PICKUP,
	COLLIDE_EXIT,
	COLLIDE_DAMAGE,
	COLLIDE_MANADRAIN,
	COLLIDE_BOMB,
	COLLIDE_SPARKEXPLOSION,
	COLLIDE_CHEST,
	COLLIDE_WALLREFLECT,
	COLLIDE_WALLREFLECTSPARK,
	COLLIDE_PIXIE,
	COLLIDE_OWN,
	COLLIDE_SPARK,
	COLLIDE_BARREL,
	COLLIDE_AUDIOEVENT,
	COLLIDE_TRIGGER,
	COLLIDE_TELEPORT,
	COLLIDE_ELEVATOR,
	COLLIDE_AWARDSPELL,
	COLLIDE_DIE,
	COLLIDE_GLYPH,
	COLLIDE_SPELLPROJECTILE,
	COLLIDE_BOOM,
	COLLIDE_SIGN,
	COLLIDE_PENTAGRAM,
	COLLIDE_SPIDERSPIT,
	COLLIDE_DEATHBALL,
	COLLIDE_DEATHBALLFRAGMENT,
	COLLIDE_TELEKINESIS,
	COLLIDE_FIST,
	COLLIDE_TELEPORTWAKE,
	COLLIDE_FLAG,
	COLLIDE_CHAKRAMINMOTION,
	COLLIDE_ARROW,
	COLLIDE_MONSTERARROW,
	COLLIDE_BEARTRAP,
	COLLIDE_POISONGASTRAP,
	COLLIDE_TRAPDOOR,
	COLLIDE_BALL,
	COLLIDE_HOMEBASE,
	COLLIDE_CROWN,
	COLLIDE_UNDEADKILLER,
	COLLIDE_YELLOWSTARSHOT,
	COLLIDE_MIMIC,
	COLLIDE_HARPOON,
	COLLIDE_MONSTERGENERATOR,
	COLLIDE_SOULGATE,
	COLLIDE_ANKH
};

enum NoxUpdateType
{
	UPDATE_NONE,
	UPDATE_PLAYER,
	UPDATE_PROJECTILE,
	UPDATE_HOMINGPROJECTILE,
	UPDATE_SPELLPROJECTILE,
	UPDATE_ANTISPELLPROJECTILE,
	UPDATE_DOOR,
	UPDATE_SPARK,
	UPDATE_PROJECTILETRAIL,
	UPDATE_PUSH,
	UPDATE_TRIGGER,
	UPDATE_TOGGLE,
	UPDATE_MONSTER,
	UPDATE_LOOPANDDAMAGE,
	UPDATE_ELEVATOR,
	UPDATE_ELEVATORSHAFT,
	UPDATE_PHANTOMPLAYER,
	UPDATE_OBELISK,
	UPDATE_LIFETIME,
	UPDATE_MAGICMISSILE,
	UPDATE_PIXIE,
	UPDATE_SPIKEBLOCK,
	UPDATE_TOWER,
	UPDATE_SKULL,
	UPDATE_PENTAGRAM,
	UPDATE_INVISIBLEPENTAGRAM,
	UPDATE_SWITCH,
	UPDATE_BLOW,
	UPDATE_MOVER,
	UPDATE_BLACKPOWDERBARREL,
	UPDATE_ONESECONDDIE,
	UPDATE_WATERBARREL,
	UPDATE_SELFDESTRUCT,
	UPDATE_BLACKPOWDERBURN,
	UPDATE_DEATHBALL,
	UPDATE_DEATHBALLFRAGMENT,
	UPDATE_MOONGLOW,
	UPDATE_SENTRYGLOBE,
	UPDATE_TELEKINESIS,
	UPDATE_FIST,
	UPDATE_METEORSHOWER,
	UPDATE_METEOR,
	UPDATE_TOXICCLOUD,
	UPDATE_SMALLTOXICCLOUD,
	UPDATE_ARACHNAPHOBIA,
	UPDATE_EXPIRE,
	UPDATE_BREAK,
	UPDATE_OPEN,
	UPDATE_BREAKANDREMOVE,
	UPDATE_CHAKRAMINMOTION,
	UPDATE_FLAG,
	UPDATE_TRAPDOOR,
	UPDATE_BALL,
	UPDATE_CROWN,
	UPDATE_UNDEADKILLER,
	UPDATE_HARPOON,
	UPDATE_WEAPONARMOR,
	UPDATE_MONSTERGENERATOR
};
enum NoxUseType
{
	USE_CONSUME,
	USE_CONSUMECONFUSE,
	USE_FIREWAND,
	USE_CAST,
	USE_ENCHANT,
	USE_MUSHROOM,
	USE_READ,
	USE_WARPREAD,
	USE_WAND,
	USE_WANDCAST,
	USE_SPELLREWARD,
	USE_ABILITYREWARD,
	USE_FIELDGUIDE,
	USE_POTION,
	USE_AMMO,
	USE_BOW
};
enum NoxXferType
{
	XFER_DEFAULT,
	XFER_SPELLPAGEPEDESTAL,
	XFER_SPELLREWARD,
	XFER_ABILITYREWARD,
	XFER_FIELDGUIDE,
	XFER_READABLE,
	XFER_EXIT,
	XFER_DOOR,
	XFER_TRIGGER,
	XFER_MONSTER,
	XFER_HOLE,
	XFER_TRANSPORTER,
	XFER_ELEVATOR,
	XFER_ELEVATORSHAFT,
	XFER_MOVER,
	XFER_GLYPH,
	XFER_INVISIBLELIGHT,
	XFER_SENTRY,
	XFER_WEAPON,
	XFER_ARMOR,
	XFER_TEAM,
	XFER_GOLD,
	XFER_AMMO,
	XFER_NPC,
	XFER_OBELISK,
	XFER_TOXICCLOUD,
	XFER_MONSTERGENERATOR,
	XFER_REWARDMARKER
};
enum NoxExtentShapeType
{
	EXTENT_BOX,
	EXTENT_CIRCLE,
	EXTENT_CENTER
};

struct NoxExtentType
{
	uint8 shape;
	union {
		uint8 x;
		uint8 radius;
	};
	uint8 y;
};

struct GNHT // Object Class
{
   BYTE obType;
   bool typeFound;

   char Name[50];
   BYTE nameLen;
   int Num_Headers;
   int Header_Nums[5];
   unsigned int ImageCode;
   uint32 flags;
   uint32 classes;
   uint32 subclass;
   uint32 collide;
   uint32 update;
   uint32 use;
   uint32 xfer;
   char use_args[5][30];
   char update_args[5][30];
   char collide_args[5][30];
   NoxExtentType extent;
   //Image_List List;
   //MLIST<unsigned int> Images;
   MLIST<Property> Properties;

 
   //IMAGE AND ANIMATION STRUCTS
   unsigned int MenuIcon;
   unsigned int PrettyImage;

   char aniName[255];

   //PLAYER/MONSTER DRAW FUNCS
   MLIST<TATS> Stats;
   MLIST<TATS> PlayerStats;
   MLIST<Property> Subcats;
   ///////////////////////////   
   GNHT()
   {   
      memset(this,0x00,sizeof(*this));	   
   }
   ~GNHT()
   {
   }

};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct LLAW
{
       //#TODO: Figure out what the unknown 14s do and add the image properties
       unsigned int Unknown; //In everything, ID maybe?
       BYTE nameLen;
       BYTE Name[25];
       BYTE Unknowns[14];//14 of them; may not be BYTES
	   MLIST<Property> Objects;

       BYTE Sound_Open_Len;
       BYTE SoundOpen[25]; 
       BYTE Sound_Close_Len;
       BYTE SoundClose[25]; 
       BYTE Sound_Destroy_Len;
       BYTE SoundDestroy[25]; 

       unsigned int ImageCode;
	   MLIST<unsigned int> Images;

   LLAW()
   {
      memset(this,0x00,sizeof(*this));	
   }

};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct EGDE
{

      //#TODO: Figure out exactly what the 14 unknown tile bytes mean
	  //       Damage,Speed,Gamma,Water,Snow?

      //EGDE ID
	  int Unknown; //In everything, maybe be type ID
      BYTE nameLen; // Name Len
      char Name[25]; // Name
      BYTE Unknowns[14]; //14 unknown bytes - must be tile properties
	  MLIST<unsigned int> Images;

  EGDE()
   {
      memset(this,0x00,sizeof(*this));
   }
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct ROLF
{

      //#TODO: Figure out exactly what the 14 unknown tile bytes mean
	  //       Damage,Speed,Gamma,Water,Snow?

      //ROLF ID
	  int Unknown; //In everything, maybe be type ID
      BYTE nameLen; // Name Len
      char Name[25]; // Name
      BYTE Unknowns[14]; //14 unknown bytes - must be tile properties
      short one; //should be 0x01 - After this starts the images
	  FASTLIST<unsigned int> Images;

  ROLF()
   {
      memset(this,0x00,sizeof(*this));
   }
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct DUASTRUCT
{
public:
   MLIST<DUA> Sounds;
   DUASTRUCT()
   {}

	unsigned short GetIndex(const char* name, int max = 0x50)
	{
		DUA* aud;
		int maxlen = 50 > max ? 50 : max;
		unsigned short id = 0;
		while(aud = Sounds.Get())
		{
			id++;
			if(!strncmp((const char*)aud->Name, name, maxlen))
			{
				Sounds.ClearGet();
				return id;
			}
		}
		Sounds.ClearGet();
		return 0;
	}
   //{delete [] Tiles;} //ERROR PRONE?
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct TNVASTRUCT
{
public:
   MLIST<TNVA> Animations;
   TNVASTRUCT()
   {}
   //{delete [] Tiles;} //ERROR PRONE?
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct GAMISTRUCT
{
public:
   MLIST<GAMI> Images;
   GAMISTRUCT()
   {}
   //{delete [] Tiles;} //ERROR PRONE?
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct ROLFSTRUCT
{
public:
  MLIST<ROLF> Tiles;

  ROLFSTRUCT()
  {}
//{delete [] Tiles;} //ERROR PRONE?
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct EGDESTRUCT
{
public:
  MLIST<EGDE> Edges;

  EGDESTRUCT()
  {}
//{delete [] Tiles;} //ERROR PRONE?
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct LLAWSTRUCT
{
public:
	MLIST<LLAW> Walls;

    LLAWSTRUCT()
	{}
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct GNHTSTRUCT
{
	public:
		MLIST<GNHT> Objects;

		unsigned short GetIndex(const char* name, int max = 0x50)
		{
			GNHT* obj;
			int maxlen = 50 > max ? 50 : max;
			unsigned short id = 0;
			while(obj = Objects.Get())
			{
				id++;
				if(!strncmp(obj->Name, name, maxlen))
				{
					Objects.ClearGet();
					return id;
				}
			}
			Objects.ClearGet();
			return 0;
		}

		GNHT * GetObject(const char* name, int max = 50)
		{
			GNHT* obj;
			int maxlen = 50 > max ? 50 : max;
			while(obj = Objects.Get())
			{
				if(!strncmp(obj->Name, name, maxlen))
				{
					Objects.ClearGet();
					return obj;
				}
			}
			Objects.ClearGet();
			return NULL;
		}

		GNHT * GetObject( unsigned short i )
		{
			return(Objects.Get(i));
		}

//~LLAWSTRUCT()
//{delete [] Walls;} //ERROR PRONE?
		~GNHTSTRUCT()
		{
		}
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct ThingDB
{
   LLAWSTRUCT Wall;
   ROLFSTRUCT Tile;
   EGDESTRUCT Edge;
   GNHTSTRUCT Object;
   GAMISTRUCT Image;
   TNVASTRUCT Animations;
   DUASTRUCT  Sounds;

~ThingDB()
{}
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

struct NoxEnumNamePair
{
	uint32 Enum;
	const char* Name;
};

class ThingBin
{
public:
         char Type[5];
         ThingDB Thing;
		 bool Is_Loaded;

         fstream *file;
         fstream out;

         bool Load_Thingdb(fstream *file);
         bool Load_Tiles(unsigned char *buff,long &count);
         bool Load_Sounds(unsigned char *buff,long &count);
         bool Load_Avents(unsigned char *buff,long &count);
         bool Load_Walls(unsigned char *buff,long &count,LLAWSTRUCT & Data);
         bool Load_Images(unsigned char *buff,long &count,GAMISTRUCT & Data);
         bool Load_Objects(unsigned char *buff,long &count,long filelen,GNHTSTRUCT & Data);

		 static uint32 noxNameToEnum (const char* name, NoxEnumNamePair table[]);

   ThingBin();
   ~ThingBin()
   {
   }


      private:
              bool Load_Avnt(long &count,unsigned char *buff, TNVA & Data);
		      bool Load_Aud(long &count,unsigned char *buff, DUA & Data);
              bool Load_Tile(long &count,unsigned char *buff, ROLF & Data);
              bool Load_Edge(long &count,unsigned char *buff, EGDE & Data); 
              bool Load_Wall(long &count,unsigned char *buff, LLAW & Data); 
              bool Load_Image(long &count,unsigned char *buff, GAMI & Data); 
              bool Load_Object(long &count,unsigned char *buff,long filelen, GNHT & Data); 

			  int Parse_String(char str[],int strLen);
};

#define sThingBin MaNGOS::Singleton<ThingBin>::Instance()
#endif