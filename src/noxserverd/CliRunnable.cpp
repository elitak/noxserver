/* 
 * Copyright (C) 2005,2006,2007 MaNGOS <http://www.mangosproject.org/>
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

/// \addtogroup mangosd
/// @{
/// \file

#include "Common.h"
#include "Log.h"
#include "World.h"
#include "ObjectMgr.h"
#include "WorldSession.h"
#include "SystemConfig.h"
#include "Config/ConfigEnv.h"
#include "Util.h"

#ifdef ENABLE_CLI
#include "CliRunnable.h"

typedef int(* pPrintf)(const char*,...);
typedef void(* pCliFunc)(char *,pPrintf);

/// Storage structure for commands
typedef struct
{
    char const * cmd;
    pCliFunc Func;
    char const * description;
}CliCommand;

//func prototypes must be defined

void CliHelp(char*,pPrintf);
void CliVersion(char*,pPrintf);
void CliExit(char*,pPrintf);
void CliIdleShutdown(char*,pPrintf zprintf);
void CliShutdown(char*,pPrintf zprintf);

/// Table of known commands
const CliCommand Commands[]=
{
    {"help", & CliHelp,"Display this help message"},
    {"version", & CliVersion,"Display server version"},
    {"idleshutdown", & CliIdleShutdown,"Shutdown server with some delay when not active connections at server"},
    {"shutdown", & CliShutdown,"Shutdown server with some delay"},
    {"exit", & CliExit,"Shutdown server NOW"}
};
/// \todo Need some pragma pack? Else explain why in a comment.
#define CliTotalCmds sizeof(Commands)/sizeof(CliCommand)

/// Print the list of commands and associated description
void CliHelp(char*,pPrintf zprintf)
{
    for (unsigned int x=0;x<CliTotalCmds;x++)
        zprintf("%-13s - %s.\r\n",Commands[x].cmd ,Commands[x].description);
}

/// Exit the realm
void CliExit(char*,pPrintf zprintf)
{
    zprintf( "Exiting daemon...\r\n" );
    World::m_stopEvent = true;
}

/// Shutdown the server (with some delay) as soon as no active connections remain on the server
void CliIdleShutdown(char* command,pPrintf zprintf)
{
    char *args = strtok(command," ");

    if(!args)
    {
        zprintf("Syntax is: idleshutdown <seconds|cancel>\r\n");
        return;
    }

    if(std::string(args)=="cancel")
    {
        sWorld.ShutdownCancel();
    }
    else
    {

        uint32 time = atoi(args);

        ///- Prevent interpret wrong arg value as 0 secs shutdown time
        if(time==0 && (args[0]!='0' || args[1]!='\0') || time < 0)
        {
            zprintf("Syntax is: idleshutdown <seconds|cancel>\r\n");
            return;
        }

        sWorld.ShutdownServ(time,true);
    }
}

/// Shutdown the server with some delay
void CliShutdown(char* command,pPrintf zprintf)
{
    char *args = strtok(command," ");

    if(!args)
    {
        zprintf("Syntax is: shutdown <seconds|cancel>\r\n");
        return;
    }

    if(std::string(args)=="cancel")
    {
        sWorld.ShutdownCancel();
    }
    else
    {
        int32 time = atoi(args);

        ///- Prevent interpret wrong arg value as 0 secs shutdown time
        if(time==0 && (args[0]!='0' || args[1]!='\0') || time < 0)
        {
            zprintf("Syntax is: shutdown <seconds|cancel>\r\n");
            return;
        }

        sWorld.ShutdownServ(time);
    }
}

/// Display %MaNGOS version
void CliVersion(char*,pPrintf zprintf)
{
                                                            //<--maybe better append to info cmd
    zprintf( "MaNGOS daemon version is %s\r\n", _FULLVERSION );
}

/// Command parser and dispatcher
void ParseCommand( pPrintf zprintf, char* input)
{
    unsigned int x;
    if (!input)
        return;

    unsigned int l=strlen(input);
    char *supposedCommand=NULL,* arguments=(char*)("");
    if(!l)
        return;

    ///- Get the command and the arguments
    supposedCommand = strtok(input," ");
    if (!supposedCommand) return;
    if (l>strlen(supposedCommand))
        arguments=&input[strlen(supposedCommand)+1];

    ///- Circle through the command table and invoke the appropriate handler
    for ( x=0;x<CliTotalCmds;x++)
        if(!strcmp(Commands[x].cmd,supposedCommand))
    {
        Commands[x].Func(arguments,zprintf);
        break;
    }

    ///- Display an error message if the command is unknown
    if(x==CliTotalCmds)
        zprintf("Unknown command: %s\r\n", input);
}

/// @}

/// %Thread start
void CliRunnable::run()
{
    ///- Init new SQL thread for the world database (one connection call enough)
    sDatabase.ThreadStart();                                // let thread do safe mySQL requests

    char commandbuf[256];

    ///- Display the list of available CLI functions then beep
    sLog.outString("");
    /// \todo Shoudn't we use here also the sLog singleton?
    CliHelp(NULL,&printf);

	if(sConfig.GetBoolDefault("BeepAtStart"))
    {
        printf("\a");                                       // \a = Alert
    }

    ///- As long as the World is running (no World::m_stopEvent), get the command line and handle it
    while (!World::m_stopEvent)
    {
        printf("mangos>");
        fflush(stdout);
        char *command = fgets(commandbuf,sizeof(commandbuf),stdin);
        if (command != NULL)
        {
            for(int x=0;command[x];x++)
                if(command[x]=='\r'||command[x]=='\n')
            {
                command[x]=0;
                break;
            }
            //// \todo Shoudn't we use here also the sLog singleton?
            ParseCommand(&printf,command);
        }
        else if (feof(stdin))
        {
            World::m_stopEvent = true;
        }
    }

    ///- End the database thread
    sDatabase.ThreadEnd();                                  // free mySQL thread resources
}
#endif
