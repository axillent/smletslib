#ifndef SMLETS_MSG_BASIC_H
#define SMLETS_MSG_BASIC_H

#include <string.h>

#include <smcom/common/crc.hpp>
#include "../smlets.hpp"

namespace Smlets {
namespace Message {

const int message_version = 0;

SMCOM_DATAPACK_START

/*-------------------------------------------------------------------------------------------------------------------
 *       Smartlets network is 16 bits address space organized as 4096 segments each with 16 devices
 *       0x000x - 0xFDFx - node address
 *       0xFE0x - 0xFEFx - local broadcast address (256 count)
 *       0xFF0x - 0xFFFx - global broadcast address (256 count)
    -------------------------------------------------------------------------------------------------------------------*/
struct ID {
protected:
	// --- data
	/// device id
    Smlets::uint16	id;
public:
	// --- listings
	enum offset {
		Local = 0xFE00,
		Global = 0xFF00
	};
	/// ApplicationID
	enum BroadcastID {
		// ---------------------------------------------------------------------
		// local applications/broadcasts 0xFE00 - 0xFEFF
		// global applications/broadcasts 0xFF00 - 0xFFFF, global = ll + 0xFF
		//
		Unassigned 		= 0,	///< special ID for unassigned devices to be used to request/get a new ID
		All				= 1,	// this should be accepted by any device
		WaitCommand		= 2,	// this should be accepted by any device in WAITCMD state (see feature_basic)
		NetBasic		= 3,	// network basic staff
		//
		LightID			= 50,	// lighting scene application
		ValueID			= 51,	///< broadcast sharing value with ID
		RelayID			= 52,
		OptionID		= 53,
		LocalTime		= 54,
		//
		Wrong			= 255
	};
	// --- methods
	// --- constructors
	/// initialize as unsigned
	ID(void) : id(Unassigned) {}
	/// initialize as copy
	ID(const ID& id_p) : id(id_p.id) {}
	/// initialize as application local or global
	ID(BroadcastID br_id, offset of) : id(static_cast<uint16_t>(br_id) + static_cast<uint16_t>(of)) {}
	/// initialize as net segment (0-4095) + net device id (0-15)
	ID(uint16_t segment, uint8_t dev_id) : id(((segment & 0xfff) << 4) | (dev_id & 0xf)) {}
	/// initialize as 16bit id
	ID(uint16_t id_p) : id(id_p) {}
	// --- comparison
	bool operator == (const ID& other_id) const  {
		if( id == other_id.id ) return true;
		if( IsBroadcast() ) {
			if( other_id.IsBroadcastGlobal() && (static_cast<uint16_t>(id) == static_cast<uint16_t>(other_id.id) - Global + Local) ) return true;
			if( IsBroadcastGlobal() && (static_cast<uint16_t>(other_id.id) == static_cast<uint16_t>(id) - Global + Local) ) return true;
		}
//		if( *this == ID(All) || *this == ID(All, Global) || other_id == ID(All) || other_id == ID(All, Global) ) return true;
		return false;
	}
	bool operator != (const ID& other_id) const  { return !(*this == other_id); }
	// --- application checks
	bool IsBroadcast() const { return id >= (uint16_t)Local; }						///< is it an application ID? local or global
	bool IsBroadcastGlobal() const { return id >= (uint16_t)Global; }				///< is it a global application ID?
	bool IsBroadcastLocal() const { return IsBroadcast() && !IsBroadcastGlobal(); }	///< is it a local application ID
    bool IsUnassigned() const { return static_cast<uint16_t>(id) == Unassigned; }
	// --- other methods
	uint16_t GetNetSegment() const { return id >> 4; }
	uint8_t	GetNetDevID() const { return id & 0xf; }
	BroadcastID GetBroadcastID() const { return static_cast<BroadcastID>((id > Global)?id-Global:((id > Local)?id-Local:Wrong)); }
    // --- conversion
    operator uint16_t () const { return id; }
};

struct Type {
protected:
	// --- data
	Smlets::uint16	type;
public:
	enum TypeList {
		// ---------------------------------------------------------------------
		// basic protocol message types
		None 				= 0,	// none type, actually this is mask to match none types
		All 				= 1,	// all types, actually this is mask to match all types
		Ping				= 2,	// bc:NetBasic, ping request
		IDRequest			= 3,	// bc:NetBasic, request ID
		IDAssign			= 4,	// reply/assign ID
		AckReply			= 5,	// acknoledgment reply
		RouteAnnounce		= 6,	// bc:NetBasic, announce routing to IP bridge
        Note                = 7,    // bc:NetBasic, present device HWID, soft version
        NotePing            = 8,    // bc:NetBasic, request Note reply
		// ---------------------------------------------------------------------
		// application message types
		Relay16ch			= 1000,	///< bc:RelayID, 16 channel relay
		LightScene4			= 1020,	///< bc:LightID, Light Scene with pwm and id
		ValueInt16			= 1030,	///< bc:ValueID, Value with ID
		ValueInt32			= 1031,	///< bc:ValueID, Value with ID
		ValueInt16Div10		= 1032,	///< bc:ValueID, Value with ID
		OptionNumRequest	= 1050,	///< bc:OptionID, request number of options
		OptionNumGet		= 1051,	///< bc:OptionID, get reply with number of option
		OptionRequest		= 1052,	///< bc:OptionID, request option by index
		OptionInt16Get		= 1053,	///< bc:OptionID, get reply to index request with int16
		OptionInt16Set		= 1054,	///< bc:OptionID, set by index int16
		OptionInt32Get		= 1055,	///< bc:OptionID, get reply to index request with int16
		OptionInt32Set		= 1056,	///< bc:OptionID, set by index int16
		LocalTimeRequest	= 1100, ///< bc:LocalTime, request local time
		LocalTime			= 1101,	///< bc:LocalTime, reply with local time
		// ---------------------------------------------------------------------
		// custom messages types
		Custom1    =  32768,// custom message 1
		Custom2,			// custom message 2
		Custom3,			// custom message 3
		Custom4,			// custom message 4
		Custom5,			// custom message 5
		Custom6,			// custom message 6
		Custom7,			// custom message 7
		Custom8,			// custom message 8
		Custom9,			// custom message 9
		Custom10,			// custom message 10
		Custom11,			// custom message 11
		Custom12,			// custom message 12
		Custom13,			// custom message 13
		Custom14,			// custom message 14
		Custom15,			// custom message 15
		Custom16,			// custom message 16
		CustomMax   = Custom1 + 128	// custom message max
	};
	// --- methods
	Type() {}
	Type(TypeList t) : type(t) {}
	Type(uint16_t t) : type(t) {}
	operator uint16_t() const { return type; }
//	Type& operator = (uint16_t p) {
//		type = p;
//		return *this;
//	}
};

//------------------------------------------------------------------------------
// Standard message
//------------------------------------------------------------------------------
//struct Data {};

struct Attributes {
  // --- data
  uint8_t	version 	: 3;	// message version
  bool	    ack 		: 1;	// set this to 1 to receive reply on message received
  bool	    session		: 1;	// session start from master or session end from slave (for M&S networks)
  bool	    endian		: 1;	// data part is 0/false - big endian 1/true - little
  // ---  methods
  Attributes() : version(Smlets::Message::message_version), ack(false), session(false), endian(false) {}
//  operator uint8_t& () {
//    return static_cast<uint8_t&>(*this);
//  }
  operator uint8_t () const { 
    return *((uint8_t*)this);
  }
  Attributes& operator=(const uint8_t p) {
    *(uint8_t*)this = p;
    return *this;
  }
};

// --- data header
struct Header {
	// --- message data
	uint8_t             size;			// message data length in bytes excluding header
	Attributes          attributes;
	ID			        rx;				// id of device this message is sending to
	ID			        tx;				// id of device this message is sended from
	Type		        type;			// type of the message
	Smlets::uint16      sid;       		// message sequence number
	// --- methods
	Header() : size(0), attributes(), sid(0) {}
};

SMCOM_DATAPACK struct MessageBase {
    // --- data
    Header header;
//    Data   data;
	// --- methods
	MessageBase () : header() {}
	MessageBase(const MessageBase& msg) {
		memcpy(this, &msg, msg.getSize());
	}
	MessageBase& operator = (const MessageBase& msg) {
		memcpy(this, &msg, msg.getSize());
		return *this;
	}
	const uint8_t* Data() const {
		return ((const uint8_t*)this) + sizeof(header);
	}
	uint8_t* Data() {
		return ((uint8_t*)this) + sizeof(header);
	}
	void copyData(const MessageBase& msg) {
		memcpy(Data(), msg.Data(), msg.getDataSize());
	}
    uint8_t& operator[](uint8_t i) {
      return ((uint8_t*)this)[i]; 
    }
    const uint8_t& operator[](uint8_t i) const { 
      return ((uint8_t*)this)[i]; 
    }
	uint8_t getSize() const { 
      return sizeof(header) +  header.size; 
    }
	uint8_t getDataSize() const {
      return header.size;
    }
	uint8_t CRC8() const { 
      return crc8((uint8_t*)this, getSize()); 
    }
};

template <int data_size>
struct DataArray {
    // --- data
	uint8_t	data[data_size];
    // --- methods
	const uint8_t& operator[](uint8_t i) const { 
      return data[i]; 
    }
	uint8_t& operator[](uint8_t i) { 
      return data[i]; 
    }
	operator void*() { 
      return data; 
    }
//	DataArray& operator =(const Data& data_p) {
//		memcpy(data, &data_p, data_size);
//		return *this;
//	}
};

template <int payload_size>
struct Message : public MessageBase {
	// --- static
	static const int data_maxsize = payload_size - sizeof(header);
	// --- message data
	DataArray<data_maxsize>	data;
    // --- methods
    Message() : MessageBase() {}
    Message(const MessageBase& msg) : MessageBase(msg) {}
};

SMCOM_DATAPACK_END

}}


#endif
