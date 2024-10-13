// ILightsDriver.h

#ifndef _I_LIGHTS_DRIVER_h
#define _I_LIGHTS_DRIVER_h

#include <stdint.h>

/// <summary>
/// Addressable Lights Driver class interface.
/// </summary>
class ILightsDriver
{
public:
	virtual void SetRGB(const uint8_t index, const uint8_t red, const uint8_t green, const uint8_t blue) {};
	virtual void Sync() {};

	virtual void Start() {};
	virtual void Stop() {};
};
#endif