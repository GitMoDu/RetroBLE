#ifndef _ATARI_SLEEPY_PAD_h
#define _ATARI_SLEEPY_PAD_h

#include <VirtualPads.h>
#include <RetroBle.h>

template<typename ControllerPin,
	uint8_t WakePin>
class AtariSleepyPad : public virtual BatteryManager::ISleep, public AtariJoystick::AtariJoystickVirtualPad<ControllerPin>
{
protected:
	using Base = AtariJoystick::AtariJoystickVirtualPad<ControllerPin>;
	using Base::PIN_INPUT_MODE;

private:
	void (*WakeInterrupt)() = nullptr;

public:
	AtariSleepyPad()
		: BatteryManager::ISleep()
		, Base()
	{
	}

	const bool Setup(void (*onWakeInterrupt)())
	{
		Base::Setup();

		WakeInterrupt = onWakeInterrupt;

		return WakeInterrupt != nullptr;
	}

public:
	virtual void OnWakeUp() final
	{
		pinMode(WakePin, PIN_INPUT_MODE);
		detachInterrupt(digitalPinToInterrupt(WakePin));
#if defined(ARDUINO_Seeed_XIAO_nRF52840_Sense) || defined(ARDUINO_Seeed_XIAO_nRF52840)
		disableInterruptIRQ(digitalPinToInterrupt(WakePin));
#else
		disableInterrupt(digitalPinToInterrupt(WakePin));
#endif
	}

	virtual bool WakeOnInterrupt() final
	{
		if (WakePin != UINT8_MAX)
		{
#if defined(ARDUINO_Seeed_XIAO_nRF52840_Sense) || defined(ARDUINO_Seeed_XIAO_nRF52840)
			enableInterruptIRQ(digitalPinToInterrupt(WakePin));
			attachInterrupt(digitalPinToInterrupt(WakePin), WakeInterrupt, FALLING);

			// Wake up gamepad pin.
			// Pulled up and wakes up when externally connected to GND.
			pinMode(WakePin, INPUT_PULLUP_SENSE);
#else
			pinMode(WakePin, INPUT_PULLUP);
			enableInterrupt(digitalPinToInterrupt(WakePin));
#endif

			return true;
		}

		return false;
	}
};
#endif
