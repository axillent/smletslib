#ifndef SMLETS_NODE_APPL_PROTO_H
#define SMLETS_NODE_APPL_PROTO_H

#include "../../msg/msg-id.hpp"
#include "../../msg/msg-base.hpp"

namespace Smlets {
namespace Node {
namespace Appl {

  class AcceptState {
  protected:
    uint8_t data;
  public:
    enum type {
      AcceptBridge, 	///< ignore for local queue but accept to bridge & route
      AcceptProceed, 	///< proceed with in local queue
      AcceptCommit, 	///< stop processing, commit
      AcceptSkip		///< skip any way
    };
    AcceptState(type t) : data(t) {}
    bool operator == (const AcceptState right) const { return right.data == data; }
    bool operator != (const AcceptState right) const { return right.data != data; }
  };
  
class Proto {
public:
	static void Init(void);
    static void Start(void);
	static void Loop(void);
	static void Update100MSec(void);
	static void Update1Sec(void);
	static void Update1Min(void);
	static Smlets::Node::Appl::AcceptState AcceptRX(const Smlets::Message::MessageBase& msg);
	static Smlets::Node::Appl::AcceptState AcceptTX(const Smlets::Message::MessageBase& msg);  
};

}}}

#endif
