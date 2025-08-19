#ifndef _I_HID_DEVICE_h
#define _I_HID_DEVICE_h

#include "../BatteryManager/ISleep.h"

struct IHidDevice : BatteryManager::ISleep
{
	enum class TargetEnum : uint8_t
	{
		None,
		Usb,
		Ble
	};

	virtual void SetTarget(TargetEnum target) = 0;

	virtual uint32_t GetElapsedMillisSinceLastActivity() const = 0;

	virtual bool IsPowerDownRequested() const = 0;
};
#endif