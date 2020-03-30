#ifndef SMLETS_MSG_ID_HPP
#define SMLETS_MSG_ID_HPP

#include "../smlets.hpp"

namespace Smlets {
namespace Message {

SMCOM_DATAPACK struct ID {
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
	ID(void) : id(static_cast<uint16_t>(Unassigned) + static_cast<uint16_t>(Local)) {}
	/// initialize as copy
//	ID(const ID& id_p) : id(id_p.id) {}
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
		if( *this == ID(All) || *this == ID(All, Global) || other_id == ID(All) || other_id == ID(All, Global) ) return true;
		return false;
	}
	bool operator != (const ID& other_id) const  { return !(*this == other_id); }
	bool operator == (BroadcastID broadcast) const  {
      if( static_cast<uint16_t>(id) == static_cast<uint16_t>(broadcast) + Local ) return true;
      if( static_cast<uint16_t>(id) == static_cast<uint16_t>(broadcast) + Global ) return true;
      return false;
    }
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
    operator uint16_t () const SMCOM_DATAPACK { return id; }
};

}}


#endif
