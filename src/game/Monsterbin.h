/* 
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

#ifndef H_MONBIN
#define H_MONBIN

#include <fstream>
#include "NoxThingClassDefs.h"
#include "Policies/Singleton.h"
#include "NoxCrypt.h"
#include "stringfuncs.h"
#include "noxbuffer.h"
#include "modifierbin.h"
#include <list>

using std::list;
using namespace std;


/////////////////////////////////////////////////////////////////////////////
// This class contains and loads all of the monster.bin data		 ////////
/////////////////////////////////////////////////////////////////////////////
class MonsterBin
{

private:
	void Load_Monster(char* buff, long len);			// Loads the monster.bin from a buffer
    bool GetString(stringstream * rdr, char * buff);	// Explained in .cpp file
	bool TestEnd(char * buff);							// Explained in .cpp file

public:
	list<MOD_OBJ> MonsterList;	// List of monster.bin sections

	// Loads the monster.bin from a filepath
	bool LoadBin(char filepath[])
	{
		fstream ifile;
		ifile.open(filepath,ios::in | ios::binary);
		if( !ifile.is_open())
			return false;		// return Failed
	
		ifile.seekg(0,ios::end);
		long len = ifile.tellg();
		ifile.seekg(0,ios::beg);
		unsigned char *buff = NULL;

		buff = new unsigned char [len];
		memset(buff,0x00,len);
	
		ifile.read((char*)buff,len); // Read file
		NoxCrypt::decrypt(buff, len, NC_MONSTER_BIN); // Decrypt by monster
		ifile.close(); // Close file

		Load_Monster((char *)buff,len); // Load from buffer

		if( buff ) // If buff was used
			delete [] buff; // Clear data

		return(true); // Return worked
	}
};

#define sMonsterBin MaNGOS::Singleton<MonsterBin>::Instance()
#endif