// Device.h

#ifndef _DEVICE_h
#define _DEVICE_h

#if defined(ARDUINO_ARCH_NRF52)
//#if defined(ARDUINO_Seeed_XIAO_nRF52840_Sense) || defined(ARDUINO_Seeed_XIAO_nRF52840)
#include <RetroBle.h>

namespace Device
{
#if defined(DEBUG)
	namespace Debug
	{
		static constexpr uint32_t SERIAL_BAUD_RATE = 115200;
	}
#endif
	static constexpr char Name[] = "VirtualPadHost";

	namespace Version
	{
		static constexpr uint16_t Code = 0;
		static constexpr char Name[] = "0";
	}

	namespace UartInterface
	{
		static constexpr auto& UartInstance = Serial1;

		enum class Pin : uint8_t
		{
			Rx = PIN_SERIAL1_RX,
			Tx = PIN_SERIAL1_TX
		};
		
	}

	namespace BLE
	{
	}

	namespace Unused
	{
		static constexpr uint8_t Pins[] = { };
	};
};

#else
#error Device is not supported for this project.
#endif

#endif

