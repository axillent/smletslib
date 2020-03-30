#ifndef SMCOM_FORMAT_H
#define SMCOM_FORMAT_H

#include "string.hpp"

namespace Smcom {

  //----------------------------------------------------------------------------
  // Format class
  //----------------------------------------------------------------------------
  class FormatAccessor {
		public:
		static uint8_t Read(const uint8_t* address) {
			return *address;
		}	  
	  };
	  
  class FormatStr {
	  protected:
	  const char* ptr;
	  public:
	  FormatStr(const char* str): ptr(str) { }
	  operator const char*() const { return ptr; }
  };
	  
  template <class Driver, typename Accessor=FormatAccessor>
	class Format {
	public:
	  //------------------------------------------------------------------------
	  // static
	  //------------------------------------------------------------------------
	  static void PrintLN() { Driver::PutByte('\n'); }
	  static void PrintChar(char c) { Driver::PutByte(c); }
	  static void PrintChar(char c, uint8_t n) {
		while( n-- ) Driver::PutByte(c);
	  }
	  static void Print(const char* str) {
		while(*str) Driver::PutByte(*(uint8_t*)str++);
	  }
	  static void Print(const char* data, uint16_t n) {
		while(n--) Driver::PutByte(*data++);
	  }
	  static void Print(const FormatStr& data) {
		  const char* ptr = data;
		  for(char c; (c = (char)Accessor::Read((const uint8_t*)ptr)); ptr++)
			Driver::PutByte(c);
	  }
	  static void PrintUInt(uint16_t data, bool leading_zero=true) {
		char	str[5];
		int8_t	i;

		for(i=4; i>=0 ; i--) {
		  str[i] = data % 10;
		  data /= 10;
		}

		i = 0;
		if( leading_zero ) for(i=0; i<4 && !str[i]; i++);
		while( i < 5 ) Driver::PutByte(str[i++] + '0');
	  }
	  //------------------------------------------------------------------------
	  static void PutBlock(const uint8_t* data, uint16_t n) {
		while(n--) Driver::PutByte(*data++);
	  }
	  //------------------------------------------------------------------------
	  // operators
	  //------------------------------------------------------------------------
	  Format& operator << (const char* str) {
		Print(str);
		return *this;
	  }
	  Format& operator << (const FormatStr& str) {
		  Print(str);
		  return *this;
	  }
	  Format& operator << (uint16_t v) {
		PrintUInt(v);
		return *this;
	  }
	};

  //----------------------------------------------------------------------------
  // VT220 class
  //----------------------------------------------------------------------------
  template <class Driver>
	class VT220 : public Format<Driver> {
	public:
	  enum Color { ColorBlack=0, ColorRed, ColorGreen, ColorYellow, ColorBlue, ColorMagenta, ColorCyan, ColorWhite };
	  static void EraseLine() { Format<Driver>::Print("\033[G\x1b[2K"); }
	  static void HCursor(uint8_t x) {
		Format<Driver>::Print("\x1b[");
		Format<Driver>::PrintChar(x);
		Format<Driver>::PrintChar('G');
	  }
	  static void SGRColor(Color color) {
		Format<Driver>::Print("\x1b[3");
		Format<Driver>::PrintChar(color);
		Format<Driver>::PrintChar('m');
	  }
	  static void SGRReset() { Format<Driver>::Print("\x1b[0m"); }
	};

}

#endif
