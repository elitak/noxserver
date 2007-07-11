/* 
 * Copyright (C) 2005,2006,2007 MaNGOS <http://www.mangosproject.org/>
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

/// \addtogroup u2w
/// @{
/// \file

#ifndef __NAMETABLES_H
#define __NAMETABLES_H

#include "Common.h"
#include "Opcodes.h"

#define INVALID_ID 0xFF

/// Lookup opcode name for human understandable logging
inline const char* LookupName(uint32 id, const char* table[])
{
    /* slow code
    for(uint32 i = 0; table[i].name != 0; i++)
    {
        if (table[i].id == id)
            return table[i].name;
    }

    return "UNKNOWN";*/
    //fast code
    if (id > MAX_OPCODE_ID) 
        return "UNKNOWN OPCODE_CODE received, it is more than max!";
    return table[id];
}

inline const uint8 LookupId(const char* name, const char* table[])
{
	for(uint8 i = 0; table[i] != NULL; i++)
	{
		if (strcmp(table[i], name))
			return i;
	}
	return INVALID_ID;
}

extern const char* g_worldOpcodeNames[];
extern const char* g_spellNames[];
extern const char* g_abilityNames[];
#endif
/// @}
