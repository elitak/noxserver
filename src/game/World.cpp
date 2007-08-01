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
    \ingroup world
*/

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "Config/ConfigEnv.h"
#include "Config/NXconfig.h"
#include "Config/GameConfig.h"
#include "SystemConfig.h"
#include "Log.h"
#include "Opcodes.h"
#include "WorldSession.h"
#include "WorldPacket.h"
#include "World.h"
#include "ObjectAccessor.cpp"
#include "ObjectMgr.h"
#include "Database/DBCStores.h"
#include "Policies/SingletonImp.h"
#include "modifierbin.h"
#include "monsterbin.h"

INSTANTIATE_SINGLETON_1( World );

volatile bool World::m_stopEvent = false;

// ServerMessages.dbc
enum ServerMessageType
{
    SERVER_MSG_SHUTDOWN_TIME      = 1,
    SERVER_MSG_RESTART_TIME       = 2,
    SERVER_MSG_STRING             = 3,
    SERVER_MSG_SHUTDOWN_CANCELLED = 4,
    SERVER_MSG_RESTART_CANCELLED  = 5
};


#define SCRIPT_COMMAND_SAY          0
#define SCRIPT_COMMAND_EMOTE        1
#define SCRIPT_COMMAND_FIELD_SET    2
#define SCRIPT_COMMAND_MOVE_TO      3
#define SCRIPT_COMMAND_FLAG_SET     4
#define SCRIPT_COMMAND_FLAG_REMOVE  5
#define SCRIPT_COMMAND_TEMP_SUMMON 10

/// World constructor
World::World()
{
    m_ShutdownIdleMode = false;
    m_ShutdownTimer = 0;
    m_gameTime=time(NULL);
    m_startTime=m_gameTime;
    m_maxSessionsCount = 0;

	// Initialize Free IDs
	m_freeids.insert(1);
	m_freeids.insert(2);
	m_freeids.insert(3);
	m_freeids.insert(4);
	m_freeids.insert(5);
	m_freeids.insert(6);
	m_freeids.insert(7);
	m_freeids.insert(8);
	m_freeids.insert(9);
	m_freeids.insert(10);
	m_freeids.insert(11);
	m_freeids.insert(12);
	m_freeids.insert(13);
	m_freeids.insert(14);
	m_freeids.insert(15);
	m_freeids.insert(16);
	m_freeids.insert(17);
	m_freeids.insert(18);
	m_freeids.insert(19);
	m_freeids.insert(20);
	m_freeids.insert(21);
	m_freeids.insert(22);
	m_freeids.insert(23);
	m_freeids.insert(24);
	m_freeids.insert(25);
	m_freeids.insert(26);
	m_freeids.insert(27);
	m_freeids.insert(28);
	m_freeids.insert(29);
	m_freeids.insert(30);
	m_freeids.insert(31);

	m_map = new NoxMap();
}

/// World desctructor
World::~World()
{
}

/// Find a session by its id
WorldSession* World::FindSession(uint32 id) const
{
    SessionMap::const_iterator itr = m_sessions.find(id);

    if(itr != m_sessions.end())
        return itr->second;                                 // also can return NULL for kicked session
    else
        return NULL;
}

/// Remove a given session
bool World::RemoveSession(uint32 id)
{
    ///- Find the session, kick the user and put the session in "Kicked sessions" list
    SessionMap::iterator itr = m_sessions.find(id);

    if(itr != m_sessions.end() && itr->second)
    {
        if (itr->second->PlayerLoading())
            return false;
        itr->second->KickPlayer();

        // session can't be erased or deleted currently (to prevent iterator invalidation and socket problems)
        m_kicked_sessions.insert(itr->second);
        itr->second = NULL;
        return true;
    }

    return true;
}

/// Add a session to the session list
void World::AddSession(WorldSession* s)
{
    ASSERT(s);
    m_sessions[s->GetAccountId()] = s;
    m_maxSessionsCount = max(m_maxSessionsCount,uint32(m_sessions.size()));
}

/// Initialize the World
void World::SetInitialWorldSettings()
{
    ///- Initialize the random number generator
    srand((unsigned int)time(NULL));

	// We should get filenames from ini file
	// Must load thing.bin before the map
	std::string DataPath;
	if( !sNXConfig.GetString("DataPath",&DataPath) )// File Path;
	{
		sLog.outError("Could not load data path!!");
		PostQuitMessage(0);
	}
	std::string MapLoc;
	if( !sNXConfig.GetString("DefaultMap",&MapLoc) )// File Path;
	{
		sLog.outError("Could not load map path, using default value.");
		MapLoc = "maps\\manamine\\manamine.map";
	}

	std::string MapPath = DataPath + MapLoc;
	std::string ThingPath = DataPath + "thing.bin";
	std::string ModPath = DataPath + "modifier.bin";
	std::string MonPath = DataPath + "monster.bin";
	//std::string GamedataPath = DataPath + "gamedata.bin";

	std::string GameDataName;
	if( !sNXConfig.GetString("GamedataName",&GameDataName) )// File Path;
	{
		sLog.outError("Could not find gamedata name, using default");
		GameDataName = "Gamedata.bin";
	}
	bool GameDataEncoded = false;
	if( !sNXConfig.GetBool("EncodedGamedata",&GameDataEncoded) )// File Path;
	{
		sLog.outError("Could not load gamedata info, assuming gamedata.bin is original!!");
		//GameDataEncoded = true;
	}
	if( GameDataEncoded )
	{		
		fstream file;
		file.open(GameDataName.c_str(),ios::in | ios::binary);
		if( !file.is_open())
			return;		// return Failed
	
		file.seekg(0,ios::end);
		long len = file.tellg();
		file.seekg(0,ios::beg);
		unsigned char *buff = NULL;

		buff = new unsigned char [len];
		memset(buff,0x00,len);
	
		file.read((char*)buff,len); // Read file
		NoxCrypt::decrypt(buff, len, NC_GAMEDATA_BIN); // Decrypt by modifier
		file.close(); // Close file

		GameDataName = "Gamedata.txt";
		file.open((DataPath + GameDataName).c_str(),ios::out | ios::trunc);
		file.write((const char*)buff,len);
		file.close();

		if( buff ) // If buff was used
			delete [] buff; // Clear data
	}

    if (!sGameConfig.SetSource((GameDataName).c_str()))
    {
        sLog.outError("Could not find gamedata configuration file.");
        return;
    }
    sLog.outString("Using gamedata configuration file.");

	sLog.outError("Loading thing.bin");
	fstream* thing = new fstream(ThingPath.c_str(), ios_base::in|ios_base::binary);
	sThingBin.Load_Thingdb(thing);
	thing->close();
	delete thing;

	sLog.outError("Loading modifier.bin");
	sModifierBin.LoadBin((char*)ModPath.c_str());

	sLog.outError("Loading monster.bin");
	sMonsterBin.LoadBin((char*)MonPath.c_str());

	sLog.outError((((std::string)"Loading map: ")+MapPath).c_str());
	sLog.outError("");
	sLog.outError("");
	m_map->open((char*)MapPath.c_str());

    ///- Read the version of the configuration file and warn the user in case of emptiness or mismatch
    /*uint32 confVersion = sConfig.GetIntDefault("ConfVersion", 0);
    if(!confVersion)
    {
        sLog.outError("*****************************************************************************");
        sLog.outError(" WARNING: mangosd.conf does not include a ConfVersion variable.");
        sLog.outError("          Your configuration file may be out of date!");
        sLog.outError("*****************************************************************************");
        clock_t pause = 3000 + clock();
        while (pause > clock());
    }
    else
    {
        if (confVersion < _MANGOSDCONFVERSION)
        {
            sLog.outError("*****************************************************************************");
            sLog.outError(" WARNING: Your mangosd.conf version indicates your conf file is out of date!");
            sLog.outError("          Please check for updates, as your current default values may cause");
            sLog.outError("          unexpected behavior.");
            sLog.outError("*****************************************************************************");
            clock_t pause = 3000 + clock();
            while (pause > clock());
        }
    }

    ///- Read the player limit and the Message of the day from the config file
    SetPlayerLimit( sConfig.GetIntDefault("PlayerLimit", DEFAULT_PLAYER_LIMIT) );
    SetMotd( sConfig.GetStringDefault("Motd", "Welcome to the Massive Network Game Object Server." ).c_str() );

    ///- Read all rates from the config file
    rate_values[RATE_HEALTH]      = sConfig.GetFloatDefault("Rate.Health", 1);
    rate_values[RATE_POWER_MANA]  = sConfig.GetFloatDefault("Rate.Power1", 1);
    rate_values[RATE_POWER_RAGE]  = sConfig.GetFloatDefault("Rate.Power2", 1);
    rate_values[RATE_POWER_FOCUS] = sConfig.GetFloatDefault("Rate.Power3", 1);
    rate_values[RATE_DROP_ITEMS]  = sConfig.GetFloatDefault("Rate.Drop.Items", 1);
    rate_values[RATE_DROP_MONEY]  = sConfig.GetFloatDefault("Rate.Drop.Money", 1);
    rate_values[RATE_XP_KILL]     = sConfig.GetFloatDefault("Rate.XP.Kill", 1);
    rate_values[RATE_XP_QUEST]    = sConfig.GetFloatDefault("Rate.XP.Quest", 1);
    rate_values[RATE_XP_EXPLORE]  = sConfig.GetFloatDefault("Rate.XP.Explore", 1);
    rate_values[RATE_CREATURE_NORMAL_DAMAGE]          = sConfig.GetFloatDefault("Rate.Creature.Normal.Damage", 1);
    rate_values[RATE_CREATURE_ELITE_ELITE_DAMAGE]     = sConfig.GetFloatDefault("Rate.Creature.Elite.Elite.Damage", 1);
    rate_values[RATE_CREATURE_ELITE_RAREELITE_DAMAGE] = sConfig.GetFloatDefault("Rate.Creature.Elite.RAREELITE.Damage", 1);
    rate_values[RATE_CREATURE_ELITE_WORLDBOSS_DAMAGE] = sConfig.GetFloatDefault("Rate.Creature.Elite.WORLDBOSS.Damage", 1);
    rate_values[RATE_CREATURE_ELITE_RARE_DAMAGE]      = sConfig.GetFloatDefault("Rate.Creature.Elite.RARE.Damage", 1);
    rate_values[RATE_CREATURE_NORMAL_HP]          = sConfig.GetFloatDefault("Rate.Creature.Normal.HP", 1);
    rate_values[RATE_CREATURE_ELITE_ELITE_HP]     = sConfig.GetFloatDefault("Rate.Creature.Elite.Elite.HP", 1);
    rate_values[RATE_CREATURE_ELITE_RAREELITE_HP] = sConfig.GetFloatDefault("Rate.Creature.Elite.RAREELITE.HP", 1);
    rate_values[RATE_CREATURE_ELITE_WORLDBOSS_HP] = sConfig.GetFloatDefault("Rate.Creature.Elite.WORLDBOSS.HP", 1);
    rate_values[RATE_CREATURE_ELITE_RARE_HP]      = sConfig.GetFloatDefault("Rate.Creature.Elite.RARE.HP", 1);
    rate_values[RATE_CREATURE_AGGRO]  = sConfig.GetFloatDefault("Rate.Creature.Aggro", 1);
    rate_values[RATE_REST_INGAME]                    = sConfig.GetFloatDefault("Rate.Rest.InGame", 1);
    rate_values[RATE_REST_OFFLINE_IN_TAVERN_OR_CITY] = sConfig.GetFloatDefault("Rate.Rest.Offline.InTavernOrCity", 1);
    rate_values[RATE_REST_OFFLINE_IN_WILDERNESS]     = sConfig.GetFloatDefault("Rate.Rest.Offline.InWilderness", 1);
    rate_values[RATE_DAMAGE_FALL]  = sConfig.GetFloatDefault("Rate.Damage.Fall", 1);
    rate_values[RATE_AUCTION_TIME]  = sConfig.GetFloatDefault("Rate.Auction.Time", 1);
    rate_values[RATE_AUCTION_DEPOSIT] = sConfig.GetFloatDefault("Rate.Auction.Deposit", 1);
    rate_values[RATE_AUCTION_CUT] = sConfig.GetFloatDefault("Rate.Auction.Cut", 1);

    ///- Read other configuration items from the config file

    m_configs[CONFIG_COMPRESSION] = sConfig.GetIntDefault("Compression", 1);
    if(m_configs[CONFIG_COMPRESSION] < 1 || m_configs[CONFIG_COMPRESSION] > 9)
    {
        sLog.outError("Compression level (%i) must be in range 1..9. Using default compression level (1).",m_configs[CONFIG_COMPRESSION]);
        m_configs[CONFIG_COMPRESSION] = 1;
    }
    m_configs[CONFIG_GRID_UNLOAD] = sConfig.GetIntDefault("GridUnload", 1);
    m_configs[CONFIG_INTERVAL_SAVE] = sConfig.GetIntDefault("PlayerSaveInterval", 900000);
    m_configs[CONFIG_INTERVAL_GRIDCLEAN] = sConfig.GetIntDefault("GridCleanUpDelay", 300000);
    m_configs[CONFIG_INTERVAL_MAPUPDATE] = sConfig.GetIntDefault("MapUpdateInterval", 100);
    m_configs[CONFIG_INTERVAL_CHANGEWEATHER] = sConfig.GetIntDefault("ChangeWeatherInterval", 600000);
    m_configs[CONFIG_PORT_WORLD] = sConfig.GetIntDefault("WorldServerPort", DEFAULT_WORLDSERVER_PORT);
    m_configs[CONFIG_SOCKET_SELECTTIME] = sConfig.GetIntDefault("SocketSelectTime", DEFAULT_SOCKET_SELECT_TIME);
    m_configs[CONFIG_GROUP_XP_DISTANCE] = sConfig.GetIntDefault("MaxGroupXPDistance", 74);
    m_configs[CONFIG_GROUP_XP_DISTANCE] = m_configs[CONFIG_GROUP_XP_DISTANCE]*m_configs[CONFIG_GROUP_XP_DISTANCE];
    m_configs[CONFIG_GROUP_XP_LEVELDIFF] = sConfig.GetIntDefault("MaxGroupXPLevelDiff", 10);
    /// \todo Add MonsterSight and GuarderSight (with meaning) in mangosd.conf or put them as define
    m_configs[CONFIG_SIGHT_MONSTER] = sConfig.GetIntDefault("MonsterSight", 400);
    m_configs[CONFIG_SIGHT_GUARDER] = sConfig.GetIntDefault("GuarderSight", 500);
    m_configs[CONFIG_GAME_TYPE] = sConfig.GetIntDefault("GameType", 0);
    m_configs[CONFIG_ALLOW_TWO_SIDE_ACCOUNTS] = sConfig.GetIntDefault("AllowTwoSide.Accounts", 0);
    m_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHAT]    = sConfig.GetIntDefault("AllowTwoSide.Interaction.Chat",0);
    m_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHANNEL] = sConfig.GetIntDefault("AllowTwoSide.Interaction.Channel",0);
    m_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP]   = sConfig.GetIntDefault("AllowTwoSide.Interaction.Group",0);
    m_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD]   = sConfig.GetIntDefault("AllowTwoSide.Interaction.Guild",0);
    m_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_TRADE]   = sConfig.GetIntDefault("AllowTwoSide.Interaction.Trade",0);
    m_configs[CONFIG_ALLOW_TWO_SIDE_WHO_LIST] = sConfig.GetIntDefault("AllowTwoSide.WhoList", 0);
    m_configs[CONFIG_MAX_PLAYER_LEVEL] = sConfig.GetIntDefault("MaxPlayerLevel", 60);
    if(m_configs[CONFIG_MAX_PLAYER_LEVEL] > 255)
    {
        sLog.outError("MaxPlayerLevel (%i) must be in range 1..255. Set to 255.",m_configs[CONFIG_MAX_PLAYER_LEVEL]);
        m_configs[CONFIG_MAX_PLAYER_LEVEL] = 255;
    }
    m_configs[CONFIG_IGNORE_AT_LEVEL_REQUIREMENT] = sConfig.GetBoolDefault("IgnoreATLevelRequirement", 0);

    m_configs[CONFIG_MAX_PRIMARY_TRADE_SKILL] = sConfig.GetIntDefault("MaxPrimaryTradeSkill", 2);
    m_configs[CONFIG_MIN_PETITION_SIGNS] = sConfig.GetIntDefault("MinPetitionSigns", 9);
    if(m_configs[CONFIG_MIN_PETITION_SIGNS] > 9)
    {
        sLog.outError("MinPetitionSigns (%i) must be in range 0..9. Set to 9.",m_configs[CONFIG_MIN_PETITION_SIGNS]);
        m_configs[CONFIG_MIN_PETITION_SIGNS] = 9;
    }

    m_configs[CONFIG_GM_WISPERING_TO] = sConfig.GetIntDefault("GM.WhisperingTo",0);
    m_configs[CONFIG_GM_IN_GM_LIST]  = sConfig.GetIntDefault("GM.InGMList",0);
    m_configs[CONFIG_GM_IN_WHO_LIST]  = sConfig.GetIntDefault("GM.InWhoList",0);
    m_configs[CONFIG_GM_LOGIN_STATE]  = sConfig.GetIntDefault("GM.LoginState",2);
    m_configs[CONFIG_GM_LOG_TRADE] = sConfig.GetIntDefault("GM.LogTrade", 1);

    m_configs[CONFIG_GROUP_VISIBILITY] = sConfig.GetIntDefault("GroupVisibility",0);

    m_configs[CONFIG_MAIL_DELIVERY_DELAY] = sConfig.GetIntDefault("MailDeliveryDelay",HOUR);

    m_configs[CONFIG_SKILL_CHANCE_ORANGE] = sConfig.GetIntDefault("SkillChance.Orange",100);
    m_configs[CONFIG_SKILL_CHANCE_YELLOW] = sConfig.GetIntDefault("SkillChance.Yellow",75);
    m_configs[CONFIG_SKILL_CHANCE_GREEN]  = sConfig.GetIntDefault("SkillChance.Green",25);
    m_configs[CONFIG_SKILL_CHANCE_GREY]   = sConfig.GetIntDefault("SkillChance.Grey",0);

    m_configs[CONFIG_MAX_OVERSPEED_PINGS] = sConfig.GetIntDefault("MaxOverspeedPings",2);
    if(m_configs[CONFIG_MAX_OVERSPEED_PINGS] != 0 && m_configs[CONFIG_MAX_OVERSPEED_PINGS] < 2)
    {
        sLog.outError("MaxOverspeedPings (%i) must be in range 2..infinity (or 0 to disable check. Set to 2.",m_configs[CONFIG_MAX_OVERSPEED_PINGS]);
        m_configs[CONFIG_MAX_OVERSPEED_PINGS] = 2;
    }

    m_configs[CONFIG_SAVE_RESPAWN_TIME_IMMEDIATLY] = sConfig.GetIntDefault("SaveRespawnTimeImmediately",1);
    m_configs[CONFIG_WEATHER] = sConfig.GetIntDefault("ActivateWeather",1);

    ///- Read the "Data" directory from the config file
    m_dataPath = sConfig.GetStringDefault("DataDir","./");
    if((m_dataPath.at(m_dataPath.length()-1)!='/') && (m_dataPath.at(m_dataPath.length()-1)!='\\'))
        m_dataPath.append("/");

    sLog.outString("Using DataDir %s",m_dataPath.c_str());

    ///- Check the existence of the map files for all races' startup areas.
    if(   !MapManager::ExistMAP(0,-6240.32, 331.033)
        ||!MapManager::ExistMAP(0,-8949.95,-132.493)
        ||!MapManager::ExistMAP(0,-8949.95,-132.493)
        ||!MapManager::ExistMAP(1,-618.518,-4251.67)
        ||!MapManager::ExistMAP(0, 1676.35, 1677.45)
        ||!MapManager::ExistMAP(1, 10311.3, 832.463)
        ||!MapManager::ExistMAP(1,-2917.58,-257.98)
        ||!MapManager::ExistMAP(530,10349.6,-6357.29)
        ||!MapManager::ExistMAP(530,-3961.64,-13931.2))
    {
        sLog.outError("Correct *.map files not found in path '%smaps'. Please place *.map files in the directory pointed by this path or correct the DataDir value in the mangosd.conf file.",m_dataPath.c_str());
        exit(1);
    }

    ///- Update the realm entry in the database with the realm type from the config file
    //No SQL injection as values are treated as integers
    loginDatabase.PExecute("UPDATE `realmlist` SET `icon` = %u WHERE `id` = '%d'", m_configs[CONFIG_GAME_TYPE],realmID);

    ///- Remove the bones after a restart
    sDatabase.PExecute("DELETE FROM `corpse` WHERE `bones_flag` = '1'");

    ///- Load the DBC files
    sLog.outString("Initialize data stores...");
    LoadDBCStores(m_dataPath);

    ///- Clean up and pack instances
    sLog.outString( "Cleaning up instances..." );
    objmgr.CleanupInstances();

    sLog.outString( "Packing instances..." );
    objmgr.PackInstances();

    ///- Load static data tables from the database
    sLog.outString( "Loading Game Object Templates..." );
    objmgr.LoadGameobjectInfo();

    sLog.outString( "Loading Spell Chain Data..." );
    objmgr.LoadSpellChains();

    sLog.outString( "Loading Spell Learn Skills..." );
    objmgr.LoadSpellLearnSkills();                          // must be after LoadSpellChains

    sLog.outString( "Loading Spell Learn Spells..." );
    objmgr.LoadSpellLearnSpells();

    sLog.outString( "Loading Spell Proc Event conditions..." );
    objmgr.LoadSpellProcEvents();

    sLog.outString( "Loading Aggro Spells Definitions...");
    objmgr.LoadSpellThreats();

    sLog.outString( "Loading NPC Texts..." );
    objmgr.LoadGossipText();

    sLog.outString( "Loading Items..." );
    objmgr.LoadItemPrototypes();

    sLog.outString( "Loading Page Texts..." );
    objmgr.LoadPageTexts();

    sLog.outString( "Loading Item Texts..." );
    objmgr.LoadItemTexts();

    sLog.outString( "Loading Creature templates..." );
    objmgr.LoadCreatureTemplates();

    sLog.outString( "Loading Creature Reputation OnKill Data..." );
    objmgr.LoadReputationOnKill();

    sLog.outString( "Loading Creature Addon Data..." );
    objmgr.LoadCreatureAddons();

    sLog.outString( "Loading Weather Data..." );
    objmgr.LoadWeatherZoneChances();

    sLog.outString( "Loading Quests..." );
    objmgr.LoadQuests();                                    // must be loaded after DBCs, creature_template, item_template, gameobject tables

    sLog.outString( "Loading AreaTrigger definitions..." );
    objmgr.LoadAreaTriggers();

    sLog.outString( "Loading Quest Area Triggers..." );
    objmgr.LoadQuestAreaTriggers();                         // must be after LoadQuests

    sLog.outString( "Loading Tavern Area Triggers..." );    // must be after LoadAreaTriggers
    objmgr.LoadTavernAreaTriggers();

    sLog.outString( "Loading player Create Info & Level Stats..." );
    objmgr.LoadPlayerInfo();

    sLog.outString( "Loading Pet Name Parts..." );
    objmgr.LoadPetNames();

    sLog.outString( "Loading pet level stats..." );
    objmgr.LoadPetLevelInfo();

    sLog.outString( "Loading Player Corpses..." );
    objmgr.LoadCorpses();

    sLog.outString( "Loading Loot Tables..." );
    LoadLootTables();

    ///- Load dynamic data tables from the database
    sLog.outString( "Loading Auctions..." );
    objmgr.LoadAuctionItems();
    objmgr.LoadAuctions();

    sLog.outString( "Loading Guilds..." );
    objmgr.LoadGuilds();

    sLog.outString( "Loading Groups.." );
    objmgr.LoadGroups();

    objmgr.SetHighestGuids();

    ///- Handle outdated emails (delete/return)
    sLog.outString( "Returning old mails..." );
    objmgr.ReturnOrDeleteOldMails(false);

    ///- Load and initialize scripts
    sLog.outString( "Loading Scripts..." );
    objmgr.LoadScripts(sScripts,      "scripts");           // quest scripts
    objmgr.LoadScripts(sSpellScripts, "spell_scripts");     // spell casting scripts

    sLog.outString( "Initializing Scripts..." );
    if(!LoadScriptingModule())
        exit(1);

    ///- Initialize game time and timers
    m_gameTime = time(NULL);
    m_startTime=m_gameTime;

    m_timers[WUPDATE_OBJECTS].SetInterval(0);
    m_timers[WUPDATE_SESSIONS].SetInterval(0);
    m_timers[WUPDATE_WEATHERS].SetInterval(1000);
    m_timers[WUPDATE_AUCTIONS].SetInterval(60000);          //set auction update interval to 1 minute

    //to set mailtimer to return mails every day between 4 and 5 am
    //mailtimer is increased when updating auctions
    //one second is 1000 -(tested on win system)
    mail_timer = ((((localtime( &m_gameTime )->tm_hour + 20) % 24)* HOUR * 1000) / m_timers[WUPDATE_AUCTIONS].GetInterval() );
                                                            //1440
    mail_timer_expires = ( (DAY * 1000) / (m_timers[WUPDATE_AUCTIONS].GetInterval()));
    sLog.outDebug("Mail timer set to: %u, mail return is called every %u minutes", mail_timer, mail_timer_expires);

    ///- Initialize Battlegrounds
    sLog.outString( "WORLD: Starting BattleGround System" );
    sBattleGroundMgr.CreateInitialBattleGrounds();

    ///- Initialize MapManager, AI, Waypoints, ...
    MapManager::Instance().Initialize();
    RedZone::Initialize();
    AIRegistry::Initialize();
    WaypointMovementGenerator::Initialize();
    Player::InitVisibleBits();

    //Not sure if this can be moved up in the sequence (with static data loading) as it uses MapManager
    sLog.outString( "Loading Transports..." );
    MapManager::Instance().LoadTransports();

    ///- Start the event system and register the corpse handler event
    sLog.outString( "WORLD: Starting Event System" );
    StartEventSystem();

    sLog.outString( "WORLD: Starting Corpse Handler" );
    // global event to erase corpses/bones
    // deleting expired bones time > 20 minutes and corpses > 3 days
    // it is run each 20 minutes
    AddEvent(&HandleCorpsesErase,NULL,20*MINUTE*1000,false,true);

    sLog.outString( "WORLD: World initialized" );*/
	m_gameTime = time(NULL);
    m_startTime=m_gameTime;

    m_timers[WUPDATE_OBJECTS].SetInterval(30);
    m_timers[WUPDATE_SESSIONS].SetInterval(30);				
    m_timers[WUPDATE_WEATHERS].SetInterval(1000);
    m_timers[WUPDATE_AUCTIONS].SetInterval(60000);          //set auction update interval to 1 minute
}

/// Update the World !
void World::Update(time_t diff)
{

    ///- Update the different timers
    for(int i = 0; i < WUPDATE_COUNT; i++)
	{
        if(m_timers[i].GetCurrent()>=0)
            m_timers[i].Update(diff);
		else m_timers[i].SetCurrent(0);
	}

    ///- Update the game time and check for shutdown time
    _UpdateGameTime();

    /// <li> Handle session updates when the timer has passed
    if (m_timers[WUPDATE_SESSIONS].Passed())
    {
        m_timers[WUPDATE_SESSIONS].Reset();

        ///- Delete kicked sessions
        for (std::set<WorldSession*>::iterator itr = m_kicked_sessions.begin(); itr != m_kicked_sessions.end(); ++itr)
            delete *itr;
        m_kicked_sessions.clear();

        ///- Then send an update signal to remaining ones
        for (SessionMap::iterator itr = m_sessions.begin(), next; itr != m_sessions.end(); itr = next)
        {
            next = itr;
            next++;

            if(!itr->second)
                continue;

            ///- and remove not active sessions from the list
            if(!itr->second->Update(diff))  // As interval = 0
            {
                delete itr->second;
                m_sessions.erase(itr);
            }
        }
    }
	if (m_timers[WUPDATE_OBJECTS].Passed())
	{
		m_timers[WUPDATE_OBJECTS].Reset();
		
		objmgr.Update(diff);
		ObjectAccessor::Instance().Update(diff);
	}
}

/// Update the game time
void World::_UpdateGameTime()
{
    ///- update the time
    time_t thisTime = time(NULL);
    uint32 elapsed = uint32(thisTime - m_gameTime);
    m_gameTime = thisTime;

    ///- if there is a shutdown timer
    if(m_ShutdownTimer > 0 && elapsed > 0)
    {
        ///- ... and it is overdue, stop the world (set m_stopEvent)
        if( m_ShutdownTimer <= elapsed )
        {
            if(!m_ShutdownIdleMode || GetSessionCount()==0)
                m_stopEvent = true;
            else
                m_ShutdownTimer = 1;                        // minimum timer value to wait idle state
        }
        ///- ... else decrease it and if necessary display a shutdown countdown to the users
        else
        {
            m_ShutdownTimer -= elapsed;

            ShutdownMsg();
        }
    }
    return;
}

/// Shutdown the server
void World::ShutdownServ(uint32 time, bool idle)
{
    m_ShutdownIdleMode = idle;

    ///- If the shutdown time is 0, set m_stopEvent (except if shutdown is 'idle' with remaining sessions)
    if(time==0)
    {
        if(!idle || GetSessionCount()==0)
            m_stopEvent = true;
        else
            m_ShutdownTimer = 1;                            //So that the session count is re-evaluated at next world tick
    }
    ///- Else set the shutdown timer and warn users
    else
    {
        m_ShutdownTimer = time;
        ShutdownMsg(true);
    }
}

/// Display a shutdown message to the user(s)
void World::ShutdownMsg(bool show, Player* player)
{
    // not show messages for idle shutdown mode
    if(m_ShutdownIdleMode)
        return;

    ///- Display a message every 12 hours, hours, 5 minutes, minute, 5 seconds and finally seconds
    if ( show ||
        (m_ShutdownTimer < 10) ||
                                                            // < 30 sec; every 5 sec
        (m_ShutdownTimer<30        && (m_ShutdownTimer % 5         )==0) ||
                                                            // < 5 min ; every 1 min
        (m_ShutdownTimer<5*MINUTE  && (m_ShutdownTimer % MINUTE    )==0) ||
                                                            // < 30 min ; every 5 min
        (m_ShutdownTimer<30*MINUTE && (m_ShutdownTimer % (5*MINUTE))==0) ||
                                                            // < 12 h ; every 1 h
        (m_ShutdownTimer<12*HOUR   && (m_ShutdownTimer % HOUR      )==0) ||
                                                            // > 12 h ; every 12 h
        (m_ShutdownTimer>12*HOUR   && (m_ShutdownTimer % (12*HOUR) )==0))
    {
//        std::string str = secsToTimeString(m_ShutdownTimer);
        //SendServerMessage(SERVER_MSG_SHUTDOWN_TIME,str.c_str(),player);
  //      DEBUG_LOG("Server is shuttingdown in %s",str.c_str());
    }
}

/// Cancel a planned server shutdown
void World::ShutdownCancel()
{
    if(!m_ShutdownTimer)
        return;

    m_ShutdownIdleMode = false;
    m_ShutdownTimer = 0;
    //SendServerMessage(SERVER_MSG_SHUTDOWN_CANCELLED);

    DEBUG_LOG("Server shuttingdown cancelled.");
}


uint8 World::RequestFreeId()
{
	if(m_freeids.empty())
		return 0xFF;
	uint8 id = *(m_freeids.begin());
	m_freeids.erase(m_freeids.begin());
	return id;
}

void World::ReturnFreeId(uint8 id)
{
	m_freeids.insert(id);
}
