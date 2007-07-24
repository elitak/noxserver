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
	MOD_LIST list;
	MOD_OBJ obj;
	ModProperty prop;
	for(; GetString(&rdr,buff2) && strcmp(buff2,"END"); NULL)
	{
		// Object string / item
		// Object length
		// Object size
		// Object type
		// Object weight
		// Object density
		// Object radius
		// Object Diameter
		// Object polygon
		// Object surface
		// Object text


	/*	if( !buff2[0] )
			continue;
		for(strcpy(list.SectionTitle,buff2); GetString(&rdr,buff2) && strcmp(buff2,"END"); NULL)
		{
			if( !buff2[0] )
				continue;

			GetString(&rdr,buff2);
			for(strcpy(obj.ObjectName,buff2); GetString(&rdr,buff2) && strcmp(buff2,"END"); NULL)
			{
				if( !buff2[0] )
					continue;
				prop.Load(buff2);
				obj.Properties.Add(prop);
				prop.Ident = NULL;
				prop.Value = NULL;
			}
			list.ElementList.Add(obj);
			
		}*/			
	}
}

bool ModifierBin::GetString(stringstream * rdr, char * buff)
{
	if( rdr->eof() )
		return false;

	bool ret = rdr->getline(buff,255);
	strcws(buff);
	strrc(buff,';');
	return( ret );
}