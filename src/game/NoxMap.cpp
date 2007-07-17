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
#include "WorldLog.h"
#include "ObjectMgr.h"

#include "flatland/flatland.hpp"

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
			walls.read(this);
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
		sWorldLog.Log("TOC: %s\n", (*this)[id].c_str());
	}
	
	ASSERT(size() == entries);
}

NoxWallMap::NoxWallMap(NoxBuffer* rdr)
{
	read(rdr);
}
void NoxWallMap::read(NoxBuffer* rdr)
{
	size_t finish = rdr->read<int64>() + rdr->rpos(); //order matters
	rdr->read<uint16>();
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();
	rdr->read<uint32>();

	while(rdr->read<uint8>() != 0xFF && rdr->read<uint8>() != 0xFF)
	{
		rdr->rpos(rdr->rpos() - 2); // go back two to accomodate above check
		NoxWall* wall = new NoxWall(rdr);
		AddWall(wall);
	}
	rdr->rpos(finish);	
	objmgr.AddCollisionWalls();
}
bool NoxWallMap::AddWall(NoxWall *wall)
{
	if(!insert(NoxWallPair(wall->GetPositionKey(), wall)).second)
		return false;
	objmgr.collisionWalls.AddWall(wall->GetPosition(), wall->GetFacing());

	return true;
}
NoxWallMap::NoxWallMap()
{
	//body = new btRigidBody(0, new btDefaultMotionState(start), &shape);
}

NoxWall::NoxWall(NoxWallPosition position, uint8 facing, uint8 variation, uint8 minimap) : _position(position), _facing(facing), _variation(variation), _minimap(minimap)
{
}
NoxWall::NoxWall(NoxBuffer *rdr)
{
	SetPositionX(rdr->read<uint8>());
	SetPositionY(rdr->read<uint8>());

	_facing = rdr->read<uint8>() & 0x7F; // remove the sign bit
	rdr->read<uint8>(); //matId (?)
	_variation = rdr->read<uint8>();
	_minimap = rdr->read<uint8>();
	rdr->read<uint8>(); // always null
}
NoxWall::NoxWall()
{
}

/*void AddBoxFromFlag(uint8 flag, btCompoundShape* shape)
{
	btBoxShape* box;
	btTransform trans;
	trans.setIdentity();
	const int width = 8;
	const int length = 15;
	switch(flag)
	{
		case WALL_NORTH_FLAG:
			box = new btBoxShape(btVector3(width, length, 1));
			trans.setOrigin(btVector3(0, -(length/2), 0));
		break;
		case WALL_SOUTH_FLAG:
			box = new btBoxShape(btVector3(width, length, 1));
			trans.setOrigin(btVector3(0, (length/2), 0));
		break;
		case WALL_EAST_FLAG:
			box = new btBoxShape(btVector3(length, width, 1));
			trans.setOrigin(btVector3((length/2), 0, 0));
		break;
		case WALL_WEST_FLAG:
			box = new btBoxShape(btVector3(length, width, 1));
			trans.setOrigin(btVector3(-(length/2), 0, 0));
		break;
	};
	shape->addChildShape(trans, box);
}
*/
/*btCollisionShape* NoxWall::FacingToNewShape(uint8 facing)
{
	btCompoundShape* shape = new btCompoundShape();
	switch(facing)
	{
		case WALL_NORTH:
			AddBoxFromFlag(WALL_NORTH_FLAG, shape);
			AddBoxFromFlag(WALL_SOUTH_FLAG, shape);
		break;
		case WALL_WEST:
			AddBoxFromFlag(WALL_EAST_FLAG, shape);
			AddBoxFromFlag(WALL_WEST_FLAG, shape);
		break;
		case WALL_SW_CORNER:
			AddBoxFromFlag(WALL_WEST_FLAG, shape);
			AddBoxFromFlag(WALL_SOUTH_FLAG, shape);
		break;
		case WALL_NW_CORNER:
			AddBoxFromFlag(WALL_WEST_FLAG, shape);
			AddBoxFromFlag(WALL_NORTH_FLAG, shape);
		break;
		case WALL_NE_CORNER:
			AddBoxFromFlag(WALL_NORTH_FLAG, shape);
			AddBoxFromFlag(WALL_EAST_FLAG, shape);
		break;
		case WALL_SE_CORNER:
			AddBoxFromFlag(WALL_EAST_FLAG, shape);
			AddBoxFromFlag(WALL_SOUTH_FLAG, shape);
		break;
		default:
			AddBoxFromFlag(WALL_NORTH_FLAG, shape);
			break;
	}
	return shape;
}*/
/*
bool NoxWall::SpaceOpen(GridPair pos, size_t radius)
{
	CoordPair<23> rel(pos.x_coord % 23, pos.y_coord % 23);
	uint8 table[23][23] = 
	{
		0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
		0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
		0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
		0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
		0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08
	};

	switch(GetFacing())
	{
		case WALL_NORTH:
			//if(table[rel.x_coord][rel.y_coord] & WALL_NORTH_FLAG)
			//	return false;
			for(uint8 x = 0; x < 23; x++)
				for(uint8 y = 0; y < 23; y++)
					if(table[x][y] & (WALL_NORTH_FLAG | WALL_SOUTH_FLAG))
						if(rel.distance(CoordPair<23>(x, y)) < radius)
							return false;
		break;
		case WALL_WEST:
			//if(table[rel.x_coord][rel.y_coord] & WALL_WEST_FLAG)
			//	return false;
			for(uint8 x = 0; x < 23; x++)
				for(uint8 y = 0; y < 23; y++)
					if(table[x][y] & (WALL_WEST_FLAG|WALL_EAST_FLAG))
						if(rel.distance(CoordPair<23>(x, y)) < radius)
							return false;
		break;
		case WALL_CROSS:
		break;
					
		case WALL_SOUTH_T:
		break;
		case WALL_EAST_T:
		break;
		case WALL_NORTH_T:
		break;
		case WALL_WEST_T:
		break;

		case WALL_SW_CORNER:
			for(uint8 x = 0; x < 23; x++)
				for(uint8 y = 0; y < 23; y++)
					if(table[x][y] & WALL_SW_CORNER_FLAG)
						if(rel.distance(CoordPair<23>(x, y)) < radius)
							return false;
		break;
		case WALL_NW_CORNER:
			for(uint8 x = 0; x < 23; x++)
				for(uint8 y = 0; y < 23; y++)
					if(table[x][y] & WALL_NW_CORNER_FLAG)
						if(rel.distance(CoordPair<23>(x, y)) < radius)
							return false;
		break;
		case WALL_NE_CORNER:
			for(uint8 x = 0; x < 23; x++)
				for(uint8 y = 0; y < 23; y++)
					if(table[x][y] & WALL_NE_CORNER_FLAG)
						if(rel.distance(CoordPair<23>(x, y)) < radius)
							return false;
		break;
		case WALL_SE_CORNER:
			for(uint8 x = 0; x < 23; x++)
				for(uint8 y = 0; y < 23; y++)
					if(table[x][y] & WALL_SE_CORNER_FLAG)
						if(rel.distance(CoordPair<23>(x, y)) < radius)
							return false;
		break;
	}
	return true;
}*/