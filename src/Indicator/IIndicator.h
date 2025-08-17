#ifndef _I_INDICATOR_h
#define _I_INDICATOR_h

#include <stdint.h>

/// <summary>
/// Indicator class interface.
/// </summary>
class IIndicator
{
public:
	enum class StateEnum
	{
		Off,
		Ble,
		Usb,
		Searching
	};

public:
	virtual void SetDrawMode(const IIndicator::StateEnum indicatorState, const bool charging) {}
};
#endif