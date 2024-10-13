// ILedDriver.h

#ifndef _I_LED_DRIVER_h
#define _I_LED_DRIVER_h

#include <stdint.h>

/// <summary>
/// LED Driver class interface.
/// </summary>
class ILedDriver
{
public:
	virtual void SetRGB(const bool red, const bool green, const bool blue) {};
};
#endif