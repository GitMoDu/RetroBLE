// SeeedXIAOnRF52840.h

#ifndef _SEED_XIAO_NRF52840_h
#define _SEED_XIAO_NRF52840_h

#include <Arduino.h>

#if defined(ARDUINO_Seeed_XIAO_nRF52840_Sense) || defined(ARDUINO_Seeed_XIAO_nRF52840)
#include <variant.h>
namespace SeeedXIAOnRF52840
{
	/// <summary>
	/// Onboard LED tied to IO.
	/// </summary>
	namespace Led
	{
		struct Pin
		{
			static constexpr uint8_t Red = LED_RED; // D11 is P0.26 (LED RED)
			static constexpr uint8_t Green = LED_GREEN; // D13 is P0.30 (LED GREEN)
			static constexpr uint8_t Blue = LED_BLUE; // D12 is P0.06 (LED BLUE)
		};

		static constexpr bool OnState = LOW;
	}

	/// <summary>
	/// Battery charge IC BQ25100.
	/// </summary>
	namespace BMS
	{
		struct Pin
		{
			/// <summary>
			/// Output LOW to enable reading of the BAT voltage.
			/// D14 is P0.14 (READ_BAT) 
			/// </summary>
			static constexpr uint8_t EnablePin = VBAT_ENABLE;

			/// <summary>
			/// Read the BAT voltage.
			/// D32 is P0.31 (VBAT)
			/// </summary>
			static constexpr uint8_t VInPin = PIN_VBAT;

			/// <summary>
			/// Battery Charging current:sswitch between 50/100 mA chargin current.
			/// The low current charging current is at the input model set up as HIGH LEVEL and the high current charging current is at the output model set up as LOW LEVEL.
			/// D22 is P0.13 (HICHG)
			/// </summary>
			static constexpr uint8_t ChargeCurrentPin = PIN_CHARGING_CURRENT;

			/// <summary>
			/// D23 is P0.17 (~CHG)
			/// </summary>
			static constexpr uint8_t ChargingPin = 23;
		};

		struct Bq25100Calibration
		{
			static constexpr uint16_t R1 = 1000;
			static constexpr uint16_t R2 = 510;

			static constexpr uint16_t VMin = 3350;
			static constexpr uint16_t VMax = 3700;

			static constexpr uint16_t AdcReference = 3000;
			static constexpr uint16_t AdcMax = 4095;

			static constexpr uint32_t AdcSettleMicros = 50;
		};
	}
};
#endif
#endif