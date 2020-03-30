#ifndef SMCOM_SOFT_DRV_UART_H
#define SMCOM_SOFT_DRV_UART_H

#include "../common/buffer.hpp"
#include "../common/format.hpp"

namespace Smcom { namespace SoftDrv {

  template <class UARTDriver, const uint32_t baudrate_t>
	class UARTClass : public UARTDriver, public Format<UARTDriver> {
	public:
	  static uint8_t GetByte(void) {
		  while( !UARTDriver::data.rx_buffer.AvailablePop() ) {}
		  return UARTDriver::data.rx_buffer.Pop();
	  }
	  static bool RXAvailable(void) { return UARTDriver::data.rx_buffer.AvailablePop(); }
	  static bool TXAvailable(void) { return UARTDriver::data.tx_buffer.AvailablePush(); }
	  static bool TXEmpty(void) { return UARTDriver::data.tx_buffer.State() == Smcom::Common::FifoState::Empty; }
	};

}}


#endif
