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

#ifndef H_MODBIN
#define H_MODBIN

#include <fstream>
#include "NoxThingClassDefs.h"
#include "Policies/Singleton.h"
#include "NoxCrypt.h"
#include "stringfuncs.h"
#include "noxbuffer.h"
#include <list>

using std::list;
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// This structure holds a single property entry for an object ///////////////
/////////////////////////////////////////////////////////////////////////////
struct ModProperty
{
public:
  char Ident[0x50]; // Variable/Enum ID
  char Value[0x50]; // Value/Test string
  
  // Split into componants and load property
  void Load(char string[])
  {
	char * tes = NULL;

    // Splitting string	
	tes = strtok(string, " =");
	if( tes != NULL )
		strcpy(Ident,tes);

	// #TODO: Change so it ignores the () comments?
	// Or leave it to be processed out during parsing?
	tes = strtok(NULL, " =");
	// #TODO?: tes = strtok(tes, " (");
	if( tes != NULL)
		strcpy(Value,tes);
	// #END TODO

  }

  // On initialization, memset structure to NULL
  ModProperty()
  {
	memset((void*)this,0x00,sizeof(*this));
  }
};


/////////////////////////////////////////////////////////////////////////////
// This structure holds a single object in the master section ///////////////
/////////////////////////////////////////////////////////////////////////////
struct MOD_OBJ
{
private:
public:

	char ObjectName[0x50];			// Objects name, hardcoded to 50 characters 
	list<ModProperty> Properties;	// Property list

};

/////////////////////////////////////////////////////////////////////////////
// This structure is the header structure for a modifier.bin section ////////
// it contains a list of all objects in that section				 ////////
/////////////////////////////////////////////////////////////////////////////
struct MOD_LIST
{
private:
public:

char SectionTitle[0x50];		// Section name, HardCoded to be 80 chars
list<MOD_OBJ> ElementList;		// List of objects in the section

};

/////////////////////////////////////////////////////////////////////////////
// This class contains and loads all of the modifier.bin data		 ////////
/////////////////////////////////////////////////////////////////////////////
class ModifierBin
{

private:
	void Load_Modifier(char* buff, long len);			// Loads the modifier.bin from a buffer
    bool GetString(stringstream * rdr, char * buff);	// Explained in .cpp file
	bool TestEnd(char * buff);							// Explained in .cpp file

public:
	list<MOD_LIST> ModifierList;	// List of modifier.bin sections

	// Loads the modifier.bin from a filepath
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
		NoxCrypt::decrypt(buff, len, NC_MODIFIER_BIN); // Decrypt by modifier
		ifile.close(); // Close file

		Load_Modifier((char *)buff,len); // Load from buffer

		if( buff ) // If buff was used
			delete [] buff; // Clear data

		return(true); // Return worked
	}
};

#define sModifierBin MaNGOS::Singleton<ModifierBin>::Instance()
#endif