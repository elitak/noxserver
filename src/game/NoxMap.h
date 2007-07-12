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
#include "NoxBuffer.h"

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
	bool open(char* fileName);

protected:
	NoxObjectTOC objectTOC;
	NoxMapHeader mapHeader;

	void skipSection();
	void readObjects();
};

#endif