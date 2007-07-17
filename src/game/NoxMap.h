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

#include "Common.h"
#include "GridDefines.h"
#include "NoxBuffer.h"

/// Collision
#include "flatland/flatland.hpp"
#include "flatland/shapes.hpp"

class NoxMap;

typedef std::pair<uint16, std::string> NoxObjectTOCPair;

class NoxObjectTOC : public HM_NAMESPACE::hash_map<uint16, std::string>
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
	CoordPair<255> GetPosition() { return CoordPair<255>(_position.coords.x, _position.coords.y); }
	void SetPositionX(uint8 x) { _position.coords.x = x;}
	void SetPositionY(uint8 y) { _position.coords.y = y;}

	NoxWallFacingType GetFacing() { return (NoxWallFacingType)_facing; }
	//static btCollisionShape* FacingToNewShape(uint8 facing);
protected:
	NoxWall();
	NoxWallPosition _position;
	uint8 _facing;
	uint8 _variation; //maybe only store ptr to thing.bin
	uint8 _minimap;
};

typedef std::pair<uint16, NoxWall*> NoxWallPair;

class NoxWallMap : public HM_NAMESPACE::hash_map<uint16, NoxWall*>
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
	NoxWall* GetWall(GridPair pos)
	{
		NoxWallPosition wPos;
		wPos.coords.x = (uint8)(pos.x_coord / 23);
		wPos.coords.y = (uint8)(pos.y_coord / 23);
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
	NoxMap() {};
	bool open(char* fileName);
	NoxWallMap walls;

protected:
	NoxObjectTOC objectTOC;
	NoxMapHeader mapHeader;

	void skipSection();
	void readObjects();
};

#endif