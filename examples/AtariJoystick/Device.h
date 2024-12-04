// Device.h

#ifndef _DEVICE_h
#define _DEVICE_h

#if defined(ARDUINO_Seeed_XIAO_nRF52840_Sense) || defined(ARDUINO_Seeed_XIAO_nRF52840)
#include <RetroBle.h>

namespace Device
{
#if defined(DEBUG)
	namespace Debug
	{
		static constexpr uint32_t SERIAL_BAUD_RATE = 115200;
	}
#endif
	static constexpr char Name[] = "Atari 2600 Joystick";

	namespace Version
	{
		static constexpr uint16_t Code = 1;
		static constexpr char Name[] = "1";
	}

	namespace BMS
	{
		using Pin = SeeedXIAOnRF52840::BMS::Pin;
		using Calibration = SeeedXIAOnRF52840::BMS::Bq25100Calibration;

		static constexpr uint8_t HistorySize = 100;
		static constexpr uint32_t UpdatePeriodMillis = 30;
	}

	namespace Led
	{
		using Pin = SeeedXIAOnRF52840::Led::Pin;
		static constexpr bool OnState = SeeedXIAOnRF52840::Led::OnState;
	}

	namespace AtariController
	{
		enum class Pin : uint8_t
		{
			Up = D1,
			Down = D2,
			Left = D3,
			Right = D0,
			Button = D4
		};

		static constexpr uint8_t WakePin = (uint8_t)Pin::Button;
	}

	namespace BLE
	{
		static constexpr uint32_t ConnectionIntervalMin = RetroBle::BleConfig::ConnectionIntervalFast::Min;
		static constexpr uint32_t ConnectionIntervalMax = RetroBle::BleConfig::ConnectionIntervalFast::Max;

		static constexpr uint32_t UpdatePeriodMillis = 3;

		static constexpr RetroBle::BleConfig::Appearance Appearance = RetroBle::BleConfig::Appearance::GamePad;

	}

	namespace USB
	{
		static constexpr uint32_t UpdatePeriodMillis = 2;

		static constexpr uint16_t ProductId = (uint16_t)RetroBle::Device::ProductIds::Atari;
	}

	namespace Unused
	{
		static constexpr uint8_t Pins[] = { D5, D6, D10, D9, D8, D7, LED_BUILTIN };
	};
};

#else
#error Device is not supported for this project.
#endif

#endif

