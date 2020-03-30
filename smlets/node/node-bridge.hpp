/*
SMARTLETS BRIDGE APPLICATION

Add local and global interfaces and optional application

local is a network we want to bridge to global network
local network is described by bridge node ID as a start segment and by number of segments set by SetSegments()

axillent@gmail.com
 */

#ifndef _SMARTLETSP_NODE_BRIDGE_H
#define _SMARTLETSP_NODE_BRIDGE_H

#include <stavrp/common/buffer.hpp>
#include <stavrp/softdrv/timer.hpp>

#include "appl/appl_basic.hpp"

namespace Smartlets {
namespace Node {

//----------------------------------------------------------------------
// Definition
//----------------------------------------------------------------------
template <class ConfigIn, class InterfaceLocal, class InterfaceGlobal, template <class Node> class ApplTemplate=Appl::Standard >
class Bridge {
public:
	// --- public definitions
	typedef ConfigIn								Config;
	typedef typename ConfigIn::Defaults				Defaults;
	typedef	typename ConfigIn::Defaults::Message	Message;
    typedef ApplTemplate<Bridge>         			Appl;
protected:
	static struct Data {
		uint8_t										segments;
		Smartlets::Application::State::type			state;
		STAVRP::Common::Fifo<Message, uint8_t, 2>	rx_buffer;
		STAVRP::Common::Fifo<Message, uint8_t, 2>	tx_buffer;
	} data;
    // --- protected methods
    static bool IsLocal(Smartlets::Message::ID id) {
    	uint16_t id_segment = id.GetNetSegment();
    	return ( id_segment >= GetID().GetNetSegment() && id_segment <= GetID().GetNetSegment() + data.segments - 1 );
    }
    static bool DirectRoute(Smartlets::Message::MessageBase& msg) {
		if( IsLocal(msg.header.tx) ) return InterfaceLocal::TX(msg);
		return InterfaceGlobal::TX(msg);
    }
public:
	// ---
	static void Init(Smartlets::Message::ID id=Smartlets::Message::ID(), uint8_t segments=1) {
		Config::PersistManager::Init();
		InterfaceLocal::Init(id);
		InterfaceGlobal::Init(id);
		Appl::Init();
	}
	// true if were is a placeholder to send TX()
	static bool TXAvailable(void) {
		if( data.tx_buffer.AvailablePop() ) {
			Message msg = data.tx_buffer.First();
			if( DirectRoute(msg) ) data.tx_buffer.DelFirst();
		}
		return data.tx_buffer.AvailablePush();
	}
	// true of message can be read by RX()
	static bool RXAvailable(void) {
		return data.rx_buffer.AvailablePop();
	}
	// return true if message proceeded fine
	static bool TX(Smartlets::Message::MessageBase& msg) {

		if( msg.header.rx.IsUnassigned() ) msg.header.rx = GetID();

		// --- proceed with queue
		TXAvailable();

		// --- execute application
		switch( Appl::AcceptTX(msg) ) {
		case Smartlets::Node::Appl::AcceptSkip:
		case Smartlets::Node::Appl::AcceptCommit:
			return true;
		case Smartlets::Node::Appl::AcceptBridge:
		case Smartlets::Node::Appl::AcceptProceed:
			break;
		}

		// --- try immidiate route
		if( DirectRoute(msg) ) return true;

		// --- try to store to queue for later processing
		if( !data.tx_buffer.AvailablePush() ) return false;

		// --- store
		data.tx_buffer.Push(msg);
		return true;
	}
	// returns reference to message in the queue
	// valid only if RXAvailable() is true
	static const Smartlets::Message::MessageBase& RX() {
		return data.rx_buffer.First();
	}
	// commit message, send ack if it is required by message and proceed is true
	// returns true on success
	static bool CommitRX(bool proceed) {
		if( !data.rx_buffer.AvailablePop() ) return false;

        const Smartlets::Message::MessageBase& msg = data.rx_buffer.First();

        if( !proceed || !msg.header.attributes.ack ) {
			data.rx_buffer.DelFirst();
			return true;
		}

        // --- need to send acknoledge
        // replace broadcast if any by my id
        Smartlets::Message::AckReply msg_ack(msg);
        msg_ack.header.rx = GetID();
        return TX(msg_ack);
	}
	static bool Loop(void) {

		// --- loop local interface
		if( InterfaceLocal::RXAvailable() ) {
//			Message msg = InterfaceLocal::RX();
//
//			switch( Appl::AcceptRX(msg) ){
//			case Smartlets::Node::Appl::AcceptSkip:
//				// --- ignore this message
//				InterfaceLocal::CommitRX(false);
//				break;
//			case Smartlets::Node::Appl::AcceptCommit:
//				// --- commit
//				InterfaceLocal::CommitRX(true);
//				break;
//			case Smartlets::Node::Appl::AcceptBridge:
//				// --- ignore local to local and bridge local to global
//				//! if TX is false message will be skiped
//				if( !IsLocal(msg.header.tx)	|| msg.header.tx.IsBroadcastGlobal() ) {
//					InterfaceGlobal::TX(msg);
//				}
//				InterfaceLocal::CommitRX(false);
//				break;
//			case Smartlets::Node::Appl::AcceptProceed:
//				// --- queue to local rx queue
//				//! available space in queue is not checked
//				InterfaceLocal::CommitRX(false);
//				data.rx_buffer.Push(msg);
//				break;
//			}
		}

		// --- loop global interface
//		if( InterfaceGlobal::RXAvailable() ) {
//			Message msg = InterfaceGlobal::RX();
//
//			switch( Appl::AcceptRX(msg) ){
//			case Smartlets::Node::Appl::AcceptSkip:
//				// --- ignore this message
//				InterfaceGlobal::CommitRX(false);
//				break;
//			case Smartlets::Node::Appl::AcceptCommit:
//				// --- commit
//				InterfaceGlobal::CommitRX(true);
//				break;
//			case Smartlets::Node::Appl::AcceptBridge:
//				// --- ignore global to global and bridge global to local
//				//! if TX is false message will be skiped
//				if( IsLocal(msg.header.tx)	|| msg.header.tx.IsBroadcastGlobal() ) {
//					InterfaceLocal::TX(msg);
//				}
//				InterfaceGlobal::CommitRX(false);
//				break;
//			case Smartlets::Node::Appl::AcceptProceed:
//				// --- queue to local rx queue
//				//! available space in queue is not checked
//				InterfaceGlobal::CommitRX(false);
//				data.rx_buffer.Push(msg);
//				break;
//			}
//		}

//		Appl::Loop();

		return data.rx_buffer.AvailablePop();

	}
	// ---

	static Smartlets::Application::State::type GetState() {
		return data.state;
	}
	static void SetState(Smartlets::Application::State::type state) {
		data.state = state;
	}
	static Smartlets::Message::ID GetID(void) {
		return InterfaceLocal::GetID();
	}
	static void SetID(Smartlets::Message::ID id) {
		InterfaceLocal::SetID(id);
		InterfaceGlobal::SetID(id);
	}
	static uint16_t GetSID(void) {
		return InterfaceLocal::GetSID();
	}
	static void SetSegments(uint8_t segments) {
		data.segments = segments;
	}
	static void GetSegments() {
		return data.segments;
	}
	// ---
	static void Update100MSec(void) {
		Appl::Update100MSec();
		STAVRP::SoftDrv::Timer::Call<Update1Sec, 10>::Handler();
	}
	static void Update1Sec(void) {
		Appl::Update1Sec();
		STAVRP::SoftDrv::Timer::Call<Update1Min, 60>::Handler();
	}
	static void Update1Min(void) {
		Appl::Update1Min();
		Config::PersistManager::Commit();
	}
};

template <class ConfigIn, class InterfaceLocal, class InterfaceGlobal, template <class Node> class ApplTemplate>
typename Bridge<ConfigIn, InterfaceLocal, InterfaceGlobal, ApplTemplate>::Data Bridge<ConfigIn, InterfaceLocal, InterfaceGlobal, ApplTemplate>::data;

}}

#endif
