// Ws2812Driver.h

#ifndef _WS_2812_DRIVER_h
#define _WS_2812_DRIVER_h

#include "../ILightsDriver.h"

#include <Adafruit_NeoPixel.h>

/// <summary>
/// Depends on Adafruit_NeoPixel library.
/// </summary>
/// <typeparam name="Pin"></typeparam>
template<typename Pin>
class LightsDriver : public ILightsDriver
{
private:
	static constexpr uint8_t LedCount = 1;
	static constexpr neoPixelType NeoPixelType = NEO_GRB + NEO_KHZ800;
	static constexpr uint8_t BrightnessDefault = 10;

private:
	Adafruit_NeoPixel Driver = Adafruit_NeoPixel();

public:
	LightsDriver() : ILightsDriver()
	{}

	void Setup()
	{
		pinMode(Pin::Out, OUTPUT);
		digitalWrite(Pin::Out, LOW);
		Driver.begin();
		Driver.show(); // Turn OFF all pixels ASAP.
		Driver.updateLength(LedCount);
		Driver.updateType(NeoPixelType);
		Driver.setPin(Pin::Out);
		Driver.setBrightness(BrightnessDefault);
	}

	virtual const uint8_t GetLedCount() final { return 1; }

	virtual void SetRgb(const uint8_t index, const uint8_t red, const uint8_t green, const uint8_t blue) final
	{
		Driver.setPixelColor(0, Driver.Color(red, green, blue));
	}

	virtual void Sync() final
	{
		Driver.show();
	}

	virtual void Start() final
	{
		pinMode(Pin::Out, OUTPUT);
		digitalWrite(Pin::Out, LOW);
	}

	virtual void Stop() final
	{
		pinMode(Pin::Out, OUTPUT);
		digitalWrite(Pin::Out, LOW);
		pinMode(Pin::Out, INPUT);
	}
};

#endif