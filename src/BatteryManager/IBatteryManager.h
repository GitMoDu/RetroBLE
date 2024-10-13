// IBatteryManager.h

#ifndef _I_BATTERY_MANAGER_h
#define _I_BATTERY_MANAGER_h

#include "BatteryState.h"

namespace BatteryManager
{
	/// <summary>
	/// Class interface for BMS.
	/// </summary>
	class IBatteryManager
	{
	public:
		virtual void GetBatteryState(BatteryStateStruct& batteryState) {};
		virtual void Start() {};
		virtual void Stop() {};
	};
}
#endif