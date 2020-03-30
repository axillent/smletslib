#ifndef SMCOM_STRING_H
#define SMCOM_STRING_H

#ifndef _STAVR_PLATFORM_LINUX

#include <string.h>

namespace Smcom {
  namespace String {

	//------------------------------------------------------------------------
	// helpfull functions
	//------------------------------------------------------------------------
	// --- ParseStr
	uint8_t ParseStr(char* str, char* argsv[], uint8_t argsn_max, const char* delim1, const char* delim2=NULL) {

	  uint8_t argsn = 0;
	  argsv[0] = strtok(str, delim1);

	  while( argsv[argsn] != NULL && ++argsn < argsn_max  ) {
		argsv[argsn] = strtok(NULL, (delim2==NULL)?delim1:delim2);
	  }

	  return argsn;

	}

	// --- IndexStr
	int IndexStr(const char** key_array, const char* key, uint8_t array_size) {
	  for(uint8_t i=0; i < array_size; i++) {
		if( !strcmp(key_array[i], key) ) return i;
	  }
	  return -1;
	}

	// --- Index
	template <typename T>
	  int Index(const T* key_array, T key, uint8_t array_size) {
		for(uint8_t i=0; i < array_size; i++) {
		  if( key_array[i] == key ) return i;
		}
		return -1;
	  }

	//------------------------------------------------------------------------
	// String class
	//------------------------------------------------------------------------
	template <uint16_t size>
	  class String {
	  protected:
		char	string[size];
	  public:
		//--- constuctors
		String() { string[0] = 0; }
		//--- conversion
		operator char* () { return string; }
		operator const char* () const { return string; }
        bool operator ! () const { return !string[0]; }
		//--- operators
		// =
		String& operator = (const char* str) {
		  copyStr(str);
		  return *this;
		}
		String& operator = (uint16_t data) {
		  string[0] = '\0';
		  addInt(data);
		  return *this;
		}
        bool operator == (const char* str) {
          return ! strcmp(string, str);
        }
		// +=
		String& operator += (const char* str) {
		  addStr(str);
		  return *this;
		}
		String& operator += (uint16_t data) {
		  addInt(data);
		  return *this;
		}
		// <<
		String& operator << (const char* str) {
		  addStr(str);
		  return *this;
		}
		String& operator << (uint16_t data) {
		  addInt(data);
		  return *this;
		}
		//--- functions
		void copyStr(const char* str) {
		  if( string != str ) strncpy(string, str, size);
		}
		void addStr(const char* str) {
		  if( string != str ) {
			uint8_t len = strlen(string);
			if( len < size ) strncpy(string+len, str, size-len);
		  }
		}
		void addChar(char c) {
			uint8_t len = strlen(string);
			if( len < size-1 ) {
				string[len] = c;
				string[len+1] = '\0';
			}
		}
		void addInt(uint16_t data, uint8_t digits=0) {
		  char		str[6];
		  int8_t	index;

		  for(index=0; index<5 ; index++) {
			str[4-index] = data % 10 + '0';
			data /= 10;
			if( !digits ) {
			  if( !data ) break;
			} else if( index + 1 == digits ) break;
		  }
		  str[5] = 0;
		  uint8_t len = strlen(string);
		  if( size-len > index ) strcpy(string+len, str+4-index);
		}
	  };

  }
}

#endif
#endif
