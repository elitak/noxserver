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

#ifndef _NOXBUFFER_H
#define _NOXBUFFER_H

#include "ByteBuffer.h"
#include "Common.h"
#include "NoxCrypt.h"

struct ColorType
{
	uint8 red;
	uint8 green;
	uint8 blue;
};

typedef uint8 NoxUserColorType; // number should be between 1 - 33, inclusive

class NoxBuffer : public ByteBuffer
{
public:
	bool open (char* fileName);

	template <typename T> size_t readstring (char* string, size_t maxlength)
	{
		size_t length = read<T>();
		size_t extra = 0;
		maxlength--;
		if(rpos() >= size())
			length = 0;

		if(length > maxlength)
		{
			read((uint8*) string, maxlength);
			rpos(rpos() + length - maxlength);
			length = maxlength;
		}
		else
			read((uint8*) string, length);

		string[length] = 0;
		return length;
	}
	template <typename T> size_t readstring (wchar_t* string, size_t maxlength); //length is in chars, not bytes

	int encrypt(NoxCryptFileTypes fileType)
	{
		return NoxCrypt::encrypt( &_storage[0], size(), fileType);
	}

	int decrypt(NoxCryptFileTypes fileType)
	{
		return NoxCrypt::decrypt( &_storage[0], size(), fileType);
	}

	/// Skips to the next qword (8 byte) boundary and returns the number of bytes skipped
	/// does not skip any bytes if already on a qword boundary.
	size_t skip ()
	{
		size_t skip = (size_t) (8 - rpos() % 8) % 8;
		rpos(rpos() + skip);
		return skip;
	}
};

#endif