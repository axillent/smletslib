#ifndef SMLETS_MSG_QUEUE_HPP
#define SMLETS_MSG_QUEUE_HPP

#include <stdlib.h>
#include <string.h>

#include "msg-id.hpp"
#include "msg-base.hpp"

#include <smcom/common/itoa.hpp>
#include <smcom/common/string.hpp>

namespace Smlets {
  namespace Message {
    namespace Queue {

SMCOM_DATAPACK_START

SMCOM_DATAPACK struct String2 : public MessageBase {
  char  key[16];
  char  value[16];
  String2() : MessageBase(Smlets::Message::Type::QueueString2) {
    key[0] = value[0] = 0;
  }
  String2(const char* key_p, const char* value_p) : MessageBase(Smlets::Message::Type::QueueString2) {
    strncpy(key, key_p, sizeof(key));
    strncpy(value, value_p, sizeof(value));
  }
  String2(const char* key_p, int16_t value_p, uint8_t dot=0) : MessageBase(Smlets::Message::Type::QueueString2) {
    strncpy(key, key_p, sizeof(key));
    itoa(value_p, value);
    if( dot ) {
      uint8_t l = strlen(value);
      memcpy(value + l, value + l - dot, dot);
      value[l-dot] = '.';
      value[l+dot] = 0;
    }
  }
};

SMCOM_DATAPACK_END

template <class UartDriver>
class Stream {
protected:
  enum Stat { Empty, SearchKey, ScanKey, KeyReady, SearchValue, ScanValue, Done } stat;
  Smcom::String::String<16> in_key, in_value;
  bool                      binary;
public:
  Stream() {
      in_clr();
  }
  void in_clr() {
    stat = Empty;
  }
  void out(const String2& msg) {
    UartDriver::Print("{\"id\":");
    UartDriver::PrintUInt(msg.id);
    UartDriver::Print(",\"");
    UartDriver::Print(msg.key);
    UartDriver::Print("\":\"");
    UartDriver::Print(msg.value);    
    UartDriver::Print("\",\"sid\":");
    UartDriver::PrintUInt(msg.sid);
    UartDriver::Print("}\n");
  }
  bool in(String2& msg) {
    if( stat != Done ) {
      while( UartDriver::RXAvailable() ) {
        uint8_t byte = UartDriver::GetByte();
        
        // --- look for end
        switch( byte ) {
        case '\n':
          stat = ( !in_key && !in_value ) ? Done : Empty;
          break;
        default:
          switch( stat ) {
          case Empty: 
            if( byte == '{' ) stat = SearchKey;
            in_key = "";
            in_value = "";
            break;
          case SearchKey:
            switch( byte ) {
            case '}':
              break;
            case '"':
              stat = ScanKey;
              break;
            }
            break;
          case ScanKey:
            if( byte == '"' ) stat = KeyReady;
            else in_key.addChar(byte);
            break;
          case KeyReady:
            if( byte == ':' ) stat = SearchValue;
            break;
          case SearchValue:
            if( byte == '"' ) {
              stat = ScanValue;
              binary = false;
            } else if( strchr("-0123456789", byte) != NULL ) {
              stat = ScanValue;
              binary = true;
              in_value.addChar(byte);
            }
            break;
          case ScanValue:
            if( (!binary && byte == '"') || (binary && (strchr("0123456789", byte) == NULL)) ) {
              if( in_key == "id" ) {
                msg.id = atoi(in_value);
              } else {
                strncpy(msg.key, in_key, sizeof(msg.key));
                strncpy(msg.value, in_value, sizeof(msg.value));
              }
              stat = SearchKey;
              in_key = "";
              in_value = "";
            } else in_value.addChar(byte);
            break;
          }
          break;
        }
      }
    }
    return stat == Done;
  }
};

}}}


#endif
