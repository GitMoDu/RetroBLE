// NologoProMicronRF52840.h

#ifndef _NOLOGO_PRO_MICRO_NRF52840_h
#define _NOLOGO_PRO_MICRO_NRF52840_h

#if defined(ARDUINO_ARCH_NRF52)
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <bluefruit.h>

namespace NologoProMicronRF52840
{
	namespace ExternalVcc
	{
		enum class Pin : uint8_t
		{
			/// <summary>
			/// P0.13
			/// </summary>
			Enable = 22
		};

		static constexpr uint8_t OnState = LOW;
	}

	/// <summary>
	/// Battery charge IC LTH7R.
	/// </summary>
	namespace BMS
	{
		/// <summary>
		/// Charge current is determined by external boost pin.
		/// </summary>
		enum class ChargeCurrent : uint16_t
		{
			Normal = 100,
			Fast = 300
		};
	}
};
#endif
#endif