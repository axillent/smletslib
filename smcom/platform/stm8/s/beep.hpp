#ifndef SMCOM_STM8S_BEEP_H
#define SMCOM_STM8S_BEEP_H

namespace Smcom {
  namespace STM8S {
	
    class BeepCommon {
    protected:
	  enum sel_type {
		DivX8 = 0,
		DivX4 = (1 << 6),
		DivX2 = (2 << 6)
	  };
	  enum div_type {
		Div2  = 0,
		Div3  = 1,
		Div4  = 2,
		Div5  = 3,
		Div6  = 4,
		Div7  = 5,
		Div8  = 6,
		Div9  = 7,
		Div10 = 8,
		Div11 = 9,
		Div12 = 10,
		Div13 = 11,
		Div14 = 12,
		Div15 = 13,
		Div16 = 14,
		Div17 = 15,
		Div18 = 16,
		Div19 = 17,
		Div20 = 18,
		Div21 = 19,
		Div22 = 20,
		Div23 = 21,
		Div24 = 22,
		Div25 = 23,
		Div26 = 24,
		Div27 = 25,
		Div28 = 26,
		Div29 = 27,
		Div30 = 28,
		Div31 = 29,
		Div32 = 30
	  };
    public:
	  enum freq_type {
		F500Hz  = 0 | DivX8 | Div32,
		F1KHz   = 0 | DivX8 | Div16,
		F2KHz   = 0 | DivX4 | Div16,
		F3KHz   = 0 | DivX4 | Div11,
		F4KHz   = 0 | DivX2 | Div16,
		F5KHz   = 0 | DivX2 | Div12,
		F6KHz   = 0 | DivX2 | Div10,
		F7KHz   = 0 | DivX2 | Div9,
		F8KHz   = 0 | DivX2 | Div8,
		F9KHz   = 0 | DivX2 | Div7,
		F11KHz  = 0 | DivX2 | Div6,
		F13KHz  = 0 | DivX2 | Div5,
		F16KHz  = 0 | DivX2 | Div4
	  };
    };
    
    template <BeepCommon::freq_type freq_t>
	class Beep {
	public:
	  static void Init(BeepCommon::freq_type freq = freq_t) {
		BEEP_CSR_bit.BEEPSEL = (freq >> 6) & 3;
		BEEP_CSR_bit.BEEPDIV = freq & 0x1f;
	  }
	  static void Set(bool value=1) { BEEP_CSR_bit.BEEPEN = value; }
	  static void Clr() { Set(0); }
	  static bool Get() { return BEEP_CSR_bit.BEEPEN; }
	};
	  
  }
}

#endif