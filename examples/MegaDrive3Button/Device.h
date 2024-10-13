// Definitions.h

#ifndef _DEFINITIONS_h
#define _DEFINITIONS_h

#include <Arduino.h>
#include <RetroBle.h>

#if defined(ARDUINO_Seeed_XIAO_nRF52840_Sense) || defined(ARDUINO_Seeed_XIAO_nRF52840)
#include <variant.h>

namespace Device
{
#if defined(DEBUG)
	namespace Debug
	{
		static constexpr uint32_t SERIAL_BAUD_RATE = 115200;
	}
#endif
	static constexpr char Name[] = "MegaDrive Pad";

	namespace Version
	{
		static constexpr uint16_t Code = 0;
		static constexpr char Name[] = "0";
	}

	namespace BMS
	{
		using Pin = SeeedXIAOnRF52840::BMS::Pin;
		using Calibration = SeeedXIAOnRF52840::BMS::Bq25100Calibration;

		static constexpr uint8_t WakePin = (uint8_t)Pin::ChargingPin;

		static constexpr uint8_t HistorySize = 50;
		static constexpr uint32_t UpdatePeriodMillis = 100;
	}

	namespace Led
	{
		using Pin = SeeedXIAOnRF52840::Led::Pin;
		static constexpr bool OnState = SeeedXIAOnRF52840::Led::OnState;
	}

	namespace MegaDriveController
	{
		enum class Pin : uint8_t
		{
			Up = D0,
			Down = D1,
			InfoLeft = D2,
			InfoRight = D3,
			AB = D4,
			StartC = D5,
			Select = D6
		};
	}

	namespace BLE
	{
		static constexpr uint32_t UpdatePeriodMillis = 15;

		static constexpr RetroBle::BleConfig::Appearance Appearance = RetroBle::BleConfig::Appearance::GamePad;
	}

	namespace USB
	{
		static constexpr uint32_t UpdatePeriodMillis = 5;

		//TODO:
		static constexpr uint16_t ProductId = 0;
	}

	namespace GamePad
	{
		static constexpr uint32_t LONG_PRESS_POWER_OFF_PERIOD_MILLIS = 5000;

		static constexpr uint8_t WakePin = (uint8_t)MegaDriveController::Pin::StartC;
	}

	struct Unused
	{
		static constexpr uint8_t Pins[] = { D7, D8, D9, D10 };
	};
};

#else
#error Device is not supported for this project.
#endif

#endif

