/**

\todo add clock source selection and configuration

*/

#ifndef SMCOM_STM32_MCU_H
#define SMCOM_STM32_MCU_H

namespace Smcom {
  
  namespace STM32 {
    
    class MCUCommon {
    protected:
      enum clock_src { 
        CLK_FROM_HSI = 0, 
        CLK_FROM_HSE = (1 << 8) 
      };
      enum clock_pll { 
        CLK_PLL_OFF = (15 << 10), 
        CLK_PLL_2   = 0,
        CLK_PLL_3   = (1  << 10),
        CLK_PLL_4   = (2  << 10),
        CLK_PLL_5   = (3  << 10),
        CLK_PLL_6   = (4  << 10),
        CLK_PLL_7   = (5  << 10),
        CLK_PLL_8   = (6  << 10),
        CLK_PLL_9   = (7  << 10),
        CLK_PLL_10  = (8  << 10),
        CLK_PLL_11  = (9  << 10),
        CLK_PLL_12  = (10 << 10),
        CLK_PLL_13  = (11 << 10),
        CLK_PLL_14  = (12 << 10),
        CLK_PLL_15  = (13 << 10),
        CLK_PLL_16  = (14 << 10)
      };
    public:
      enum clock_sel {
        HSI_IN8_OUT8MHz  = 8   | CLK_FROM_HSI | CLK_PLL_OFF,
        HSI_IN8_OUT16MHz = 16  | CLK_FROM_HSI | CLK_PLL_4,
        HSI_IN8_OUT32MHz = 32  | CLK_FROM_HSI | CLK_PLL_8,
        HSI_IN8_OUT48MHz = 48  | CLK_FROM_HSI | CLK_PLL_12,
        HSE_IN8_OUT8MHz  = 8   | CLK_FROM_HSE | CLK_PLL_OFF,
        HSE_IN8_OUT16MHz = 16  | CLK_FROM_HSE | CLK_PLL_2,
        HSE_IN8_OUT32MHz = 32  | CLK_FROM_HSE | CLK_PLL_4,
        HSE_IN8_OUT48MHz = 48  | CLK_FROM_HSE | CLK_PLL_6,
        HSE_IN16_OUT16MHz = 16 | CLK_FROM_HSE | CLK_PLL_OFF,
        HSE_IN16_OUT32MHz = 32 | CLK_FROM_HSE | CLK_PLL_2,
        HSE_IN16_OUT48MHz = 48 | CLK_FROM_HSE | CLK_PLL_3
      };
      static const Smcom::Platform::type platform = Smcom::Platform::STM32F0;
      enum APBENR { APB1ENR, APB2ENR };
      static constexpr volatile uint32_t* RegApbEnr(APBENR reg) {
        return (reg==APB1ENR) ? (&RCC->APB1ENR) : (&RCC->APB2ENR);
      }
      static const uint32_t * UniqueID() { return (uint32_t *)0x1FFFF7AC; }
      static void InterruptEnable(void) { __enable_irq (); }
      static void InterruptDisable(void) { __disable_irq (); }
      static void InterruptWait(void) { }
    };
    
    template <MCUCommon::clock_sel clock_p = MCUCommon::HSI_IN8_OUT16MHz>
      class MCU : public MCUCommon {
      public:
        static const unsigned long cpu_f = (clock_p & 0xff) * 1000000UL;
        static void Init(void) {
          
          bool use_pll = (clock_p & (0xf << 10)) != CLK_PLL_OFF;
          bool use_hsi = (clock_p & (0x3 << 8)) == CLK_FROM_HSI;
          
          if( use_pll ) {
            // --- seting multiplication
            RCC->CFGR |= ((clock_p >> 10) & 0xf) << 18;
            // --- PLL source
            RCC->CFGR |= (use_hsi)?RCC_CFGR_PLLSRC_HSI_Div2:RCC_CFGR_PLLSRC_PREDIV1;
            // --- enable PLL
            RCC->CR   |= RCC_CR_PLLON;
            // --- wait until start complete
            //while( !(RCC->CR & RCC_CR_PLLRDY) );
          }

          // clock source
          if( use_hsi ) {
            // --- enable HSI
            RCC->CR |= RCC_CR_HSION;
            // --- wait until start complete
            while( !(RCC->CR & RCC_CR_HSIRDY) );
            // --- select system source
            RCC->CFGR |= (use_pll)?RCC_CFGR_SW_PLL:RCC_CFGR_SW_HSI;
          } else {
            // --- enable HSE
            RCC->CR |= RCC_CR_HSEON;
            // --- wait until start complete
            while( !(RCC->CR & RCC_CR_HSERDY) );
            RCC->CFGR |= (use_pll)?RCC_CFGR_SW_PLL:RCC_CFGR_SW_HSE;
          }
                       
        }
      };
      
  }
}

extern "C" {
  void SystemInit(void) {}
}

#endif
