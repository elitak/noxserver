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

#ifndef MANGOS_GRIDDEFINES_H
#define MANGOS_GRIDDEFINES_H

#include "GameSystem/NGrid.h"

#include "zthread/CountedPtr.h"
#include <cmath>

// Forward class definitions
class Player;

using ZThread::CountedPtr;
typedef std::map<OBJECT_HANDLE, Player* >        PlayerMapType;

template<const unsigned int LIMIT>
struct MANGOS_DLL_DECL CoordPair
{
    CoordPair(uint32 x=0, uint32 y=0) : x_coord(x), y_coord(y) {}
    CoordPair(const CoordPair<LIMIT> &obj) : x_coord(obj.x_coord), y_coord(obj.y_coord) {}
    bool operator==(const CoordPair<LIMIT> &obj) const { return (obj.x_coord == x_coord && obj.y_coord == y_coord); }
    bool operator!=(const CoordPair<LIMIT> &obj) const { return !operator==(obj); }
    CoordPair<LIMIT>& operator=(const CoordPair<LIMIT> &obj)
    {
        this->~CoordPair<LIMIT>();
        new (this) CoordPair<LIMIT>(obj);
        return *this;
    }

    void operator<<(const uint32 val)
    {
        if( x_coord >= val )
            x_coord -= val;
    }

    void operator>>(const uint32 val)
    {
        if( x_coord+val < LIMIT )
            x_coord += val;
    }

    void operator-=(const uint32 val)
    {
        if( y_coord >= val )
            y_coord -= val;
    }

    void operator+=(const uint32 val)
    {
        if( y_coord+val < LIMIT )
            y_coord += val;
    }

    uint32 x_coord;
    uint32 y_coord;
	bool in(CoordPair leftTop, CoordPair rightBottom)
	{
		if(leftTop.x_coord <= x_coord && rightBottom.x_coord >= x_coord && leftTop.y_coord <= y_coord && rightBottom.y_coord >= y_coord)
			return true;
		else
			return false;
	}
	uint32 distance(CoordPair b)
	{
		return (uint32)sqrt( pow((double)x_coord - b.x_coord, 2) + pow((double)y_coord - b.y_coord, 2) );
	}
};

typedef CoordPair<5880> GridPair;

namespace MaNGOS
{
    template<class RET_TYPE, int CENTER_VAL>
        inline RET_TYPE Compute(float x, float y, float center_offset, float size)
    {
        // calculate and store temporary values in double format for having same result as same mySQL calcalations
        double x_offset = (double(x) - center_offset)/size;
        double y_offset = (double(y) - center_offset)/size;

        int x_val = int(x_offset+CENTER_VAL + 0.5);
        int y_val = int(y_offset+CENTER_VAL + 0.5);
        return RET_TYPE(x_val, y_val);
    }

  /*  inline GridPair ComputeGridPair(float x, float y)
    {
        return Compute<GridPair, CENTER_GRID_ID>(x, y, CENTER_GRID_OFFSET, SIZE_OF_GRIDS);
    }

    inline CellPair ComputeCellPair(float x, float y)
    {
        return Compute<CellPair, CENTER_GRID_CELL_ID>(x, y, CENTER_GRID_CELL_OFFSET, SIZE_OF_GRID_CELL);
    }*/
}
#endif
