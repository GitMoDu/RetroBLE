// LedDriver.h

#ifndef _LED_DRIVER_h
#define _LED_DRIVER_h

#include <Arduino.h>
#include <stdint.h>

#include "../ILedDriver.h"

/// <summary>
/// Native driver for RGB LED with templated pin mapping and on-state.
/// </summary>
/// <typeparam name="Pin">RGB Pin map type = { uint8_t Red = LED_RED; uint8_t Green = LED_GREEN; uint8_t Blue = LED_BLUE; };</typeparam>
/// <typeparam name="OnState">IO state for LED On.</typeparam>
template<typename Pin,
	const bool OnState = LOW>
class LedDriver : public virtual ILedDriver
{
public:
	LedDriver() : ILedDriver()
	{}

	void Setup()
	{
		pinMode(Pin::Red, OUTPUT);
		digitalWrite(Pin::Red, HIGH);

		pinMode(Pin::Green, OUTPUT);
		digitalWrite(Pin::Blue, HIGH);

		pinMode(Pin::Blue, OUTPUT);
		digitalWrite(Pin::Green, HIGH);
	}

	virtual void SetRGB(const bool red, const bool green, const bool blue) final
	{
		digitalWrite(Pin::Red, IoState(red));
		digitalWrite(Pin::Green, IoState(green));
		digitalWrite(Pin::Blue, IoState(blue));
	}

private:
	static const bool IoState(const bool state)
	{
		if (OnState)
		{
			return state;
		}
		else
		{
			return !state;
		}
	}
};
#endif