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

#ifndef _NOXMAP_H
#define _NOXMAP_H

#include "global.h"
#include "NoxBuffer.h"
#include <map>

class NoxMap;

typedef std::pair<uint16, std::string> NoxObjectTOCPair;

class NoxObjectTOC : public std::map<uint16, std::string>
{
	friend NoxMap;
public:
protected:
	NoxObjectTOC() {};
	NoxObjectTOC(NoxBuffer* rdr);

	void read (NoxBuffer* rdr);
};

enum NoxWallFacingType
{
	WALL_NORTH,//same as SOUTH
	WALL_WEST,//same as EAST
	WALL_CROSS,
				
	WALL_SOUTH_T,
	WALL_EAST_T,//4
	WALL_NORTH_T,
	WALL_WEST_T,

	WALL_SW_CORNER,
	WALL_NW_CORNER,//8
	WALL_NE_CORNER,
	WALL_SE_CORNER//10
};
enum NoxWallFacingFlagType
{
	WALL_NORTH_FLAG = 0x01,
	WALL_WEST_FLAG = 0x02,
	WALL_SOUTH_FLAG = 0x04,
	WALL_EAST_FLAG = 0x08,
	WALL_SOUTH_T_FLAG,
	WALL_EAST_T_FLAG,//4
	WALL_NORTH_T_FLAG,
	WALL_WEST_T_FLAG,

	WALL_SW_CORNER_FLAG = 0x06,
	WALL_NW_CORNER_FLAG = 0x03,//8
	WALL_NE_CORNER_FLAG = 0x09,
	WALL_SE_CORNER_FLAG = 0x0C//10
};


union NoxWallPosition
{
	struct
	{
		uint8 x;
		uint8 y;
	} coords;
	uint16 key;
};

class NoxWall
{
public:
	NoxWall(NoxWallPosition position, uint8 facing, uint8 variation, uint8 minimap = 100);
	NoxWall(NoxBuffer* rdr);

	uint16 GetPositionKey() { return _position.key; }
	boost::numeric::ublas::vector<uint8> GetPosition() 
	{
		boost::numeric::ublas::vector<uint8> v(2);
		v[0] = _position.coords.x;
		v[1] = _position.coords.y;
		return v;
	}
	void SetPositionX(uint8 x) { _position.coords.x = x;}
	void SetPositionY(uint8 y) { _position.coords.y = y;}

	NoxWallFacingType GetFacing() { return (NoxWallFacingType)_facing; }
protected:
	NoxWall();
	NoxWallPosition _position;
	uint8 _facing;
	uint8 _variation; //maybe only store ptr to thing.bin
	uint8 _minimap;
};

typedef std::pair<uint16, NoxWall*> NoxWallPair;

class NoxWallMap : public std::map<uint16, NoxWall*>
{
	friend NoxMap;
public:
	bool AddWall(NoxWall* wall);
	NoxWall* GetWall(NoxWallPosition pos) 
	{ 
		iterator iter = find(pos.key);
		if(iter == end())
			return NULL;
		else
			return iter->second; 
	}
	NoxWall* GetWall(uint8 x, uint8 y)
	{
		NoxWallPosition wPos;
		wPos.coords.x = (uint8)(x / 23);
		wPos.coords.y = (uint8)(y / 23);
		return GetWall(wPos);
	}
protected:
	NoxWallMap(NoxBuffer* rdr);
	NoxWallMap();

	void read (NoxBuffer* rdr);
};
struct NoxMapHeaderType
{
	uint32 fileid;
	uint32 unk1;
	uint32 checksum;
	uint32 unk2;
	uint32 unk3;
	uint32 unk4;
};

class NoxMapHeader
{
	friend NoxMap;
public:
	uint32 GetChecksum() { return mapHeader.checksum; }
protected:
	NoxMapHeader() {};
	NoxMapHeader(NoxBuffer* rdr);

	void read (NoxBuffer* rdr);
private:
	NoxMapHeaderType mapHeader;
};



class NoxMap : NoxBuffer
{
public:
	NoxMap() 
	{
		nxzFile = 0;
		nxzSize = 0;
	};
	bool open(char* fileName);
	NoxWallMap walls;

	uint32 GetNxzSize();
	uint16 ReadNxzBytes(uint32 offset, uint8* buffer, uint16 size);
	const char* GetNxzName(); //will always be a pointer to a 0x50 buffer

	uint32 GetChecksum() { return mapHeader.GetChecksum(); }

protected:
	NoxObjectTOC objectTOC;
	NoxMapHeader mapHeader;

	char mappath[512];
	char nxzpath[512];
	char nxzname[0x50]; // don't change this size, used by packet to send map
	FILE * nxzFile;
	uint32 nxzSize;

	void getNxz();
	void skipSection();
	void readObjects();
};

#endif