#ifndef SMCOM_SOFT_DRV_SPI_H
#define SMCOM_SOFT_DRV_SPI_H

namespace Smcom { namespace SoftDrv { namespace SPI {

	struct SPIMode {
		enum type { Master, Slave };
	};

	template <class MISO, class MOSI, class SCK, SPIMode::type mode_t=SPIMode::Master>
	class SPISoftDriver {
	public:
		static void Init() {
			MISO::Init();
			MOSI::Init();
			SCK::Init();
		}
	};

}}}

#endif
