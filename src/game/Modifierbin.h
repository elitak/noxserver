#ifndef H_MODBIN
#define H_MODBIN

#include <fstream>
#include "NoxThingClassDefs.h"
#include "Policies/Singleton.h"
#include "NoxCrypt.h"
#include "stringfuncs.h"
#include "noxbuffer.h"

using namespace std;


struct ModProperty
{
public:
  char* Ident;
  char* Value;

  void Load(char string[])
  {
    // Splitting string	
	char* Ident = strtok(string, " =");
	char* ValueParam = strtok(NULL, " =");
	
	//Load values
	LoadValue(Ident,ValueParam);
  }
  ModProperty()
  {
	memset((void*)this,0x00,sizeof(*this));
  }
  ~ModProperty()
  {
	if( Ident )
		delete [] Ident;
	if( Value )
		delete [] Value;
  }
private:

  void LoadValue(char Identifier[], char ValueParam[])
  {
	int IDlen = strlen(Identifier)+1;
	int ValLen = strlen(ValueParam)+1;
	if( IDlen )
		Ident = new char [IDlen];
	if( ValLen )
		Value = new char [ValLen];

	strcpy(Ident,Identifier);
	strcpy(Value,ValueParam);
  }
};

struct MOD_OBJ
{
private:
public:
char ObjectName[0x80];
MLIST<ModProperty> Properties;

};
struct MOD_LIST
{
private:
public:

char SectionTitle[0x80];
MLIST<MOD_OBJ> ElementList;

};
class ModifierBin
{

private:
	void Load_Modifier(char* buff, long len);
    bool GetString(stringstream * rdr, char * buff);
public:
	MLIST<MOD_LIST> ModifierList;

	bool LoadBin(char filepath[])
	{
		fstream ifile;
		ifile.open(filepath,ios::in | ios::binary);
		if( !ifile.is_open())
			return false;

		long count=0;
		ifile.seekg(0,ios::end);
		long len = ifile.tellg();
		ifile.seekg(0,ios::beg);
		unsigned char *buff = NULL;

		buff = new unsigned char [len+10];
		memset(buff,0x00,len+10);

		ifile.read((char*)buff,len);
		NoxCrypt::decrypt(buff, len, NC_MODIFIER_BIN);
		ifile.close();

		Load_Modifier((char *)buff,len);

		if( buff )
			delete [] buff;
		return(true);
	}
};

#endif