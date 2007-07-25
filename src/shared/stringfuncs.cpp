#include "stringfuncs.h"

// This function will sort through a string and remove all whitespaces
// Moving all characters to the beginning of the string and leaving a null at the end
void strcws(char *string, bool removespaces)
{
  int len = strlen((const char*)string);
  unsigned char *retval = (unsigned char*)string;
  unsigned char *entval = (unsigned char*)string;

  if( removespaces )
  {
	for(int i=0; i<len; i++,entval++)
		(isCharw(*entval)) ? *retval = *entval,retval++ : NULL;
  }
  else
  {
	for(int i=0; i<len; i++,entval++)
		(isChar(*entval)) ? *retval = *entval,retval++ : NULL;
  }

  *retval = 0x00;
}

// This function will sort through a string and remove the defined character
// Moving all characters to the beginning of the string and leaving a null at the end
void strrc(char *string, char remove)
{
  int len = strlen((const char*)string);
  unsigned char *retval = (unsigned char*)string;
  unsigned char *entval = (unsigned char*)string;

	for(int i=0; i<len; i++,entval++)
		(*entval != remove) ? *retval = *entval,retval++ : NULL;

  *retval = 0x00;
}