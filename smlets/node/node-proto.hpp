#ifndef SMLETS_NODE_PROTO_H
#define SMLETS_NODE_PROTO_H

#include "../smlets.hpp"
#include "../msg/msg-id.hpp"
#include "../msg/msg-base.hpp"

namespace Smlets {
namespace Node {

class Proto {
public:
    static void _Init();
    static void _Start();
	static void Init(Smlets::Message::ID id = Smlets::Message::ID());
	static bool TXAvailable(void);
	static bool RXAvailable(void);
	static bool TX(Smlets::Message::MessageBase& msg);
	static const Smlets::Message::MessageBase& RX();
	static bool CommitRX(bool proceed);
	static bool Loop(void);
	// ---
	static Smlets::Application::State::type GetState();
	static void SetState(Smlets::Application::State::type state);
	static Smlets::Message::ID GetID(void);
	static void SetID(Smlets::Message::ID id);
	static uint16_t GetSID(void);
	// ---
	static void Update100MSec(void);
	static void Update1Sec(void);
	static void Update1Min(void);
};

}}

#endif
