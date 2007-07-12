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

#include "Errors.h"
#include "NoxMap.h"
#include "ObjectMgr.h"

bool NoxMap::open(char *fileName)
{
	char buffer[255];
	if(!NoxBuffer::open(fileName))
		return false;

	if(read<uint32>() != 0xFADEFACE)
		decrypt(NC_MAP);
	rpos(0);

	mapHeader.read(this);

	while(rpos() < size())
	{
		readstring<uint8>(buffer, 0xFF);
		skip();

		//this could probably be improved, table perhaps?
		if(!strcmp(buffer, "MapInfo"))
			skipSection();
		else if(!strcmp(buffer, "WallMap"))
			skipSection();
		else if(!strcmp(buffer, "FloorMap"))
			skipSection();
		else if(!strcmp(buffer, "SecretWalls"))
			skipSection();
		else if(!strcmp(buffer, "DestructableWalls"))
			skipSection();
		else if(!strcmp(buffer, "WayPoints"))
			skipSection();
		else if(!strcmp(buffer, "DebugData"))
			skipSection();
		else if(!strcmp(buffer, "WindowWalls"))
			skipSection();
		else if(!strcmp(buffer, "GroupData"))
			skipSection();
		else if(!strcmp(buffer, "ScriptObject"))
			skipSection();
		else if(!strcmp(buffer, "AmbientData"))
			skipSection();
		else if(!strcmp(buffer, "Polygons"))
			skipSection();
		else if(!strcmp(buffer, "MapIntro"))
			skipSection();
		else if(!strcmp(buffer, "ScriptData"))
			skipSection();
		else if(!strcmp(buffer, "ObjectTOC"))
			objectTOC.read(this);
		else if(!strcmp(buffer, "ObjectData"))
			readObjects();
	}

	return true;
}

void NoxMap::skipSection()
{
	size_t finish = read<int64>() + rpos(); //order matters

	rpos(finish);
}

void NoxMap::readObjects()
{
	size_t finish = read<int64>() + rpos(); //order matters
	read<uint16>(); //unknown - always 0x0001?
	while(rpos() < size())
	{
		if(objmgr.CreateObjectFromFile(this, &objectTOC) == NULL)
			break;
	}
}

NoxMapHeader::NoxMapHeader(NoxBuffer* rdr)
{
	read(rdr);
}

void NoxMapHeader::read(NoxBuffer* rdr)
{
	rdr->read((uint8*)&mapHeader, sizeof(mapHeader));

	ASSERT(mapHeader.fileid == 0xFADEFACE);
}

NoxObjectTOC::NoxObjectTOC(NoxBuffer* rdr)
{
	read(rdr);
}

void NoxObjectTOC::read(NoxBuffer* rdr)
{
	char buffer[255];
	uint16 id;

	size_t finish = rdr->read<int64>() + rdr->rpos(); //order matters
	
	rdr->read<uint16>(); //unknown
	uint16 entries = rdr->read<uint16>(); //number of entries

	while(rdr->rpos() < finish)
	{
		id = rdr->read<uint16>();
		insert(NoxObjectTOCPair(id, std::string(buffer, rdr->readstring<uint8>(buffer, 0xFF))));
	}
	
	ASSERT(size() == entries);
}