/*
SMARTLETS BRIDGE APPLICATION

Add local and global interfaces and optional application

local is a network we want to bridge to global network
local network is described by bridge node ID as a start segment and by number of segments set by SetSegments()

axillent@gmail.com
 */

#ifndef _SMARTLETSP_NODE_BRIDGE_LINUX_H
#define _SMARTLETSP_NODE_BRIDGE_LINUX_H

#include <stavrp/timer.hpp>

#include <stavrp/platform/linux/drv/tcp.hpp>
#include <stavrp/platform/linux/drv/fstream.hpp>

#include <smartletsp/common/comm_routetable.hpp>
#include <smartletsp/interface/drv/drv_uart.hpp>
#include <smartletsp/interface/transl/transl_inet.hpp>
#include <smartletsp/interface/transl/transl_uart.hpp>
#include <smartletsp/interface/in_p2p.hpp>

#include <smartletsp/node/appl/appl_basic.hpp>
#include "smartletsp/node/appl/appl_routeannonce.hpp"

#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <vector>

namespace Smartlets {
namespace Linux {

//----------------------------------------------------------------------
// Definition
//----------------------------------------------------------------------
template <class ConfigIn, template <class Node> class ApplTemplate=Smartlets::Node::Appl::Standard >
class LinuxNode {
public:
	// --- public definitions
	typedef ConfigIn								Config;
	typedef typename ConfigIn::Defaults				Defaults;
	typedef	typename ConfigIn::Defaults::Message	Message;
    typedef ApplTemplate<LinuxNode>      			Appl;
protected:
    // --- inet
    typedef Smartlets::Common::RouteTableSingle										RouteTable;
    typedef STAVRP::Linux::TCPDrv<Config>											TCPDrv;
    typedef Smartlets::Interface::Translator::INet<Config, TCPDrv, RouteTable>		TranslINet;
    typedef Smartlets::Interface::P2P<Config, TranslINet, 16, 16>					InterfaceGlobal;
    // --- uart
    typedef STAVRP::Linux::FStreamDrv<Config>										UARTDrv;
    typedef Smartlets::Interface::Translator::Uart<Config, UARTDrv>					TranslUart;
    typedef Smartlets::Interface::P2P<Config, TranslUart, 16, 16>					InterfaceLocal;
	// --- route annonce application
	typedef Smartlets::Node::Appl::RouteAnnonce<LinuxNode>							ApplRouteAnnonce;
	// --- data
	static struct Data {
		Smartlets::Application::State::type		state;
		std::vector<Message>					rx_buffer;
		std::vector<Message>					tx_buffer;
		std::vector<Message>					uart_buffer;
		Smartlets::Common::RouteEntry			route_entry;
	} data;
    // --- protected methods
    static bool IsLocal(Smartlets::Message::ID id) {
    	uint16_t id_segment = id.GetNetSegment();

//		Config::Logger::Debug() << "islocal() my_segment=" << GetID().GetNetSegment() << " my_segments=" << data.route_entry.segments << " other_segment=" << id_segment << "";

    	return ( id_segment >= GetID().GetNetSegment() && id_segment <= GetID().GetNetSegment() + data.route_entry.segments - 1 );
    }
    static bool DirectRoute(Smartlets::Message::MessageBase& msg) {
		if( IsLocal(msg.header.tx) ) {
			Config::Logger::Debug() << "tx -> local " << msg << "";
			data.uart_buffer.push_back(msg);
			return true;
		}
		Config::Logger::Debug() << "tx -> global" << msg << "";
		return InterfaceGlobal::TX(msg);
    }
public:
	// ---
    static void setUartFileName(std::string name) {
    	UARTDrv::setFileName(name);
    }
	static void setIPPort(const uint8_t* ip, uint16_t port) {
		data.route_entry.port = port;
		RouteTable::addRoute(ip, port, 0, 0);
	}
	static const Smartlets::Common::RouteEntry& GetRouteEntry() {

		if( !data.route_entry.ip[0] ) {
			// --- looking for my IP
		    struct ifreq ifr;

		    memset(&ifr, 0, sizeof(ifr));
		    strcpy(ifr.ifr_name, "eth0");

		    int s = socket(AF_INET, SOCK_DGRAM, 0);
		    ioctl(s, SIOCGIFADDR, &ifr);
		    close(s);

		    struct sockaddr_in *sa = (struct sockaddr_in*)&ifr.ifr_addr;

		    Config::Logger::Debug() << "my addr = " << inet_ntoa(sa->sin_addr) << "";

//		    uint32_t addr = ntohl(sa->sin_addr.s_addr);
		    uint32_t addr = sa->sin_addr.s_addr;
		    memcpy(data.route_entry.ip, &addr, 4);
		}

		return data.route_entry;
	}
    // ---
	static void Init(Smartlets::Message::ID id=Smartlets::Message::ID(), uint16_t segments=1) {
		data.route_entry.segment = id.GetNetSegment();
		data.route_entry.segments = segments;

		if( data.route_entry.port ) {
			TCPDrv::setPort(data.route_entry.port);
		} else {
			Config::Logger::Error() << "LinuxNode::Init port is not set\n";
		}

		InterfaceLocal::Init(id);
		InterfaceGlobal::Init(id);
		ApplRouteAnnonce::Init();
		Appl::Init();
	}
	// true if were is a placeholder to send TX()
	static bool TXAvailable(void) {
		if( data.tx_buffer.size() ) {
			if( DirectRoute(data.tx_buffer.front()) ) data.tx_buffer.erase(data.tx_buffer.begin());
		}
		return true;
	}
	// true of message can be read by RX()
	static bool RXAvailable(void) {
		return data.rx_buffer.size();
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
			Config::Logger::Debug() << "tx skip " << msg << "";
			return true;
		case Smartlets::Node::Appl::AcceptBridge:
		case Smartlets::Node::Appl::AcceptProceed:
			break;
		}

		// --- try immidiate route
		if( DirectRoute(msg) ) return true;

		// --- store
		data.tx_buffer.push_back(msg);
		TXAvailable();

		return true;
	}
	// returns reference to message in the queue
	// valid only if RXAvailable() is true
	static const Smartlets::Message::MessageBase& RX() {
		return data.rx_buffer.front();
	}
	// commit message, send ack if it is required by message and proceed is true
	// returns true on success
	static bool CommitRX(bool proceed) {
		if( !data.rx_buffer.size() ) return false;

        const Smartlets::Message::MessageBase& msg = data.rx_buffer.front();

        if( !proceed || !msg.header.attributes.ack ) {
			data.rx_buffer.erase(data.rx_buffer.begin());
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
			Message msg = InterfaceLocal::RX();

			Config::Logger::Debug() << "uart -> " << msg << "";

			switch( Appl::AcceptRX(msg) ){
			case Smartlets::Node::Appl::AcceptSkip:
				// --- ignore this message
				InterfaceLocal::CommitRX(false);
				break;
			case Smartlets::Node::Appl::AcceptCommit:
				// --- commit
				InterfaceLocal::CommitRX(true);
				break;
			case Smartlets::Node::Appl::AcceptBridge:
				// --- ignore local to local and bridge local to global
				//! if TX is false message will be skiped
				if( !IsLocal(msg.header.tx)	|| msg.header.tx.IsBroadcastGlobal() ) {
					Config::Logger::Debug() << "-> inet " << msg << "";
					InterfaceGlobal::TX(msg);
				}
				InterfaceLocal::CommitRX(false);
				break;
			case Smartlets::Node::Appl::AcceptProceed:
				// --- queue to local rx queue
				//! available space in queue is not checked
				InterfaceLocal::CommitRX(false);
				data.rx_buffer.push_back(msg);
				break;
			}
		}

		// --- loop global interface
		if( InterfaceGlobal::RXAvailable() ) {
			Message msg = InterfaceGlobal::RX();

			Config::Logger::Debug() << "inet -> " << msg << "";

			switch( Appl::AcceptRX(msg) ){
			case Smartlets::Node::Appl::AcceptSkip:
				// --- ignore this message
				Config::Logger::Debug() << "accept skip\n";
				InterfaceGlobal::CommitRX(false);
				break;
			case Smartlets::Node::Appl::AcceptCommit:
				// --- commit
				Config::Logger::Debug() << "accept commit\n";
				InterfaceGlobal::CommitRX(true);
				break;
			case Smartlets::Node::Appl::AcceptBridge:
				// --- ignore global to global and bridge global to local
				//! if TX is false message will be skiped
				if( IsLocal(msg.header.tx)	|| msg.header.tx.IsBroadcastGlobal() ) {
					Config::Logger::Debug() << "-> uart " << msg << "";
					data.uart_buffer.push_back(msg);
				} else {
					Config::Logger::Debug() << "accept not local ignore\n";
				}
				InterfaceGlobal::CommitRX(false);
				break;
			case Smartlets::Node::Appl::AcceptProceed:
				// --- queue to local rx queue
				//! available space in queue is not checked
				Config::Logger::Debug() << "accept myself\n";
				InterfaceGlobal::CommitRX(false);
				data.rx_buffer.push_back(msg);
				break;
			}
		}

		Appl::Loop();

		return data.rx_buffer.size();

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
		data.route_entry.segments = segments;
	}
	static void GetSegments() {
		return data.route_entry.segments;
	}
	// ---
	static void LoopUart(void) {
		if( data.uart_buffer.size() && InterfaceLocal::TXAvailable() ) {
			if( InterfaceLocal::TX(data.uart_buffer.front()) ) {
				data.uart_buffer.erase(data.uart_buffer.begin());
			}
		}
	}
	static void Update100MSec(void) {
		Appl::Update100MSec();
		STAVRP::Timer::Call<LoopUart, 5>::Handler();
		STAVRP::Timer::Call<Update1Sec, 10>::Handler();
	}
	static void Update1Sec(void) {
		Appl::Update1Sec();
		STAVRP::Timer::Call<Update1Min, 60>::Handler();
	}
	static void Update1Min(void) {
		STAVRP::Linux::Logger::Debug() << "Update1Min\n";

		Appl::Update1Min();
		ApplRouteAnnonce::Update1Min();
	}
};

template <class ConfigIn, template <class Node> class ApplTemplate>
typename LinuxNode<ConfigIn, ApplTemplate>::Data LinuxNode<ConfigIn, ApplTemplate>::data;

}}

#endif
