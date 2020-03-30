#ifndef SMCOM_SOFTDRV_LEDSCAN_HPP
#define SMCOM_SOFTDRV_LEDSCAN_HPP

#include "../smcom.hpp"

namespace Smcom {	namespace SoftDrv { namespace LedScan {

	class NopDrv {
	  public:
		inline static void Init(uint8_t) {}
		inline static void Set(uint8_t) {}
	};
    
    template <int n_p, class p0, class p1=NopDrv, class p2=NopDrv, class p3=NopDrv, class p4=NopDrv, class p5=NopDrv, class p6=NopDrv, class p7=NopDrv>
      class RawColDrv {
      public:
        static const int n = n_p;
        static void Init() {
          p0::Init(PinMode::OutputPushPull);
          p1::Init(PinMode::OutputPushPull);
          p2::Init(PinMode::OutputPushPull);
          p3::Init(PinMode::OutputPushPull);
          p4::Init(PinMode::OutputPushPull);
          p5::Init(PinMode::OutputPushPull);
          p6::Init(PinMode::OutputPushPull);
          p7::Init(PinMode::OutputPushPull);
        }
        static void Put(uint8_t mask) {
          p0::Set(mask & 1);
          if( n_p >= 2) p1::Set(mask & 2);
          if( n_p >= 3) p2::Set(mask & 4);
          if( n_p >= 4) p3::Set(mask & 8);
          if( n_p >= 5) p4::Set(mask & 16);
          if( n_p >= 6) p5::Set(mask & 32);
          if( n_p >= 7) p6::Set(mask & 64);
          if( n_p >= 8) p7::Set(mask & 128);
        }
      };
    
    template <uint8_t *data, uint8_t size>
      class PutDrv {
	  public:
        inline static void Put(uint8_t digit, uint8_t mask) {
          data[digit] = mask;
        }
      };
    
	template <uint8_t *data, uint8_t size>
      class PutReverseDrv {
	  public:
        inline static void Put(uint8_t digit, uint8_t mask) {
          // fill data array reverse order
          for (uint8_t i = 0; i < size; i++) {
			if( mask & (1 << i) ) data[i] |= (1 << digit); else data[i] &= ~(1 << digit);
          }
        }
      };    
    
	template <class RawDrv, class ColDrv, template<uint8_t*, uint8_t> class PutDrv>
      class Display {
	  protected:
        static const int n = RawDrv::n;
        static uint8_t	data[RawDrv::n];
        static uint8_t	index;
	  public:
        static void Init(void) {
          index = 0;
          RawDrv::Init();
          ColDrv::Init();
        }
        static void On(void) {
        }
        static void Off(void) {
        }
        static void PutDigit(uint8_t digit, uint8_t mask) {
          PutDrv<data, n>::Put(digit, mask);
        }
        static void Update(void) {
          RawDrv::Put(1 << index);
          ColDrv::Put(data[index]);
          if( ++(index) >= n ) index = 0;
        }
      };
	template <class RawDrv, class ColDrv, template<uint8_t*, uint8_t> class PutDrv>
      uint8_t Display<RawDrv, ColDrv, PutDrv>::data[RawDrv::n];
	template <class RawDrv, class ColDrv, template<uint8_t*, uint8_t> class PutDrv>
      uint8_t Display<RawDrv, ColDrv, PutDrv>::index;
    
}}}

#endif
