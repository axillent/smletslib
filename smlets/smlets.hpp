#ifndef SMLETS_H
#define SMLETS_H

#include <smcom/byteorder.hpp>

namespace Smlets {

//-----------------------------------------------
// byte order
//-----------------------------------------------
typedef Smcom::ByteOrder::BigEndian::uint16 uint16;
typedef Smcom::ByteOrder::BigEndian::int16  int16;
typedef Smcom::ByteOrder::BigEndian::uint32 uint32;
typedef Smcom::ByteOrder::BigEndian::int32  int32;

//-----------------------------------------------
// application state
//-----------------------------------------------
namespace Application {

struct State{
	enum type {
		Unsupported    = 0,    ///< state is not supported by application
		Normal         = 1,    ///< normal state
		NoID           = 2,    ///< ID is not yet set
		WaitCmd        = 3     ///< wainting a special broadcast command
	};
};

}}

#endif
