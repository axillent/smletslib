/*
smart devices interface
supports AVR8 & STM8S & STM32

SMARTLETS APPLICATION

axillent@gmail.com
 */

#ifndef SMLETS_NODE_BASIC_H
#define SMLETS_NODE_BASIC_H

#include <smcom/softdrv/timer.hpp>

#include "node-proto.hpp"
#include "appl/appl-basic.hpp"

namespace Smlets {
namespace Node {

template <class Node>
class NodeBasic : public Proto {
protected:
    static Smlets::Message::ID              id;
    static uint16_t                         sid;
    static Smlets::Application::State::type state;
    static bool                             start;
public:
    static void Init(Smlets::Message::ID id_p = Smlets::Message::ID()) {
      id = id_p;
      
      Node::Hardware::Init(Update100MSec);
      Node::_Init();
      Node::Application::Init();
      Node::Hardware::Start();
    }
//	static bool TXAvailable(void);
//	static bool RXAvailable(void);
//	static bool TX(Smlets::Message::MessageBase& msg);
//	static const Smlets::Message::MessageBase& RX();
//	static bool CommitRX(bool proceed);
	static bool Loop(void) {
      if( start ) {
        start = false;
        Node::_Start();
        Node::Application::Start();
      }
      Node::Application::Loop();
      Node::Hardware::Loop();
      return Node::RXAvailable();
    }
	// ---
	static Smlets::Application::State::type GetState() {
      return state;
    }
	static void SetState(Smlets::Application::State::type state_p) {
      state = state_p;
    }
	static Smlets::Message::ID GetID(void) {
      return id;
    }
	static void SetID(Smlets::Message::ID id_p) {
      id = id_p;
    }
	static uint16_t GetSID(void) {
      return sid;
    }
	// ---
	static void Update100MSec(void) {
      Node::Update100MSec();
      Node::Application::Update100MSec();
      Smcom::SoftDrv::Timer::Call<Update1Sec, 10>::Handler();
    }
	static void Update1Sec(void) {
      Node::Update1Sec();
      Node::Application::Update1Sec();
      Smcom::SoftDrv::Timer::Call<Update1Min, 60>::Handler();
    }
	static void Update1Min(void) {
      Node::Update1Min();
      Node::Application::Update1Min();
    }
};

template <class Node> Smlets::Message::ID               NodeBasic<Node>::id;
template <class Node> Smlets::Application::State::type  NodeBasic<Node>::state = Smlets::Application::State::Normal;
template <class Node> uint16_t                          NodeBasic<Node>::sid = 0;
template <class Node> bool                              NodeBasic<Node>::start = true;

}}

#endif

/*
//----------------------------------------------------------------------
// Basic Node
//----------------------------------------------------------------------
template <class ConfigIn, class Interface, template <class Node> class ApplTemplate=Appl::Standard >
class NodeBasic {
public:
	// --- public definitions
	typedef ConfigIn								Config;
	typedef typename ConfigIn::Defaults				Defaults;
	typedef	typename ConfigIn::Defaults::Message	Message;
    typedef ApplTemplate<NodeBasic>         		Appl;
protected:
	static struct Data {
		Smartlets::Application::State::type	state;
		bool								rx_available;
	} data;
public:
	// ---
	static void Init(Smartlets::Message::ID id=Smartlets::Message::ID());
	static bool TXAvailable(void) { return Interface::TXAvailable(); }
	static bool RXAvailable(void);
	static bool TX(Smartlets::Message::MessageBase& msg);
	static const Smartlets::Message::MessageBase& RX();
	static bool CommitRX(bool proceed);
	static bool Loop(void);
	// ---
	static Smartlets::Application::State::type GetState() { return data.state; }
	static void SetState(Smartlets::Application::State::type state) { data.state = state; }
	static Smartlets::Message::ID GetID(void) { return Interface::GetID(); }
	static void SetID(Smartlets::Message::ID id) { Interface::SetID(id); }
	static uint16_t GetSID(void) { return Interface::GetSID(); }
	// ---
	static void Update100MSec(void);
	static void Update1Sec(void);
	static void Update1Min(void);
};

template <class ConfigIn, class Interface, template <class Node> class ApplTemplate>
typename NodeBasic<ConfigIn, Interface, ApplTemplate>::Data NodeBasic<ConfigIn, Interface, ApplTemplate>::data;

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------
//--- Init
template <class ConfigIn, class Interface, template <class Node> class ApplTemplate>
void NodeBasic<ConfigIn, Interface, ApplTemplate>::Init(Smartlets::Message::ID id) {

	data.rx_available = false;

	Config::PersistManager::Init();
	Interface::Init(id);
    Appl::Init();

}

//--- TX
template <class ConfigIn, class Interface, template <class Node> class ApplTemplate>
bool NodeBasic<ConfigIn, Interface, ApplTemplate>::TX(Smartlets::Message::MessageBase& msg) {

    if( msg.header.rx.IsUnassigned() ) msg.header.rx = GetID();

    if( Appl::AcceptTX(msg) == Smartlets::Node::Appl::AcceptSkip ) return true;

	return Interface::TX(msg);

}

//--- RXAvailable
template <class ConfigIn, class Interface, template <class Node> class ApplTemplate>
bool NodeBasic<ConfigIn, Interface, ApplTemplate>::RXAvailable() {

	while( Interface::RXAvailable() ) {

		// --- a message is awaiting
		if( data.rx_available ) return true;

		// --- should we accept the message?
		const Smartlets::Message::MessageBase& msg = Interface::RX();

		switch( Appl::AcceptRX(msg) ) {
		case Smartlets::Node::Appl::AcceptSkip:
		case Smartlets::Node::Appl::AcceptBridge:
			CommitRX(false);
			break;
		case Smartlets::Node::Appl::AcceptCommit:
			CommitRX(true);
			break;
		case Smartlets::Node::Appl::AcceptProceed:
			data.rx_available = true;
			return true;
		}

	}

	return false;
}

//--- RX
template <class ConfigIn, class Interface, template <class Node> class ApplTemplate>
const Smartlets::Message::MessageBase& NodeBasic<ConfigIn, Interface, ApplTemplate>::RX() {

	RXAvailable();

	return Interface::RX();

}

//--- CommitRX
template <class ConfigIn, class Interface, template <class Node> class ApplTemplate>
bool NodeBasic<ConfigIn, Interface, ApplTemplate>::CommitRX(bool proceed) {

	data.rx_available = false;

	return Interface::CommitRX(proceed);

}

//--- Loop
template <class ConfigIn, class Interface, template <class Node> class ApplTemplate>
bool NodeBasic<ConfigIn, Interface, ApplTemplate>::Loop(void) {

	Appl::Loop();
	return RXAvailable();

}

//--- Update100MSec
template <class ConfigIn, class Interface, template <class Node> class ApplTemplate>
void NodeBasic<ConfigIn, Interface, ApplTemplate>::Update100MSec(void) {

	Appl::Update100MSec();
    STAVRP::SoftDrv::Timer::Call<Update1Sec, 10>::Handler();

}

//--- Update1Sec
template <class ConfigIn, class Interface, template <class Node> class ApplTemplate>
void NodeBasic<ConfigIn, Interface, ApplTemplate>::Update1Sec(void) {

	Appl::Update1Sec();
    STAVRP::SoftDrv::Timer::Call<Update1Min, 60>::Handler();

}

//--- Update1Min
template <class ConfigIn, class Interface, template <class Node> class ApplTemplate>
void NodeBasic<ConfigIn, Interface, ApplTemplate>::Update1Min(void) {

	Appl::Update1Min();
	Config::PersistManager::Commit();

}

}}*/

