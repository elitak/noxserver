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

#include "object_mgr.h"
#include "NoxMap.h"

bool NoxMap::open(char *fileName)
{
	char buffer[255];
	if(!NoxBuffer::open(fileName))
		return false;
	strncpy(mappath, fileName, 512);
	getNxz();
	
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
		// we should change this to a handler parameter/variable, so that we can make this
		// into a library instead of part of the main code
		if(object_mgr::instance->create_object_from_file(this, &objectTOC) == NULL)
			break;
	}
}


void NoxMap::getNxz()
{
	memcpy(nxzpath, mappath, 512);
	char* ptr = strrchr(nxzpath, '.');
	if(ptr == NULL)
		return;
	else
	{
		ptr[1] = 'n';
		ptr[2] = 'x';
		ptr[3] = 'z';

		nxzFile = fopen(nxzpath, "rb");
		if(nxzFile)
		{
			fseek(nxzFile, 0, SEEK_END);
			nxzSize = ftell(nxzFile);
			ptr = strrchr(mappath, '\\');
			if(ptr != NULL)
				strncpy(nxzname, ptr+1, 0x50);
		}
		else
			nxzSize = 0;
	}
}
uint32 NoxMap::GetNxzSize()
{
	return nxzSize;
}

uint16 NoxMap::ReadNxzBytes(uint32 offset, uint8* buffer, uint16 size)
{
	if(nxzFile == NULL || !nxzSize)
		return 0;
	uint16 read = (size + offset) > nxzSize ? (nxzSize - offset) : size;
	fseek(nxzFile, offset, SEEK_SET);
	return fread(buffer, 1, size, nxzFile);
}
const char* NoxMap::GetNxzName()
{
	return nxzname;
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
		//sWorldLog.Log("TOC: %s\n", (*this)[id].c_str());
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
	//objmgr.AddCollisionWalls();
}
bool NoxWallMap::AddWall(NoxWall *wall)
{
	if(!insert(NoxWallPair(wall->GetPositionKey(), wall)).second)
		return false;
	//objmgr.collisionWalls.AddWall(wall->GetPosition(), wall->GetFacing());

	return true;
}
NoxWallMap::NoxWallMap()
{
	//body = new btRigidBody(0, new btDefaultMotionState(start), &shape);
}

NoxWall::NoxWall(uint8 x, uint8 y, uint8 facing, uint8 variation, uint8 minimap) : _facing(facing), _variation(variation), _minimap(minimap)
{
	SetPosition(x, y);
}
NoxWall::NoxWall(NoxBuffer *rdr)
{
	uint8 x = rdr->read<uint8>();
	uint8 y = rdr->read<uint8>();

	_facing = rdr->read<uint8>() & 0x7F; // remove the sign bit
	rdr->read<uint8>(); //matId (?)
	_variation = rdr->read<uint8>();
	_minimap = rdr->read<uint8>();
	rdr->read<uint8>(); // always null

	SetPosition(x, y);
}
NoxWall::NoxWall()
{
}

void NoxWall::create_body(uint8 x, uint8 y)
{
	float real_x = ( (x * 23) + 15) * SCALING_FACTOR;
	float real_y = ( (y * 23) + 15) * SCALING_FACTOR;

	b2Body* body = object_mgr::instance->get_walls_body();
	b2PolygonDef polygonDef;

	const float width = 2 * SCALING_FACTOR;
	const float height = 8 * SCALING_FACTOR;

	float d = sqrt(height*height/2);

//#define EAST_WALL polygonDef.SetAsBox(height, width, b2Vec2(real_x+(height/2), real_y), 0.25f*b2_pi);
//#define WEST_WALL polygonDef.SetAsBox(height, width, b2Vec2(real_x-(height/2), real_y), 0.25f*b2_pi);
//#define NORTH_WALL polygonDef.SetAsBox(width, height, b2Vec2(real_x, real_y-(height/2)), 0.25f*b2_pi);
//#define SOUTH_WALL polygonDef.SetAsBox(width, height, b2Vec2(real_x, real_y+(height/2)), 0.25f*b2_pi);

#define EAST_WALL polygonDef.SetAsBox(height, width, b2Vec2(real_x+d, real_y+d), 0.25f*b2_pi);
#define WEST_WALL polygonDef.SetAsBox(height, width, b2Vec2(real_x-d, real_y-d), 0.25f*b2_pi);
#define SOUTH_WALL polygonDef.SetAsBox(width, height, b2Vec2(real_x+d, real_y-d), 0.25f*b2_pi);
#define NORTH_WALL polygonDef.SetAsBox(width, height, b2Vec2(real_x-d, real_y+d), 0.25f*b2_pi);

	switch(_facing)
	{
		case WALL_CROSS:
			//east
			EAST_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));

			//west
			WEST_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));
		case WALL_NORTH:
			//north
			NORTH_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));

			//south
			SOUTH_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));
		break;

		case WALL_WEST:
			//east
			EAST_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));

			//west
			WEST_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));
		break;

		case WALL_SOUTH_T:
			//north
			NORTH_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));
		case WALL_SW_CORNER:
			//west
			WEST_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));

			//south
			SOUTH_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));
		break;

		case WALL_WEST_T:
			EAST_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));
		case WALL_SE_CORNER:
			//west
			WEST_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));

			//north
			NORTH_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));
		break;

		case WALL_NORTH_T:
			//south
			SOUTH_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));
		case WALL_NE_CORNER:
			//north
			NORTH_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));
			//east
			EAST_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));
		break;

		case WALL_EAST_T:
			//west
			WEST_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));
		case WALL_NW_CORNER:
			//east
			EAST_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));
			//south
			SOUTH_WALL
			m_shapes.insert(body->CreateShape(&polygonDef));
		break;
		default:
			break;
	}

	b2FilterData filter;
	filter.categoryBits = 0x0004;
	filter.maskBits = 0xFFFF;
	BOOST_FOREACH(b2Shape* s, m_shapes)
	{
		s->SetFilterData(filter);
	}
}
void NoxWall::destroy_body()
{
	BOOST_FOREACH(b2Shape* s, m_shapes)
	{
		object_mgr::instance->get_walls_body()->DestroyShape(s);
	}

	m_shapes.clear();
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