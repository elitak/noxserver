#ifndef THING_LIB
#define THING_LIB


#include "NoxThingClassDefs.h"
#include "Policies/Singleton.h"
using namespace std;

  struct Property
   {
	   unsigned int ValueLen;
	   char Value[512];
   };

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
struct DUA
{
  // DUA ID
  unsigned int Unknown; //Type id? Section len if not 01?
  BYTE nameLen;
  BYTE Name[25];
  BYTE Unknowns[9];
  LIST2<Property> Sounds;

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
  LIST2<Property> Sounds;

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
  LIST2<unsigned int> Images;
  //int numImages;
  //Image_List List;

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
    LIST2<unsigned int> Images;
	//Image_List List;
};

///////////////////////////////////////////////////////////
   class TATS
   {	
	 public:
           unsigned int SubCat;
           BYTE loopName[50];
		   LIST2<unsigned int> Images;
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
	SUBCLASS_SLOT_BEGIN = 0x0,
	SUBCLASS_HELMET = 0x0,
	SUBCLASS_BREASTPLATE = 0x1,
	SUBCLASS_ARM_ARMOR = 0x2,
	SUBCLASS_PANTS = 0x3,
	SUBCLASS_BOOTS = 0x4,
	SUBCLASS_SHIRT = 0x5,
	SUBCLASS_LEG_ARMOR = 0x6,
	SUBCLASS_BACK = 0x7,
	SUBCLASS_SHIELD = 0x8,
	SUBCLASS_FLAG = 0x9,
	SUBCLASS_QUIVER = 0xA,
	SUBCLASS_SLOT_LAST = 0xF,
	SUBCLASS_WEAPON = 0x40,
	SUBCLASS_BOW = 0x41,
	SUBCLASS_CROSSBOW = 0x32,
	SUBCLASS_ARROW = 0x43,
	SUBCLASS_BOLT = 0x44,
	SUBCLASS_CHAKRAM = 0x45,
	SUBCLASS_SHURIKEN = 0x46,
	SUBCLASS_SWORD = 0x47,
	SUBCLASS_LONG_SWORD = 0x48,
	SUBCLASS_GREAT_SWORD = 0x49,
	SUBCLASS_MACE = 0x4A,
	SUBCLASS_AXE = 0x4B,
	SUBCLASS_OGRE_AXE = 0x4C,
	SUBCLASS_HAMMER = 0x4D,
	SUBCLASS_STAFF = 0xC0,
	SUBCLASS_STAFF_SULPHOROUS_FLARE = 0xC1,
	SUBCLASS_STAFF_SULPHOROUS_SHOWER = 0xC2,
	SUBCLASS_STAFF_LIGHTNING = 0xC3,
	SUBCLASS_STAFF_FIREBALL = 0xC4,
	SUBCLASS_STAFF_TRIPLE_FIREBALL = 0xC5,
	SUBCLASS_STAFF_FORCE_OF_NATURE = 0xC6,
	SUBCLASS_STAFF_DEATH_RAY = 0xC7,
	SUBCLASS_STAFF_OBLIVION_HALBERD = 0xC8,
	SUBCLASS_STAFF_OBLIVION_HEART = 0xC9,
	SUBCLASS_STAFF_OBLIVION_WIERDLING = 0xCA,
	SUBCLASS_STAFF_OBLIVION_ORB = 0xCB
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

struct GNHT // Object Class
{
   BYTE obType;
   bool typeFound;

   char Name[50];
   BYTE nameLen;
   //char HeaderNames[5][20];
   int Num_Headers;
   int Header_Nums[5];
   unsigned int ImageCode;
   uint32 flags;
   uint32 classes;
   uint32 subclass;
   //Image_List List;
   //LIST2<unsigned int> Images;
 
   //int numStrings;
   //char propStrings[25][255];

   LIST2<Property> Properties;

 
   //IMAGE AND ANIMATION STRUCTS
   unsigned int MenuIcon;
   unsigned int PrettyImage;

   char aniName[255];

   //PLAYER/MONSTER DRAW FUNCS
   
   //TATS Stat[70];
   //int numStats;
   LIST2<TATS> Stats;
   LIST2<TATS> PlayerStats;
   //TATS *PlayerStat;
   //int numPlayerStats;
   LIST2<Property> Subcats;
   //CString *SubCats;
   //int numSubCats;
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
       //long numObjects;
	   LIST2<Property> Objects;

       BYTE Sound_Open_Len;
       BYTE SoundOpen[25]; 
       BYTE Sound_Close_Len;
       BYTE SoundClose[25]; 
       BYTE Sound_Destroy_Len;
       BYTE SoundDestroy[25]; 
       //int numImages;
       unsigned int ImageCode;
	   LIST2<unsigned int> Images;

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
	  LIST2<unsigned int> Images;

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
   LIST2<DUA> Sounds;
   DUASTRUCT()
   {}
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
   LIST2<TNVA> Animations;
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
   LIST2<GAMI> Images;
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
  LIST2<ROLF> Tiles;

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
  LIST2<EGDE> Edges;

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
	LIST2<LLAW> Walls;

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
		LIST2<GNHT> Objects;

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

/*int Get_Object_Loc(char ObName[100])
{
  for(int i=0; i<Thing.Object.numObjects; i++)
  {
    if(!_stricmp(ObName,Thing.Object.Objects[i].Name))
	{return(i+1);}
  }
  return(0);
}*/
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