// BatteryState.h

#ifndef _BATTERY_STATE_h
#define _BATTERY_STATE_h

namespace BatteryManager
{
	/// <summary>
	/// Charge level range [0 ; UINT8_MAX].
	/// </summary>
	static constexpr uint8_t ChargeLevelMax = UINT8_MAX;

	/// <summary>
	/// Battery state model.
	/// </summary>
	struct BatteryStateStruct
	{
		uint8_t ChargeLevel = 0;
		bool Charging = false;
	};

	/// <summary>
	/// Get charge level in percentage from raw charge level.
	/// </summary>
	/// <param name="chargeLevel">Charge level [0 ; ChargeLevelMax]</param>
	/// <returns>Charge level in percentage [0% ; 100%].</returns>
	const uint8_t ChargeLevelPercent(const uint8_t chargeLevel)
	{
		if (chargeLevel < ChargeLevelMax)
		{
			return (((uint16_t)chargeLevel) * (uint16_t)100) / ChargeLevelMax;
		}
		else
		{
			return 100;
		}
	}

	/// <summary>
	/// Get charge level in percentage from battery state.
	/// </summary>
	/// <param name="batteryState"></param>
	/// <returns>Charge level in percentage [0% ; 100%].</returns>
	const uint8_t ChargeLevelPercent(BatteryStateStruct& batteryState)
	{
		return ChargeLevelPercent(batteryState.ChargeLevel);
	}
}
#endif