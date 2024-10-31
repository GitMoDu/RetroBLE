// BleConfig.h

#ifndef _BLE_CONFIG_h
#define _BLE_CONFIG_h

#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <bluefruit.h>

namespace RetroBle
{
	namespace BleConfig
	{
		static constexpr int8_t TxPower = 2; // Check bluefruit.h for supported values

		enum class Appearance : uint16_t
		{
			GamePad = BLE_APPEARANCE_HID_GAMEPAD,
			Keyboard = BLE_APPEARANCE_HID_KEYBOARD,
			Mouse = BLE_APPEARANCE_HID_MOUSE,
			WeightScale = BLE_APPEARANCE_GENERIC_WEIGHT_SCALE
		};

		/* Set connection interval (min, max) to your perferred value.
		 * Note: It is already set by BLEHidAdafruit::begin() to 11.25ms - 15ms
		 * min = 9*1.25=11.25 ms, max = 12*1.25= 15 ms
		 */
		struct ConnectionInterval
		{
			static constexpr uint8_t Min = 6;
			static constexpr uint8_t Max = 10;
		};

		static constexpr uint32_t BATTERY_UPDATE_PERIOD_MILLIS = 500;

		static constexpr uint32_t ADVERTISE_FAST_TIMEOUT_MILLIS = 15000;
		static constexpr uint32_t ADVERTISE_NO_ACTIVITY_TIMEOUT_MILLIS = 30000;

		static constexpr uint32_t CONNECTED_NO_ACTIVITY_TIMEOUT_MILLIS = 5 * 60000;
	}
}
#endif