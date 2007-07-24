#include "Modifierbin.h"
#include <iostream>

#define END "END"
/*		DESC = modifier.db:ArcherArrowDesc;
		RANGE = 200.0;
		DAMAGE_MIN = 10;
		DAMAGE_COEFFICIENT = 1.481;
		COLOR1 = 165 85 5 (main case);
		COLOR2 = 253 188 41 (case tips);
		COLOR3 = 41 190 253 (ties);
		EFFECTIVENESS = COLOR3;
		PRIMARYENCHANTMENT = COLOR1;
		SECONDARYENCHANTMENT = COLOR2;
		DURABILITY = 50;
		CLASSUSE = CONJURER;
		REQUIRED_STRENGTH = 20;*/
void ModifierBin::Load_Modifier(char* buff, long len)
{
	string st = buff;
	stringstream rdr(st,ios::in);

	char buff2[255];
	for(; !rdr.eof() && rdr.getline(buff2,255); NULL)
	{
		strcws(buff2);
		strrc(buff2,';');
		if( buff2[0] )
		{



		}
	}
}