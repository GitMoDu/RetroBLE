// Device.h

#ifndef _DEVICE_h
#define _DEVICE_h

#include <Arduino.h>
#include <RetroBle.h>


#if defined(ARDUINO_Seeed_XIAO_nRF52840_Sense) || defined(ARDUINO_Seeed_XIAO_nRF52840)
#include <variant.h>

namespace Device
{
	namespace Debug
	{
		static constexpr uint32_t SERIAL_BAUD_RATE = 921600;
	}

	namespace BMS
	{
		using Pin = SeeedXIAOnRF52840::BMS::Pin;
		using Calibration = SeeedXIAOnRF52840::BMS::Bq25100Calibration;

		static constexpr uint8_t HistorySize = 100;
		static constexpr uint32_t UpdatePeriodMillis = 33;
	}
};
#else
#error Device is not supported for this project.
#endif

#endif

