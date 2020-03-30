#ifndef SMLETS_NODE_UART_HPP
#define SMLETS_NODE_UART_HPP

#include <smcom/softdrv/timer.hpp>

#include "../msg/msg-queue.hpp"
#include "appl/appl-proto.hpp"
#include "node-basic.hpp"

namespace Smlets {
namespace Node {

template <class Hardware_t, template <class Node> class ApplTemplate>
class Uart : public NodeBasic<Uart<Hardware_t, ApplTemplate> > {
public:
  typedef Hardware_t            Hardware;
  typedef ApplTemplate<Uart>    Application;
protected:
  typedef NodeBasic<Uart>       Basic;
  static struct Data {
    Smlets::Message::Queue::Stream<typename Hardware::UartDriver>  stream;
    Smlets::Message::Queue::String2  rx_msg;
    bool                             rx_ready;
  } data;
public:
  static void _Init() {
    data.rx_ready = false;
  }
  static void _Start() {
    // --- announcing
    Smlets::Message::Queue::String2 msg1("HWName", Hardware::GetHWName());
    Smlets::Message::Queue::String2 msg2("HWID", Hardware::HWID);
    TX(msg1);
    TX(msg2);
  }
  static bool TXAvailable(void) {
    return Hardware::UartDriver::TXAvailable();
  }
  static bool RXAvailable(void) {
    if( !data.rx_ready && data.stream.in(data.rx_msg) ) {
      data.stream.in_clr();
      data.rx_ready = (Application::AcceptRX(data.rx_msg) == Appl::AcceptState::AcceptProceed);
    }
    return data.rx_ready;
  }
  static bool TX(Smlets::Message::MessageBase& msg) {
    if( Application::AcceptTX(msg) != Appl::AcceptState::AcceptProceed ) return false;
    
    if( msg.id == Smlets::Message::ID::Unassigned ) msg.id = Basic::id;
    msg.sid = Basic::sid++;
    
    switch ( msg.type ) {
    case Smlets::Message::Type::QueueString2:
      data.stream.out((Smlets::Message::Queue::String2&)msg);
      break;
    default:
      return false;
    }
    
    return true;
  }
  static const Smlets::Message::MessageBase& RX() {
    return data.rx_msg;
  }
  static bool CommitRX(bool proceed=true) {
    data.rx_ready = false;;
    return true;
  }
  // ---
  static void Update100MSec(void) {
  }
  static void Update1Sec(void) {
  }
  static void Update1Min(void) {
  }
};

template <class Hardware, template <class Node> class ApplTemplate> 
typename Uart<Hardware, ApplTemplate>::Data Uart<Hardware, ApplTemplate>::data;

}}

#endif