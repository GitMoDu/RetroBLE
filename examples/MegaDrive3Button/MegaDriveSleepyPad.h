// MegaDriveSleepyPad.h

#ifndef _MEGA_DRIVE_SLEEPY_PAD_h
#define _MEGA_DRIVE_SLEEPY_PAD_h

#include <VirtualPads.h>
#include <RetroBle.h>

template<typename ControllerPin,
	const uint8_t WakePin>
class MegaDriveSleepyPad 
	: public virtual BatteryManager::ISleep
	, public MegaDriveController::MegaDriveControllerVirtualPad<ControllerPin>
{
protected:
	using Base = MegaDriveController::MegaDriveControllerVirtualPad<ControllerPin>;
	using Base::PIN_INPUT_MODE;

private:
	void (*WakeInterrupt)() = nullptr;

public:
	MegaDriveSleepyPad()
		: BatteryManager::ISleep()
		, Base()
	{}

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

	virtual const bool WakeOnInterrupt() final
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
