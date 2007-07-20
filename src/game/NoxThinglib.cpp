#include "NoxThinglib.h"
#include "NoxCrypt.h"
#include "WorldLog.h"
#include "Policies/SingletonImp.h"

INSTANTIATE_SINGLETON_1( ThingBin );

uint32 ThingBin::noxNameToEnum (const char* name, NoxEnumNamePair table[])
{
	NoxEnumNamePair *pair = table;
	while(pair->Name != NULL)
	{
		if(!strcmp(name, pair->Name))
			break;
		pair++;
	}
	return pair->Enum;
}

NoxEnumNamePair g_noxClassNames[] = 
{
	CLASS_MISSILE, "MISSILE",
	CLASS_MONSTER, "MONSTER",
	CLASS_PLAYER, "PLAYER",
	CLASS_OBSTACLE, "OBSTACLE",
	CLASS_FOOD, "FOOD",
	CLASS_EXIT, "EXIT",
	CLASS_KEY, "KEY",
	CLASS_DOOR, "DOOR",
	CLASS_INFO_BOOK, "INFO_BOOK",
	CLASS_TRIGGER, "TRIGGER",
	CLASS_TRANSPORTER, "TRANSPORTER",
	CLASS_HOLE, "HOLE",
	CLASS_WAND, "WAND",
	CLASS_FIRE, "FIRE",
	CLASS_ELEVATOR, "ELEVATOR",
	CLASS_ELEVATOR_SHAFT, "ELEVATOR_SHAFT",
	CLASS_DANGEROUS, "DANGEROUS",
	CLASS_MONSTERGENERATOR, "MONSTERGENERATOR",
	CLASS_READABLE, "READABLE",
	CLASS_LIGHT, "LIGHT",
	CLASS_SIMPLE, "SIMPLE",
	CLASS_COMPLEX, "COMPLEX",
	CLASS_IMMOBILE, "IMMOBILE",
	CLASS_VISIBLE_ENABLE, "ENABLE",
	CLASS_WEAPON, "WEAPON",
	CLASS_ARMOR, "ARMOR",
	CLASS_NOT_STACKABLE, "NOT_STACKABLE",
	CLASS_TREASURE, "TREASURE",
	CLASS_FLAG, "FLAG",
	CLASS_CLIENT_PERSIST, "CLIENT_PERSIST",
	CLASS_CLIENT_PREDICT, "CLIENT_PREDICT",
	CLASS_PICKUP, "PICKUP",
	0, NULL
};

NoxEnumNamePair g_noxSubclassNames[] =
{
	SUBCLASS_HELMET, "HELMET",
	SUBCLASS_SHIELD, "SHIELD",
	SUBCLASS_BREASTPLATE, "BREASTPLATE",
	SUBCLASS_ARM_ARMOR, "ARM_ARMOR",
	SUBCLASS_PANTS, "PANTS",
	SUBCLASS_BOOTS, "BOOTS",
	SUBCLASS_SHIRT, "SHIRT",
	SUBCLASS_LEG_ARMOR, "LEG_ARMOR",
	SUBCLASS_BACK,"BACK",
	SUBCLASS_FLAG, "FLAG",
	SUBCLASS_QUIVER, "QUIVER",
	SUBCLASS_BOW, "BOW",
	SUBCLASS_CROSSBOW, "CROSSBOW",
	SUBCLASS_ARROW, "ARROW",
	SUBCLASS_BOLT, "BOLT",
	SUBCLASS_CHAKRAM, "CHAKRAM",
	SUBCLASS_SHURIKEN, "SHURIKEN",
	SUBCLASS_SWORD, "SWORD",
	SUBCLASS_LONG_SWORD, "LONG_SWORD",
	SUBCLASS_GREAT_SWORD, "GREAT_SWORD",
	SUBCLASS_MACE, "MACE",
	SUBCLASS_AXE, "AXE",
	SUBCLASS_OGRE_AXE, "OGRE_AXE",
	SUBCLASS_HAMMER, "HAMMER",
	SUBCLASS_STAFF, "STAFF",
	SUBCLASS_STAFF_SULPHOROUS_FLARE, "STAFF_SULPHOROUS_FLARE",
	SUBCLASS_STAFF_SULPHOROUS_SHOWER, "STAFF_SULPHOROUS_SHOWER",
	SUBCLASS_STAFF_LIGHTNING, "STAFF_LIGHTNING",
	SUBCLASS_STAFF_FIREBALL, "STAFF_FIREBALL",
	SUBCLASS_STAFF_TRIPLE_FIREBALL, "STAFF_TRIPLE_FIREBALL",
	SUBCLASS_STAFF_FORCE_OF_NATURE, "STAFF_FORCE_OF_NATURE",
	SUBCLASS_STAFF_DEATH_RAY, "STAFF_DEATH_RAY",
	SUBCLASS_STAFF_OBLIVION_HALBERD, "STAFF_OBLIVION_HALBERD",
	SUBCLASS_STAFF_OBLIVION_HEART, "STAFF_OBLIVION_HEART",
	SUBCLASS_STAFF_OBLIVION_WIERDLING, "STAFF_OBLIVION_WIERDLING",
	SUBCLASS_STAFF_OBLIVION_ORB, "STAFF_OBLIVION_ORB",
	0, NULL
};

NoxEnumNamePair g_noxFlagNames[] =
{
	FLAG_BELOW, "BELOW",
	FLAG_NO_UPDATE, "NO_UPDATE",
	FLAG_ACTIVE, "ACTIVE",
	FLAG_ALLOW_OVERLAP, "ALLOW_OVERLAP",
	FLAG_SHORT, "SHORT",
	FLAG_DESTROYED, "DESTROYED",
	FLAG_NO_COLLIDE, "NO_COLLIDE",
	FLAG_MISSILE_HIT, "MISSILE_HIT",
	FLAG_EQUIPPED, "EQUIPPED",
	FLAG_PARTITIONED, "PARTITIONED",
	FLAG_NO_COLLIDE_OWNER, "NO_COLLIDE_OWNER",
	FLAG_OWNER_VISIBLE, "OWNER_VISIBLE",
	FLAG_EDIT_VISIBLE, "EDIT_VISIBLE",
	FLAG_NO_PUSH_CHARACTERS, "NO_PUSH_CHARACTERS",
	FLAG_AIRBORNE, "AIRBORNE",
	FLAG_DEAD, "DEAD",
	FLAG_SHADOW, "SHADOW",
	FLAG_FALLING, "FALLING",
	FLAG_IN_HOLE, "IN_HOLE",
	FLAG_RESPAWN, "RESPAWN",
	FLAG_ON_OBJECT, "ON_OBJECT",
	FLAG_SIGHT_DESTROY, "SIGHT_DESTROY",
	FLAG_TRANSIENT, "TRANSIENT",
	FLAG_BOUNCY, "BOUNCY",
	FLAG_ENABLED, "ENABLED",
	FLAG_PENDING, "PENDING",
	FLAG_TRANSLUCENT, "TRANSLUCENT",
	FLAG_STILL, "STILL",
	FLAG_NO_AUTO_DROP, "DROP_AUTO_DROP",
	FLAG_FLICKER, "FLICKER",
	FLAG_SELECTED, "SELECTED",
	FLAG_MARKED, "MARKED",
	0, NULL
};

NoxEnumNamePair g_noxCollideNames[] =
{
	COLLIDE_NONE, "NoCollide",
	COLLIDE_DEFAULT, "DefaultCollide",
	COLLIDE_MONSTER, "MonsterCollide",
	COLLIDE_PLAYER, "PlayerCollide",
	COLLIDE_PROJECTILE, "ProjectileCollide",
	COLLIDE_PROJECTILESPARK, "ProjectileSparkCollide",
	COLLIDE_DOOR, "DoorCollide",
	COLLIDE_PICKUP, "PickupCollide",
	COLLIDE_EXIT, "ExitCollide",
	COLLIDE_DAMAGE, "DamageCollide",
	COLLIDE_MANADRAIN, "ManaDrainCollide",
	COLLIDE_BOMB, "BombCollide",
	COLLIDE_SPARKEXPLOSION, "SparkExplosionCollide",
	COLLIDE_CHEST, "ChestCollide",
	COLLIDE_WALLREFLECT, "WallReflectCollide",
	COLLIDE_WALLREFLECTSPARK, "WallReflectSparkCollide",
	COLLIDE_PIXIE, "PixieCollide",
	COLLIDE_OWN, "OwnCollide",
	COLLIDE_SPARK, "SparkCollide",
	COLLIDE_BARREL, "BarrelCollide",
	COLLIDE_AUDIOEVENT, "AudioEventCollide",
	COLLIDE_TRIGGER, "TriggerCollide",
	COLLIDE_TELEPORT, "TeleportCollide",
	COLLIDE_ELEVATOR, "ElevatorCollide",
	COLLIDE_AWARDSPELL, "AwardSpellCollide",
	COLLIDE_DIE, "DieCollide",
	COLLIDE_GLYPH, "GlyphCollide",
	COLLIDE_SPELLPROJECTILE, "SpellProjectileCollide",
	COLLIDE_BOOM, "BoomCollide",
	COLLIDE_SIGN, "SignCollide",
	COLLIDE_PENTAGRAM, "PentagramCollide",
	COLLIDE_SPIDERSPIT, "SpiderSpitCollide",
	COLLIDE_DEATHBALL, "DeathBallCollide",
	COLLIDE_DEATHBALLFRAGMENT, "DeathBallFragmentCollide",
	COLLIDE_TELEKINESIS, "TelekinesisCollide",
	COLLIDE_FIST, "FistCollide",
	COLLIDE_TELEPORTWAKE, "TeleportWakeCollide",
	COLLIDE_FLAG, "FlagCollide",
	COLLIDE_CHAKRAMINMOTION, "ChakramInMotionCollide",
	COLLIDE_ARROW, "ArrowCollide",
	COLLIDE_MONSTERARROW, "MonsterArrowCollide",
	COLLIDE_BEARTRAP, "BearTrapCollide",
	COLLIDE_POISONGASTRAP, "PoisonGasTrapCollide",
	COLLIDE_TRAPDOOR, "TrapDoorCollide",
	COLLIDE_BALL, "BallCollide",
	COLLIDE_HOMEBASE, "HomeBaseCollide",
	COLLIDE_CROWN, "CrownCollide",
	COLLIDE_UNDEADKILLER, "UndeadKillerCollide",
	COLLIDE_YELLOWSTARSHOT, "YellowStarShotCollide",
	COLLIDE_MIMIC, "MimicCollide",
	COLLIDE_HARPOON, "HarpoonCollide",
	COLLIDE_MONSTERGENERATOR, "MonsterGeneratorCollide",
	COLLIDE_SOULGATE, "SoulGateCollide",
	COLLIDE_ANKH, "AnkhCollide",
	0, NULL
};

NoxEnumNamePair g_noxUpdateNames[] = 
{
	UPDATE_NONE, "NoUpdate",
	UPDATE_PLAYER, "PlayerUpdate",
	UPDATE_PROJECTILE, "ProjectileUpdate",
	UPDATE_HOMINGPROJECTILE, "HomingProjectileUpdate",
	UPDATE_SPELLPROJECTILE, "SpellProjectileUpdate",
	UPDATE_ANTISPELLPROJECTILE, "AntiSpellProjectilUpdate",
	UPDATE_DOOR, "DoorUpdate",
	UPDATE_SPARK, "SparkUpdate",
	UPDATE_PROJECTILETRAIL, "ProjectileTrailUpdate",
	UPDATE_PUSH, "PushUpdate",
	UPDATE_TRIGGER, "TriggerUpdate",
	UPDATE_TOGGLE, "ToggleUpdate",
	UPDATE_MONSTER, "MonsterUpdate",
	UPDATE_LOOPANDDAMAGE, "LoopAndDamageUpdate",
	UPDATE_ELEVATOR, "ElevatorUpdate",
	UPDATE_ELEVATORSHAFT, "ElevatorShaftUpdate",
	UPDATE_PHANTOMPLAYER, "PhantomPlayerUpdate",
	UPDATE_OBELISK, "ObeliskUpdate",
	UPDATE_LIFETIME, "LifetimeUpdate",
	UPDATE_MAGICMISSILE, "MagicMissileUpdate",
	UPDATE_PIXIE, "PixieUpdate",
	UPDATE_SPIKEBLOCK, "SpikeBlockUpdate",
	UPDATE_TOWER, "TowerUpdate",
	UPDATE_SKULL, "SkullUpdate",
	UPDATE_PENTAGRAM, "PentagramUpdate",
	UPDATE_INVISIBLEPENTAGRAM, "InvisiblePentagramUpdate",
	UPDATE_SWITCH, "SwitchUpdate",
	UPDATE_BLOW, "BlowUpdate",
	UPDATE_MOVER, "MoverUpdate",
	UPDATE_BLACKPOWDERBARREL, "BlackPowderBarrelUpdate",
	UPDATE_ONESECONDDIE, "OneSecondDieUpdate",
	UPDATE_WATERBARREL, "WaterBarrelUpdate",
	UPDATE_SELFDESTRUCT, "SelfDestructUpdate",
	UPDATE_BLACKPOWDERBURN, "BlackPowderBurnUpdate",
	UPDATE_DEATHBALL, "DeathBallUpdate",
	UPDATE_DEATHBALLFRAGMENT, "DeathBallFragmentUpdate",
	UPDATE_MOONGLOW, "MoonglowUpdate",
	UPDATE_SENTRYGLOBE, "SentryGlobeUpdate",
	UPDATE_TELEKINESIS, "TelekinesisUpdate",
	UPDATE_FIST, "FistUpdate",
	UPDATE_METEORSHOWER, "MeteorShowerUpdate",
	UPDATE_METEOR, "MeteorUpdate",
	UPDATE_TOXICCLOUD, "ToxicCloudUpdate",
	UPDATE_SMALLTOXICCLOUD, "SmallToxicCloudUpdate",
	UPDATE_ARACHNAPHOBIA, "ArachnaphobiaUpdate",
	UPDATE_EXPIRE, "ExpireUpdate",
	UPDATE_BREAK, "BreakUpdate",
	UPDATE_OPEN, "OpenUpdate",
	UPDATE_BREAKANDREMOVE, "BreakAndRemoveUpdate",
	UPDATE_CHAKRAMINMOTION, "ChakramInMotionUpdate",
	UPDATE_FLAG, "FlagUpdate",
	UPDATE_TRAPDOOR, "TrapDoorUpdate",
	UPDATE_BALL, "BallUpdate",
	UPDATE_CROWN, "CrownUpdate",
	UPDATE_UNDEADKILLER, "UndeadKillerUpdate",
	UPDATE_HARPOON, "HarpoonUpdate",
	UPDATE_WEAPONARMOR, "WeaponArmorUpdate",
	UPDATE_MONSTERGENERATOR, "MonsterGeneratorUpdate",
	0, NULL
};

NoxEnumNamePair g_noxUseNames[] = 
{
	USE_CONSUME, "ConsumeUse",
	USE_CONSUMECONFUSE, "ConsumeConfuseUse",
	USE_FIREWAND, "FireWandUse",
	USE_CAST, "CaseUse",
	USE_ENCHANT, "EnchantUse",
	USE_MUSHROOM, "MushroomUse",
	USE_READ, "ReadUse",
	USE_WARPREAD, "WarpReadUse",
	USE_WAND, "WandUse",
	USE_WANDCAST, "WandCastUse",
	USE_SPELLREWARD, "SpellRewardUse",
	USE_ABILITYREWARD, "AbilityRewardUse",
	USE_FIELDGUIDE, "FieldGuideUse",
	USE_POTION, "PotionUse",
	USE_AMMO, "AmmoUse",
	USE_BOW, "BowUse",
	0, NULL
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int ThingBin::Parse_String(char str[],int strLen)
{

	int aniType=0;

//CString str2=str;

if(strstr(str,"Update")!=NULL || strstr(str,"UPDATE")!=NULL)
//if(str2.Find("Update")!=(-1) || str2.Find("UPDATE")!=(-1))
{return(0);}

        if(
			!strcmp(str,"StaticDraw") ||
			!strcmp(str,"ArmorDraw")  ||
			!strcmp(str,"WeaponDraw") ||
		    !strcmp(str,"SlaveDraw")  ||		
			!strcmp(str,"BaseDraw")  
			)
		{aniType=1;}
		 else if(!strcmp(str,"PlayerDraw"))
		 {aniType=10;}
        else if(
			!strcmp(str,"AnimateDraw")         ||
			!strcmp(str,"SphericalShieldDraw") ||
			!strcmp(str,"WeaponAnimateDraw")   ||
		    !strcmp(str,"FlagDraw")            ||		
			!strcmp(str,"SummonEffectDraw")    ||
			!strcmp(str,"ReleasedSoulDraw")    ||
			!strcmp(str,"GlyphDraw")
			)
		{aniType=2;}
        else if(
			!strcmp(str,"AnimateStateDraw") ||
			!strcmp(str,"MonsterDraw")      ||
          //  !strcmp(str,"PlayerDraw")       ||
		    !strcmp(str,"MaidenDraw")			
			)
		{aniType=3;}
        else if(
			!strcmp(str,"BoulderDraw")       ||
			!strcmp(str,"StaticRandomDraw")  ||
			!strcmp(str,"DoorDraw")          ||
		    !strcmp(str,"ArrowDraw")         ||		
			!strcmp(str,"HarpoonDraw")       ||
			!strcmp(str,"WeakArrowDraw")
			)
		{aniType=4;}
		else if(!strcmp(str,"VectorAnimateDraw"))
		{aniType=5;}
		else if(!strcmp(str,"ConditionalAnimateDraw") || !strcmp(str,"MonsterGeneratorDraw"))
		{aniType=6;}
		else if(!strcmp(str,"MENUICON"))
		{aniType=7;}
		else if(!strcmp(str,"PRETTYIMAGE"))
		{aniType=8;}
        else if(strstr(str,"Draw")!=NULL)
		{aniType=9;}
		else
		{aniType=0;}


return(aniType);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool ThingBin::Load_Thingdb(fstream *file)
{

long count=0;
file->seekg(0,ios::end);
long len = file->tellg();
file->seekg(0,ios::beg);
unsigned char *buff;
//long count=0;


buff = new unsigned char [len+10];
memset(buff,0x00,len+10);

file->read((char*)buff,len);
NoxCrypt::decrypt(buff, len, NC_THING_BIN);

memset(Type,0x00,5);
memcpy((void*)&*(Type),(const void*)&*(buff+count),sizeof(int));

Load_Tiles(buff,count);
Load_Walls(buff,count, Thing.Wall);
Load_Sounds(buff,count);

while(strcmp(Type,"GAMI") && count<len)
	  {
	        memset(Type,0x00,5);
            memcpy((void*)&*(Type),(const void*)&*(buff+count),sizeof(int));
            count++;
		  }
count+=3;
Load_Images(buff,count,Thing.Image);

while(strcmp(Type,"GNHT"))
	  {
 	        memset(Type,0x00,5);          
	        memcpy((void*)&*(Type),(const void*)&*(buff+count),sizeof(int));
            count++;
		  }

Load_Objects(buff,count,len, Thing.Object);

delete [] buff;
Is_Loaded=true;
return(true);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool ThingBin::Load_Images(unsigned char *buff,long &count, GAMISTRUCT & Images)
{

GAMI Image;

int numImages = 0;
memcpy((void*)&numImages,(const void*)&*(buff+count),sizeof(int));
count+=4;

int pos=0;
//numImages=20; //NEED TO MAKE THIS A REAL VALUE BY ADDING ANIMATION SUPPORT!!!

      while(pos<numImages) 
	  {
           Load_Image(count,buff,Image);
		   Images.Images.Add(Image);
		   memset((void*)&Image, 0x00,sizeof(GAMI));

		   pos++;
		  }
            memcpy((void*)&*(Type),(const void*)&*(buff+count),sizeof(int));
            count+=4;


return(true);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool ThingBin::Load_Walls(unsigned char *buff,long &count, LLAWSTRUCT & Walls)
{
LLAW Wall;
      while(!strcmp(Type,"LLAW"))
	  {
		   
	       memset((void*)&Wall, 0x00,sizeof(LLAW));
           Load_Wall(count,buff,Wall);
	       Thing.Wall.Walls.Add(Wall);
		   memcpy((void*)&*(Type),(const void*)&*(buff+count),sizeof(int));
           count+=4;
	  }
count -= 4;
memset((void*)&Wall, 0x00,sizeof(LLAW));

return(true);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool ThingBin::Load_Tiles(unsigned char *buff,long &count)
{
   ROLF Tile;
   EGDE Edge;
   memset(Type,0x00,5);
   memcpy((void*)&*(Type),(const void*)&*(buff+count),sizeof(int));
   count+=4;
      while(!strcmp(Type,"ROLF") || !strcmp(Type,"EGDE"))
	  {
		   if(!strcmp(Type,"ROLF"))
		   {
			Load_Tile(count,buff,Tile);
			Thing.Tile.Tiles.Add(Tile);
			memset((void*)&Tile, 0x00,sizeof(ROLF));
		   }
		   else
		   {
			Load_Edge(count,buff,Edge);
			Thing.Edge.Edges.Add(Edge);
			memset((void*)&Edge, 0x00,sizeof(EGDE));
		   }
             memcpy((void*)&*(Type),(const void*)&*(buff+count),sizeof(int));
             count+=4;
	  }


return(true);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool ThingBin::Load_Sounds(unsigned char *buff,long &count)
{
   DUA Aud;
   memset(Type,0x00,5);
   memcpy((void*)&*(Type),(const void*)&*(buff+count),sizeof(int));
   count+=4;
      if(!strcmp(Type," DUA"))
	  {
		  int len = *((uint32*)(buff+count));
		  count += 4;
		  for(; len > 0; len--)
		  {
           Load_Aud(count,buff,Aud);
		   Thing.Sounds.Sounds.Add(Aud);
		  }
	  }
return(true);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool ThingBin::Load_Objects(unsigned char *buff,long &count,long filelen,GNHTSTRUCT & Data)
{

      GNHT Object;
      while(!strcmp(Type,"GNHT"))  
       {
	       memset((void*)&Object, 0x00,sizeof(GNHT));
           Load_Object(count,buff,filelen,Object);
		   Thing.Object.Objects.Add(Object);
           memset((void*)&Object, 0x00,sizeof(GNHT));

		  while(strcmp(Type,"GNHT") && count<filelen)
		  {
             memcpy((void*)&*(Type),(const void*)&*(buff+count),sizeof(int));
             count++;
		  }
	   }
return(true);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
uint32 g_noxObjectCtr = 0;
bool ThingBin::Load_Object(long &count,unsigned char *buff,long filelen, GNHT & Object)
{

  int aniType		=0;
  int strLen		=0;
  char str[255];
  int Length32		=0;
  long Length64		=0;
  long finishPos	=0;
  int k=0,j=0;
  int subcount=0;
  TATS Stat;
  memset((void*)&Stat,0x00,sizeof(TATS));
  Property Prop;
  memset((void*)&Prop,0x00,sizeof(Property));
  BYTE numStats=0;

memcpy((void*)&Object.nameLen,(const void*)&*(buff+count),sizeof(BYTE));
count++;

memset(Object.Name,0x00,50);
memcpy((void*)&Object.Name,(const void*)&*(buff+count),Object.nameLen);
count+=Object.nameLen;

g_noxObjectCtr++;
sWorldLog.Log("%s: 0x%.4X\n", &Object.Name, g_noxObjectCtr);
//LOAD EVERYTHING ELSE;;;;


memset(Type,0x00,4);
while(strcmp(Type,"GNHT") && count<filelen)
	  {
       
        memcpy((void*)&strLen,(const void*)&*(buff+count),sizeof(char));
        count++;

		memset(str,0x00,255);
        memcpy((void*)&str,(const void*)&*(buff+count),strLen);
        count+=strLen;

        aniType=Parse_String(str,strLen);
		Property propString;

		if(aniType==0 && strLen > 0)
		{
          
		  memset((void*)&propString,0x00,sizeof(Property));
		  memcpy((void*)&propString.Value,(const void*)&str,255);

		  // lazy warning! tables might be better
		  char* string = strtok(propString.Value, " +");
		  if(!strcmp(string, "CLASS")) //move the below to a separate function
		  {
			  strtok(NULL, " +"); // =
			  while( (string = strtok(NULL, " +")) != NULL)
				  Object.classes |= noxNameToEnum(string, g_noxClassNames);
		  }
		  else if(!strcmp(string, "FLAGS"))
		  {
			  strtok(NULL, " +"); // =
			  while( (string = strtok(NULL, " +")) != NULL)
				  Object.flags |= noxNameToEnum(string, g_noxFlagNames);
		  }
		  else if(!strcmp(string, "SUBCLASS"))
		  {
			  strtok(NULL, " +"); // =
			  while( (string = strtok(NULL, " +")) != NULL)
				  Object.subclass |= noxNameToEnum(string, g_noxSubclassNames);
		  }
		  else if(!strcmp(string, "COLLIDE"))
		  {
			  strtok(NULL, " +"); // =
			  if( (string = strtok(NULL, " +")) != NULL)
			  {
				  Object.collide = noxNameToEnum(string, g_noxCollideNames);
				  for(int i = 0; (string = strtok(NULL, " +")) != NULL; i++)
				       strncpy(Object.collide_args[i], string, 30);
			  }
		  }
		  else if(!strcmp(string, "UPDATE"))
		  {
			  strtok(NULL, " +"); // =
			  if( (string = strtok(NULL, " +")) != NULL)
			  {
				  Object.update = noxNameToEnum(string, g_noxUpdateNames);
				  for(int i = 0; (string = strtok(NULL, " +")) != NULL; i++)
				       strncpy(Object.update_args[i], string, 30);
			  }
		  }
		  else if(!strcmp(string, "USE"))
		  {
			  strtok(NULL, " +"); // =
			  if( (string = strtok(NULL, " +")) != NULL)
			  {
				  Object.use = noxNameToEnum(string, g_noxUseNames);
				  for(int i = 0; (string = strtok(NULL, " +")) != NULL; i++)
				       strncpy(Object.use_args[i], string, 30);
			  }
		  }
		  else if(!strcmp(string, "EXTENT"))
		  {
			  strtok(NULL, " +"); // =
			  string = strtok(NULL, " +");
			  if(!strcmp(string, "BOX"))
			  {
				  Object.extent.shape = EXTENT_BOX;
				  string = strtok(NULL, " +");
				  if(string)
				  {
					Object.extent.x = atoi(string);
					string = strtok(NULL, " +");
					if(string)
						Object.extent.y = atoi(string);
				  }
			  }
			  else if(!strcmp(string, "CIRCLE"))
			  {
				  Object.extent.shape = EXTENT_CIRCLE;
				  string = strtok(NULL, " +");
				  if(string)
				  {
					Object.extent.radius = atoi(string);
				  }
			  }
		  }

		  Object.Properties.Add(propString);
          memset((void*)&propString,0x00,sizeof(Property));
		  //Object.numStrings++;
		}


//ALL FINISH POS OPERATIONS CAN HAVE *ANI = new [section size/4];
//WILL OVER BUFFER, but should be within decent results.

        switch(aniType)
		{
            case 0:break;

            case 1: //return(Object);
				count+=((8 - count % 8) % 8);
				memcpy((void*)&Length64,(const void*)&*(buff+count),sizeof(Length64));
                count+=(8);
				//count+=Length64;
                finishPos=count+Length64;

				if(!strcmp(str,"SlaveDraw"))
				{
                memcpy((void*)&Length32,(const void*)&*(buff+count),sizeof(BYTE));
                count+=sizeof(BYTE);
				while((Length32--)!=0)
				{
				    Object.ImageCode=0;
					//memcpy((void*)&Object.aniList[Object.aniNum],(const void*)&*(buff+count),sizeof(int));
 				    memcpy((void*)&Object.ImageCode,(const void*)&*(buff+count),sizeof(int));
				//	Object.Stat[0].Loop.Images[Object.Stat[0].Loop.numImages]=Object.ImageCode;
				//	Object.Stat[0].Loop.numImages++;                  
					///////////////////////////////////////////////////////////
					Stat.Images.Add(Object.ImageCode);
					//Object.Stat[0].Loop.List.Add(Object.ImageCode);
					////////////////////////////////////////////////////////////
					
					count+=sizeof(int);
					//Object.aniNum++;
					}
				}
				else
				{  
					Object.ImageCode=0;
				//	memcpy((void*)&Object.aniList[Object.aniNum],(const void*)&*(buff+count),sizeof(int));
					memcpy((void*)&Object.ImageCode,(const void*)&*(buff+count),sizeof(int));
				//	Object.Stat[0].Loop.Images[Object.Stat[0].Loop.numImages]=Object.ImageCode;
				//	Object.Stat[0].Loop.numImages++;                 
					////////////////////////////////////////////////////////////
					Stat.Images.Add(Object.ImageCode);
					//Object.Stat[0].Loop.List.Add(Object.ImageCode);
					////////////////////////////////////////////////////////////
					
					count+=sizeof(int);
				//	Object.aniNum++;
				}
                count=finishPos;
				Object.Stats.Add(Stat);
				memcpy((void*)&Object.aniName,(const void*)&str,50);
				break;

            case 2:// return(Object);
                
                count+=((8 - count % 8) % 8);//SkipToNextBoundary
				memcpy((void*)&Length64,(const void*)&*(buff+count),sizeof(Length64));
                count+=8;
				finishPos = Length64 + count;
                count+=2;
      
				memcpy((void*)&Length32,(const void*)&*(buff+count),sizeof(BYTE));
                count+=sizeof(BYTE);				

				memcpy((void*)&Object.aniName,(const void*)&*(buff+count),Length32);
                count+=Length32;


				strcpy((char*)Stat.loopName,Object.aniName);
				//Object.numStats=1;
				

						while (count < finishPos)
						{
							
						if(count+4>finishPos)
						{count=finishPos;} //DO NOT READ THIS ANIMATION!
						else
						{
			            	//memcpy((void*)&Object.aniList[Object.aniNum],(const void*)&*(buff+count),sizeof(int));
 			            	//Object.aniNum++;
							Object.ImageCode=0;
							memcpy((void*)&Object.ImageCode,(const void*)&*(buff+count),sizeof(int));
					//		Object.Stat[0].Loop.Images[Object.Stat[0].Loop.numImages]=Object.ImageCode;
					//		Object.Stat[0].Loop.numImages++;
					////////////////////////////////////////////////////////////
							Stat.Images.Add(Object.ImageCode);
							//Object.Stat[0].Loop.List.Add(Object.ImageCode);
					////////////////////////////////////////////////////////////							
							count+=sizeof(int);				
							//Object.Stat[0].Loop.numImages++;		
							}
						}
			    count=finishPos;
	
				memcpy((void*)&Object.aniName,(const void*)&str,50);
				Object.Stats.Add(Stat);
				break;

			case 10: //PLAYERDRAW

           
			    Length64=0;
				count+=((8 - count % 8) % 8);//SkipToNextBoundary
				memcpy((void*)&Length64,(const void*)&*(buff+count),sizeof(Length64));
                count+=8;
				finishPos=count+Length64;
			//	count=finishPos;
				// Images not important for server, only that it works

                while(count<finishPos)
				{
					memset((void*)&Stat,0x00,sizeof(TATS));
					memcpy((void*)&Type,(const void*)&*(buff+count),sizeof(int));
					count+=4;
					if(!strcmp(Type," DNE"))
					{count=finishPos;}
					else
					{
                        if(!strcmp(Type,"UQES"))
						{
						memset(Object.aniName,0x00,50);
						Length32=0;
                        memcpy((void*)&Length32,(const void*)&*(buff+count),sizeof(BYTE));
                        count++;
						memcpy((void*)&Object.aniName,(const void*)&*(buff+count),Length32);
						count+=Length32;
						strcpy((char*)Stat.loopName,Object.aniName);
						///////////////Object.PlayerStats.Get(Object.numPlayerStats)->loopName=Object.aniName;
							}
						else
						{

						memset(Object.aniName,0x00,50);
						//count++;
						Length32=0;
                        memcpy((void*)&Length32,(const void*)&*(buff+count),sizeof(BYTE));
                        count++;
						memcpy((void*)&Object.aniName,(const void*)&*(buff+count),Length32);
						count+=Length32;
						strcpy((char*)Stat.nullString,Object.aniName);
						strcpy((char*)Prop.Value,Object.aniName);
						Prop.ValueLen = strlen(Object.aniName);
						Object.Subcats.Add(Prop);
						memset((void*)&Prop,0x00,sizeof(Property));
                        //////Object.PlayerStat[Object.numPlayerStats].nullString=Object.aniName;
                        //////Object.SubCats[Object.numSubCats]=Object.aniName;
						count+=2;
						 

						memset(Object.aniName,0x00,50);
						Length32=0;
                        memcpy((void*)&Length32,(const void*)&*(buff+count),sizeof(BYTE));
                        count++;
						memcpy((void*)&Object.aniName,(const void*)&*(buff+count),Length32);
						count+=Length32;
						strcpy((char*)Stat.loopName,Object.aniName);
                        //Object.PlayerStat[Object.numPlayerStats].loopName=Object.aniName;						
                        			
						memcpy((void*)&Type,(const void*)&*(buff+count),sizeof(int));
						if(!strcmp(Type,"UQES"))
						{
						  count+=4;
						
						  memset(Object.aniName,0x00,50);
						  Length32=0;
                          memcpy((void*)&Length32,(const void*)&*(buff+count),sizeof(BYTE));
                          count++;
						  memcpy((void*)&Object.aniName,(const void*)&*(buff+count),Length32);
						  count+=Length32;
						  strcpy((char*)Stat.loopName,Object.aniName);
						  //Object.PlayerStat[Object.numPlayerStats].loopName=Object.aniName;					
						
						}
						//Object.numSubCats++;
							}

                        memcpy((void*)&Type,(const void*)&*(buff+count),sizeof(int));
						subcount=0;
						while(strcmp(Type," DNE") && strcmp(Type,"TATS") && strcmp(Type,"UQES"))
						{
						  //Object.Stat[Object.numStats].Loop.Images[Object.Stat[Object.numStats].Loop.numImages]=0;
                          Object.ImageCode=0;
						  memcpy((void*)&Object.ImageCode,(const void*)&*(buff+count),sizeof(int));							
						  count+=4;
						  //Object.Stat[Object.numStats].Loop.numImages++;
						  Stat.Images.Add(Object.ImageCode);
						  Stat.SubCat = Object.Subcats.numNodes;
						  //Object.PlayerStat[Object.numPlayerStats].Loop.List.Add(Object.ImageCode);
						  //Object.PlayerStat[Object.numPlayerStats].SubCat=Object.numSubCats;
						  memcpy((void*)&Type,(const void*)&*(buff+count),sizeof(int));
							}
						
						//Object.numPlayerStats++;
						Object.PlayerStats.Add(Stat);
                        memset((void*)&Stat,0x00,sizeof(TATS));
							}
					}
	

				memcpy((void*)&Object.aniName,(const void*)&str,50);
				count=finishPos;

				memcpy((void*)&Type,(const void*)&*(buff+count),sizeof(int));
				while(strcmp(Type," DNE"))
				{
					memcpy((void*)&Type,(const void*)&*(buff+count),sizeof(int));
					count++;
					}
				count+=3;
				count=finishPos;
				break;



            case 3:// return(Object);
				//
				// THIS IS THE JACK DRAWING!!!.....BE CAREFULL!
				//
			    Length64=0;
				count+=((8 - count % 8) % 8);//SkipToNextBoundary
				memcpy((void*)&Length64,(const void*)&*(buff+count),sizeof(Length64));
                count+=8;
				finishPos=count+Length64;

				if(/*!strcmp(str,"PlayerDraw") && */strcmp(str,"MonsterDraw") && strcmp(str,"MaidenDraw") && strcmp(str,"AnimateDraw"))
				{
					count=finishPos;
					break;						
				}

                while(count<finishPos)
				{
					memcpy((void*)&Type,(const void*)&*(buff+count),sizeof(int));
					count+=4;
					if(!strcmp(Type," DNE"))
					{count=finishPos;}
					else
					{
						memset(Object.aniName,0x00,20);
						count++;
						Length32=0;
                        memcpy((void*)&Length32,(const void*)&*(buff+count),sizeof(BYTE));
                        count++;
						memcpy((void*)&Object.aniName,(const void*)&*(buff+count),Length32);
						count+=Length32;
						strcpy((char*)Stat.nullString,Object.aniName);
                        //Object.Stat[Object.numStats].nullString=Object.aniName;
                         count+=4;

						memset(Object.aniName,0x00,20);
						Length32=0;
                        memcpy((void*)&Length32,(const void*)&*(buff+count),sizeof(BYTE));
                        count++;
						memcpy((void*)&Object.aniName,(const void*)&*(buff+count),Length32);
						count+=Length32;
						strcpy((char*)Stat.loopName,Object.aniName);
						//Object.Stat[Object.numStats].loopName=Object.aniName;						

                        memcpy((void*)&Type,(const void*)&*(buff+count),sizeof(int));
						subcount=0;
						while(strcmp(Type," DNE") && strcmp(Type,"TATS"))
						{
						  //Object.Stat[Object.numStats].Loop.Images[Object.Stat[Object.numStats].Loop.numImages]=0;
                          Object.ImageCode=0;
						  memcpy((void*)&Object.ImageCode,(const void*)&*(buff+count),sizeof(int));							
						  count+=4;
						  //Object.Stat[Object.numStats].Loop.numImages++;
						  Stat.Images.Add(Object.ImageCode);
						  //Object.Stat[Object.numStats].Loop.List.Add(Object.ImageCode);
						  memcpy((void*)&Type,(const void*)&*(buff+count),sizeof(int));					
							}
						Object.Stats.Add(Stat);
                        memset((void*)&Stat,0x00,sizeof(TATS));
						//Object.numStats++;
							}
					}
	
				memcpy((void*)&Object.aniName,(const void*)&str,50);
				memcpy((void*)&Type,(const void*)&*(buff+count),sizeof(int));
				while(strcmp(Type," DNE"))
				{
					memcpy((void*)&Type,(const void*)&*(buff+count),sizeof(int));
					count++;
				}
				count+=3;
				count=finishPos;
				break;

            case 4: //return(Object);
				count+=((8 - count % 8) % 8);//SkipToNextBoundary
				
				memcpy((void*)&Length64,(const void*)&*(buff+count),sizeof(Length64));
                count+=8;
				finishPos = Length64 + count;
				
				BYTE numFrames;
				memcpy((void*)&numFrames,(const void*)&*(buff+count),sizeof(BYTE));
                count+=sizeof(BYTE);
				
				//Object.numStats=1;
				while (numFrames-- > 0)
				{//rdr.ReadInt32();
				
				   memcpy((void*)&Object.ImageCode,(const void*)&*(buff+count),sizeof(int));		   
				  // Object.Stat[0].Loop.Images[Object.Stat[0].Loop.numImages]=Object.ImageCode;
				  // Object.Stat[0].Loop.numImages++;					
					////////////////////////////////////////////////////////////
				   Stat.Images.Add(Object.ImageCode); 
				   //Object.Stat[0].Loop.List.Add(Object.ImageCode);
					////////////////////////////////////////////////////////////
				   //memcpy((void*)&Object.aniList[Object.aniNum],(const void*)&*(buff+count),sizeof(int));
                count+=sizeof(int);
				//Object.aniNum++;
					}//count+=4;
				Object.Stats.Add(Stat);
                memset((void*)&Stat,0x00,sizeof(TATS));
				memcpy((void*)&Object.aniName,(const void*)&str,50);
				count = finishPos;
				break;

            case 5: //return(Object);
				count+=((8 - count % 8) % 8);//SkipToNextBoundary
				memcpy((void*)&Length64,(const void*)&*(buff+count),sizeof(Length64));
                count+=8;
				finishPos = Length64 + count;
                count+=2;
      
				memcpy((void*)&Length32,(const void*)&*(buff+count),sizeof(BYTE));
                count+=sizeof(BYTE);				

				memcpy((void*)&Object.aniName,(const void*)&*(buff+count),Length32);
                count+=Length32;
	
				strcpy((char*)Stat.loopName,Object.aniName);
                //Object.Stat[0].loopName=Object.aniName;
                //Object.numStats=1;

						//FIXME: this may be a Loop of Loops and should probably be constructed as such
						//HACK: right now we just read until we reach the given length, tacking on the frames to the existing ones
						while (count < finishPos)
						{

						if(count+4>finishPos)
						{count=finishPos;} //DO NOT READ THIS ANIMATION!
						else
						{
							Object.ImageCode=0;
							memcpy((void*)&Object.ImageCode,(const void*)&*(buff+count),sizeof(int));
						//    Object.Stat[0].Loop.Images[Object.Stat[0].Loop.numImages]=Object.ImageCode;
							
						//	Object.Stat[0].Loop.numImages++;
					  ////////////////////////////////////////////////////////////
							Stat.Images.Add(Object.ImageCode);
							//Object.Stat[0].Loop.List.Add(Object.ImageCode);
					  ////////////////////////////////////////////////////////////
							//	memcpy((void*)&Object.aniList[Object.aniNum],(const void*)&*(buff+count),sizeof(int));
                            count+=sizeof(int);
			             //	Object.aniNum++;							
							
							}
						}
			    count=finishPos;
				Object.Stats.Add(Stat);
                memset((void*)&Stat,0x00,sizeof(TATS));
				memcpy((void*)&Object.aniName,(const void*)&str,50);
				break;

            case 6: //return(Object);
				count+=((8 - count % 8) % 8);//SkipToNextBoundary
				Length64=0;
				memcpy((void*)&Length64,(const void*)&*(buff+count),sizeof(Length64));
                count+=8;
				finishPos = Length64 + count;

				//BYTE numAni;
			    numStats=0;
				memcpy((void*)&numStats,(const void*)&*(buff+count),sizeof(BYTE));
                count+=sizeof(BYTE);


				for(j=0; j<numStats; j++)
				{
					memset((void*)&Stat,0x00,sizeof(TATS));
					Length64=0;
					Length32=0;
				    memcpy((void*)&Length64,(const void*)&*(buff+count),sizeof(BYTE));//Num Images
                    count+=sizeof(BYTE);
				    count++;
				    
					memcpy((void*)&Length32,(const void*)&*(buff+count),sizeof(BYTE));
                    count+=sizeof(BYTE);

					memset(Object.aniName,0x00,70);
                    memcpy((void*)&Object.aniName,(const void*)&*(buff+count),Length32);
					strcpy((char*)Stat.loopName,Object.aniName);
					//Object.Stat[j].loopName=Object.aniName;
					count+=Length32;

//                    Object.Stat[j].Loop.numImages=0;
					for(k=0; k<Length64; k++)
					{
                      Object.ImageCode=0;
					  memcpy((void*)&Object.ImageCode,(const void*)&*(buff+count),sizeof(int));
                      count+=sizeof(int);
					 // Object.Stat[j].Loop.numImages++;					  
					////////////////////////////////////////////////////////////
                    //Object.Stat[j].Loop.Images[k]=Object.ImageCode;
					  Stat.Images.Add(Object.ImageCode);
					  //Object.Stat[j].Loop.List.Add(Object.ImageCode);
					////////////////////////////////////////////////////////////
			
					}
					Object.Stats.Add(Stat);
                    memset((void*)&Stat,0x00,sizeof(TATS));
				}		
						count=finishPos;
					
				 memcpy((void*)&Object.aniName,(const void*)&str,50);
				 break;

            case 7: //return(Object);
				memcpy((void*)&Object.MenuIcon,(const void*)&*(buff+count),sizeof(int));
                count+=sizeof(int);				
				break;

            case 8: //return(Object);
				memcpy((void*)&Object.PrettyImage,(const void*)&*(buff+count),sizeof(int));
                count+=sizeof(int);					
				break;

            case 9: //return(Object);

				count+=((8 - count % 8) % 8);//SkipToNextBoundary
				count+=8;
				break;
            
			default :break;
		}

	   if(*(buff+count)==0x00)
				{
					count++;
				}
	   memcpy((void*)&*(Type),(const void*)&*(buff+count),sizeof(int));
       }

count+=4;

//////////////////////////////end remove
 memset((void*)&Stat,0x00,sizeof(TATS));
 memset((void*)&Prop,0x00,sizeof(Property));
return(true);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


bool ThingBin::Load_Wall(long &count,unsigned char *buff, LLAW & Wall)
{
	long numObjects = 0;
	int Len= 0;
    Property Object;
	memset((void*)&Object,0x00,sizeof(Property));

Wall.Unknown = (int)*(buff+count);
count+=4;

Wall.nameLen = (BYTE)*(buff+count);
count++;

memset(Wall.Name,0x00,25);
memcpy((void*)&Wall.Name,(const void*)&*(buff+count),Wall.nameLen);
count+=Wall.nameLen;

count+=14;
count+=((8 - count % 8) % 8);

numObjects = (long)*(buff+count);
count+=sizeof(long);
count+=((8 - count % 8) % 8);

for(int i=0; i<numObjects; i++)
{
Object.ValueLen = NULL;
Object.ValueLen = (BYTE)*(buff+count);
count++;

memset(Object.Value,0x00,50);
memcpy((void*)&Object.Value,(const void*)&*(buff+count),Object.ValueLen);
count+=Object.ValueLen;
Wall.Objects.Add(Object);
memset((void*)&Object,0x00,sizeof(Property));
}

Wall.Sound_Open_Len=0;
memset(Wall.SoundOpen,0x00,25);
Wall.Sound_Open_Len = (BYTE)*(buff+count);
count++;
memcpy((void*)&Wall.SoundOpen,(const void*)&*(buff+count),Wall.Sound_Open_Len);
count+=Wall.Sound_Open_Len;

memset(Wall.SoundClose,0x00,25);
Wall.Sound_Close_Len=0;
Wall.Sound_Close_Len = (BYTE)*(buff+count);
count++;
memcpy((void*)&Wall.SoundClose,(const void*)&*(buff+count),Wall.Sound_Close_Len);
count+=Wall.Sound_Close_Len;

memset(Wall.SoundDestroy,0x00,25);
Wall.Sound_Destroy_Len = (BYTE)*(buff+count);
count++;
memcpy((void*)&Wall.SoundDestroy,(const void*)&*(buff+count),Wall.Sound_Destroy_Len);
count+=Wall.Sound_Destroy_Len;


//MOVE TO IMAGE BOUNDRY!!!!
count+=((8 - count % 8) % 8);
while((int)*(buff+count)!=0x00)
{
count++;
count+=((8 - count % 8) % 8);
}
count++;
count+=((8 - count % 8) % 8);


short tem;//=0;

while(strcmp(Type," DNE"))
{
    count+=4;
    Wall.Images.Add((unsigned int &)Type);

    memcpy((void*)&*(Type),(const void*)&*(buff+count),sizeof(int));
    if(strcmp(Type," DNE"))
    {
       tem = (short)*(buff+count);
	   count+=2;
       if(tem>0)
       {count+=14;}
       else
       {count+=6;}
    }

memcpy((void*)&*(Type),(const void*)&*(buff+count),sizeof(int));
}
count+=4;
return(true);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//LOAD THE EDGES AND TILES!!!////////////////


bool ThingBin::Load_Edge(long &count,unsigned char *buff, EGDE & Edge)
{

unsigned int ImageCode = 0;


memcpy((void*)&Edge.Unknown,(const void*)&*(buff+count),sizeof(int));
count+=4;

memcpy((void*)&Edge.nameLen,(const void*)&*(buff+count),sizeof(BYTE));
count++;

memset(Edge.Name,0x00,25);
memcpy((void*)&Edge.Name,(const void*)&*(buff+count),Edge.nameLen);
count+=Edge.nameLen;

memcpy((void*)&*(Edge.Unknowns),(const void*)&*(buff+count),14);
count+=14;

while(true)
{
memcpy((void*)&*(Type),(const void*)&*(buff+count)/*[i]*/,sizeof(int));
if(!strcmp(Type," DNE"))
{count+=4;break;}
else
{
     ImageCode=0;
     memcpy((void*)&ImageCode,(const void*)&*(buff+count),sizeof(int));
	 Edge.Images.Add(ImageCode);
count+=4;
}

	}

return(true);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//LOAD THE EDGES AND TILES!!!////////////////


bool ThingBin::Load_Tile(long &count,unsigned char *buff, ROLF & Tile)
{

unsigned int ImageCode = 0;


memcpy((void*)&Tile.Unknown,(const void*)&*(buff+count),sizeof(int));
count+=4;

memcpy((void*)&Tile.nameLen,(const void*)&*(buff+count),sizeof(BYTE));
count++;

memset(Tile.Name,0x00,25);
memcpy((void*)&Tile.Name,(const void*)&*(buff+count),Tile.nameLen);
count+=Tile.nameLen;

memcpy((void*)&*(Tile.Unknowns),(const void*)&*(buff+count),14);
count+=14;

memcpy((void*)&Tile.one,(const void*)&*(buff+count),2);
count+=2;


while(true)
{
memcpy((void*)&*(Type),(const void*)&*(buff+count)/*[i]*/,sizeof(int));
count+=4;
if(!strcmp(Type," DNE"))
{break;}
Tile.Images.Add((unsigned int &)Type);
}

return(true);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool ThingBin::Load_Avnt(long &count,unsigned char *buff, TNVA & Avnt)
{
return(false);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool ThingBin::Load_Aud(long &count,unsigned char *buff, DUA & Aud)
{
memcpy((void*)&Aud.nameLen,(const void*)&*(buff+count),sizeof(BYTE));
count++;

memset(Aud.Name,0x00,25);
memcpy((void*)&Aud.Name,(const void*)&*(buff+count),Aud.nameLen);
count+=Aud.nameLen;

memcpy((void*)&*(Aud.Unknowns),(const void*)&*(buff+count),9);
count+=9;

Property prop;
while( *(buff+count) != NULL )
{
	memcpy((void*)&prop.ValueLen,(const void*)&*(buff+count),sizeof(BYTE));
    count++;

	memset(prop.Value,0x00,25);
	memcpy((void*)&prop.Value,(const void*)&*(buff+count),prop.ValueLen);
	count+=prop.ValueLen;
	Aud.Sounds.Add(prop);
}
count++;

return(true);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool ThingBin::Load_Image(long &count,unsigned char *buff, GAMI & Image)
{

  int numLoop=0,temp=0;
  unsigned int ImageCode = 0;
  BYTE numImages = 0;

memcpy((void*)&Image.nameLen,(const void*)&*(buff+count),sizeof(BYTE));
count++;

memset(Image.Name,0x00,50);
memcpy((void*)&Image.Name,(const void*)&*(buff+count),Image.nameLen);
count+=Image.nameLen;

memcpy((void*)&numImages,(const void*)&*(buff+count),sizeof(BYTE));
count++;

if(numImages == 1)
{	
memcpy((void*)&ImageCode,(const void*)&*(buff+count),sizeof(int));
count+=4;
Image.Images.Add((unsigned int &)ImageCode);
strcpy((char*)Image.aniName,"Static Image");
	}
else
{
memcpy((void*)&numLoop,(const void*)&*(buff+count),sizeof(BYTE));
count+=2;

memcpy((void*)&temp,(const void*)&*(buff+count),sizeof(BYTE));
count++;

memset(Image.aniName,0x00,50);
memcpy((void*)&Image.aniName,(const void*)&*(buff+count),temp);
count+=temp;

for(int i=0; i<numLoop; i++)
{
memcpy((void*)&ImageCode,(const void*)&*(buff+count),sizeof(int));
count+=4;	
Image.Images.Add(ImageCode);
//Image.numImages++;	
}


}

return(true);
}

ThingBin::ThingBin()
{
	Is_Loaded = false;
}

