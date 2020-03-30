#ifndef SMCOM_STM8S_MCU_H
#define SMCOM_STM8S_MCU_H

#include "../../../smcom.hpp"

namespace Smcom {
  namespace STM8S {
    
    class MCUCommon {
    protected:
      enum clock_src { 
        CLK_FROM_HSI = 0, 
        CLK_FROM_HSE = (1 << 8),
        CLK_FROM_LSI = (2 << 8) 
      };
      enum clock_div {
        CLK_DIV_1  = 0, 
        CLK_DIV_2  = (1 << 10), 
        CLK_DIV_4  = (2 << 10), 
        CLK_DIV_8  = (3 << 10), 
        CLK_DIV_16 = (4 << 10), 
        CLK_DIV_32 = (5 << 10), 
        CLK_DIV_64 = (6 << 10), 
        CLK_DIV_128= (7 << 10)
      };
    public:
      enum clock_sel {
        // --- low RC
        LSI        = 128 | CLK_FROM_LSI | CLK_DIV_1,
        // --- RC
        HSI_16MHz  = 160 | CLK_FROM_HSI | CLK_DIV_1,
        HSI_8MHz   = 80  | CLK_FROM_HSI | CLK_DIV_2,
        HSI_4MHz   = 40  | CLK_FROM_HSI | CLK_DIV_4,
        HSI_2MHz   = 20  | CLK_FROM_HSI | CLK_DIV_8,
        HSI_1MHz   = 10  | CLK_FROM_HSI | CLK_DIV_16,
        HSI_500KHz = 5   | CLK_FROM_HSI | CLK_DIV_32,
        // --- crystal 16 MHz
        HSE_IN16_OUT16MHz = 160 | CLK_FROM_HSE | CLK_DIV_1,
        HSE_IN16_OUT8MHz  = 80  | CLK_FROM_HSE | CLK_DIV_2,
        HSE_IN16_OUT4MHz  = 40  | CLK_FROM_HSE | CLK_DIV_4,
        HSE_IN16_OUT2MHz  = 20  | CLK_FROM_HSE | CLK_DIV_8,
        HSE_IN16_OUT1MHz  = 10  | CLK_FROM_HSE | CLK_DIV_16,
        HSE_IN16_OUT500KHz= 5   | CLK_FROM_HSE | CLK_DIV_32,
        // --- crystal 8 MHz
        HSE_IN8_OUT8MHz  = 80  | CLK_FROM_HSE | CLK_DIV_1,
        HSE_IN8_OUT4MHz  = 40  | CLK_FROM_HSE | CLK_DIV_2,
        HSE_IN8_OUT2MHz  = 20  | CLK_FROM_HSE | CLK_DIV_4,
        HSE_IN8_OUT1MHz  = 10  | CLK_FROM_HSE | CLK_DIV_8,
        HSE_IN8_OUT500KHz= 5   | CLK_FROM_HSE | CLK_DIV_16
      };
      enum extint { 
        EXTINT_FallingLow = 0, 
        EXTINT_Rising = 1,
        EXTINT_Falling = 2,
        EXTINT_FallingRising = 3
      };
      static void InterruptEnable(void) { __enable_interrupt(); }
      static void InterruptDisable(void) { __disable_interrupt(); }
      static void InterruptWait(void) { __wait_for_interrupt(); }
    };
    
    template <MCUCommon::clock_sel clock = MCUCommon::HSI_16MHz>
      class MCU : public MCUCommon {
      public:
        static const Smcom::Platform::type platform = Smcom::Platform::STM8S;
        static const uint32_t cpu_f = (clock & 0xff) * ((clock & static_cast<uint16_t>(CLK_FROM_LSI))?1:100000UL);
        static void Init(void) {
          
          while( CLK_SWCR_bit.SWBSY ) {}
          
          switch( clock & 0x0700 ) {
          case CLK_FROM_HSI:
            if( CLK_SWR != 0xE1 ) {
              CLK_SWCR_bit.SWEN = 1;
              CLK_SWR = 0xE1;
            }
            CLK_CKDIVR_bit.HSIDIV = 0;
            break;
          case CLK_FROM_HSE:
            CLK_SWCR_bit.SWEN = 1;
            CLK_SWR = 0xB4;
            break;
          case CLK_FROM_LSI:
            CLK_SWCR_bit.SWEN = 1;
            CLK_SWR = 0xD2;
            break;
          };
          CLK_CKDIVR_bit.HSIDIV = (clock >> 10) & 7;
        }
      };
    
    
  }
}

#endif
