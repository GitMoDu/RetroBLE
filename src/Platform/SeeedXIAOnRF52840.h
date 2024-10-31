// SeeedXIAOnRF52840.h

#ifndef _SEED_XIAO_NRF52840_h
#define _SEED_XIAO_NRF52840_h


#if defined(ARDUINO_Seeed_XIAO_nRF52840_Sense) || defined(ARDUINO_Seeed_XIAO_nRF52840)
#include <variant.h>

#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <bluefruit.h>

namespace SeeedXIAOnRF52840
{
	/// <summary>
	/// Onboard LED tied to IO.
	/// </summary>
	namespace Led
	{
		enum class Pin : uint8_t
		{
			Red = LED_RED, // D11 is P0.26 (LED RED)
			Green = LED_GREEN, // D13 is P0.30 (LED GREEN)
			Blue = LED_BLUE // D12 is P0.06 (LED BLUE)
		};

		static constexpr bool OnState = LOW;
	}

	/// <summary>
	/// Battery charge IC BQ25100.
	/// </summary>
	namespace BMS
	{
		enum class Pin : uint8_t
		{
			/// <summary>
			/// Output LOW to enable reading of the BAT voltage.
			/// D14 is P0.14 (READ_BAT) 
			/// </summary>
			Enable = VBAT_ENABLE,

			/// <summary>
			/// Read the BAT voltage.
			/// D32 is P0.31 (VBAT)
			/// </summary>
			VIn = PIN_VBAT,

			/// <summary>
			/// Battery Charging current:sswitch between 50/100 mA chargin current.
			/// The low current charging current is at the input model set up as HIGH LEVEL and the high current charging current is at the output model set up as LOW LEVEL.
			/// D22 is P0.13 (HICHG)
			/// </summary>
			ChargeCurrent = PIN_CHARGING_CURRENT,

			/// <summary>
			/// D23 is P0.17 (~CHG)
			/// </summary>
			Charging = 23
		};

		struct Bq25100Calibration
		{
			static constexpr uint16_t R1 = 1000;
			static constexpr uint16_t R2 = 510;

			static constexpr uint16_t VMin = 3600;
			static constexpr uint16_t VMax = 4000;
			static constexpr uint16_t VMaxCharging = 4200;

			static constexpr uint32_t AdcSettleMicros = 5;
		};
	}
};
#endif
#endif