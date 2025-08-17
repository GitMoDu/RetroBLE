// IHidDevice.h

#ifndef _I_HID_DEVICE_h
#define _I_HID_DEVICE_h

#include "../BatteryManager/ISleep.h"

class IHidDevice : public virtual BatteryManager::ISleep
{
public:
	enum class TargetEnum : uint8_t
	{
		None,
		Usb,
		Ble
	};

public:
	virtual void SetTarget(TargetEnum target) = 0;
	virtual uint32_t GetElapsedMillisSinceLastActivity() const  = 0; //const { return UINT32_MAX; }

public:
	virtual bool IsPowerDownRequested() const = 0; //{ return false; }
};

#endif