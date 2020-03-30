#ifndef SMLETS_MSG_BASE_HPP
#define SMLETS_MSG_BASE_HPP

#include "../smlets.hpp"

namespace Smlets {
namespace Message {
  
SMCOM_DATAPACK_START

struct Type {
  enum list {
    QueueListList     = 1,
    QueueListBinary   = 2,
    QueueString2      = 3
  };
  uint8_t data;
  Type(list type_p) : data(type_p) {}
  operator list () { return static_cast<list>(data); }
};

SMCOM_DATAPACK struct MessageBase {
  Smlets::Message::ID   id;
  Type                  type;
  Smlets::uint16        sid;
  MessageBase(Type type_p) : type(type_p), sid(0) {}
};

SMCOM_DATAPACK_END

}}

#endif
