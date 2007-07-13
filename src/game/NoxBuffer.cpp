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

#include <cstdio>

#include "ByteBuffer.h"
#include "NoxBuffer.h"

bool NoxBuffer::open(char *fileName)
{
	FILE * file;
	long int size;

	file = fopen(fileName, "rb");
	if(file == NULL)
		return false;

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	_storage.reserve(size);
	if(_storage.capacity() < size)
	{
		fclose(file);
		return false;
	}

	fseek(file, 0, SEEK_SET);
	uint8* buffer = new uint8[size];

	if(fread(buffer, 1, size, file) != size)
	{
		fclose(file);
		return false;
	}
	std::vector<uint8> v(buffer, buffer+size);
	_storage.swap(v);

	fclose(file);
	return true;
}