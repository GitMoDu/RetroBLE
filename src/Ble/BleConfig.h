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

		/// <summary>
		/// BLE fastest update period with the lowest latency.
		/// </summary>
		namespace ConnectionIntervalFast
		{
			/// <summary>
			/// Lowest minimum connection interval permitted, in units of 1.25 ms, i.e. 7.5 ms.
			/// </summary>
			static constexpr uint8_t Min = BLE_GAP_CP_MIN_CONN_INTVL_MIN;

			/// <summary>
			/// Lowest maximum connection interval permitted, in units of 1.25 ms, i.e. 7.5 ms.
			/// </summary>
			static constexpr uint8_t Max = BLE_GAP_CP_MAX_CONN_INTVL_MIN;
		};

		/// <summary>
		/// Fast BLE update with some tolerance.
		/// </summary>
		namespace ConnectionIntervalRegular
		{
			/// <summary>
			/// Lowest minimum connection interval permitted with +1 tolerance, in units of 1.25 ms, i.e. 7.5 ms.
			/// </summary>
			static constexpr uint8_t Min = BLE_GAP_CP_MIN_CONN_INTVL_MIN + 1;

			/// <summary>
			/// 1 more unit over the minimum.
			/// </summary>
			static constexpr uint8_t Max = Min + 1;
		};

		static constexpr uint32_t BATTERY_UPDATE_PERIOD_MILLIS = 3000;

		static constexpr uint32_t ADVERTISE_FAST_TIMEOUT_MILLIS = 15000;
		static constexpr uint32_t ADVERTISE_NO_ACTIVITY_TIMEOUT_MILLIS = 30000;

		static constexpr uint32_t CONNECTED_NO_ACTIVITY_TIMEOUT_MILLIS = 5 * 60000;
	}
}
#endif