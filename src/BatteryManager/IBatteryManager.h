// IBatteryManager.h

#ifndef _I_BATTERY_MANAGER_h
#define _I_BATTERY_MANAGER_h

#include "BatteryState.h"
#include "ISleep.h"

namespace BatteryManager
{
	/// <summary>
	/// Class interface for BMS.
	/// </summary>
	class IBatteryManager : public virtual BatteryManager::ISleep
	{
	public:
		virtual void GetBatteryState(BatteryStateStruct& batteryState) {}
	};
}
#endif