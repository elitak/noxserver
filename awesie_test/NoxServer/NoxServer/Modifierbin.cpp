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

#include "Modifierbin.h"
#include <iostream>
#include "global.h"

#define END "END" // Std definition of the end of the file

/////////////////////////////////////////////////////////////////////////////
// This function loads the modifier.bin data into the three tier list ///////
/////////////////////////////////////////////////////////////////////////////
void ModifierBin::Load_Modifier(char* buff, long len)
{
	string st = buff; // Create retention string
	stringstream rdr(st,ios::in); // Create string stream using rentention string

	char buff2[255]; // Our data buffer
	MOD_LIST list;
	MOD_OBJ obj;
	ModProperty prop;

	// While ! END, get next line
	for(; GetString(&rdr,buff2) && TestEnd(buff2); NULL)
	{
		if( !buff2[0] ) // Ignore null lines
			continue;

		// While ! END, get next line
		for(strcpy(list.SectionTitle,buff2); GetString(&rdr,buff2) && TestEnd(buff2); NULL)
		{
			if( !buff2[0] )
				continue;

			for(strcpy(obj.ObjectName,buff2); GetString(&rdr,buff2) && TestEnd(buff2); NULL)
			{
				if( !buff2[0] )
					continue;

				prop.Load(buff2); // Load property into the object
				obj.Properties.push_back(prop);
						}
			list.ElementList.push_back(obj); // Load object into the header list
			obj.Properties.clear();
		}	
		ModifierList.push_back(list); // Load header list into the master list
		list.ElementList.clear();
	}
}

/////////////////////////////////////////////////////////////////////////////
// This function reads and processes a string ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool ModifierBin::GetString(stringstream * rdr, char * buff)
{
	if( rdr->eof() ) // If end of file, return false
		return false;

	rdr->getline(buff,255);				// Get current line
	strcws(buff);						// Remove whitespaces
	strrc(buff,';');					// Remove character ';'
	return( true );						// Return success / fail
}

/////////////////////////////////////////////////////////////////////////////
// This function tests for the end of a modifier.bin segment ////////////////
/////////////////////////////////////////////////////////////////////////////
bool ModifierBin::TestEnd(char * buff)
{
	int i=0;
	int len = strlen(buff);
	for(i=0; i<len && !isCharw(buff[i]); i++); // Remove all beginning whitespaces
	return(strcmp((const char*)&buff[i],END)); // Test for the END
}