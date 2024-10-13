// LedAnimator.h

#ifndef _LED_ANIMATOR_h
#define _LED_ANIMATOR_h

#define _TASK_OO_CALLBACKS
#include <TaskSchedulerDeclarations.h>

#include "ILedDriver.h"

/// <summary>
/// Animates onboard RGB LED according to the GamePad connection, battery level and charge state.
/// </summary>
class LedAnimatorTask : private TS::Task
{
public:
	enum class ConnectionLights
	{
		Off,
		Ble,
		Usb,
		Searching
	};

private:
	static constexpr uint32_t BLE_SEARCHING_ANIMATE_PERIOD = 250;

private:
	ILedDriver* LedDriver;

private:
	uint32_t AnimationStart = 0;

	struct DrawStateStruct
	{
		ConnectionLights ConnectionState = ConnectionLights::Off;
		bool Charging = false;
	} DrawState;

public:
	LedAnimatorTask(TS::Scheduler& scheduler,
		ILedDriver* ledDriver,
		const uint32_t updatePeriod = 5)
		: TS::Task(updatePeriod, TASK_FOREVER, &scheduler, false)
		, LedDriver(ledDriver)
	{}

	void Start()
	{
		if (LedDriver != nullptr)
		{
			TS::Task::enable();
			LedOff();
			AnimationStart = millis();
		}
	}

	void Stop()
	{
		TS::Task::disable();
		LedOff();
	}

	void LedOff()
	{
		LedDriver->SetRGB(false, false, false);
	}

	virtual void SetDrawMode(const ConnectionLights connectionState, const bool charging = false)
	{
		if (DrawState.ConnectionState != connectionState)
		{
			AnimationStart = millis();
		}

		DrawState.Charging = charging;
		DrawState.ConnectionState = connectionState;

		TS::Task::enable();

		LedDriver->SetRGB(DrawState.Charging, false, false);
	}

	virtual bool Callback() final
	{
		const uint32_t timestamp = millis();
		const uint32_t elapsed = timestamp - AnimationStart;

		bool red = false;
		bool blue = false;
		bool green = false;

		red = DrawState.Charging;

		switch (DrawState.ConnectionState)
		{
		case ConnectionLights::Off:
			TS::Task::disable();
			break;
		case ConnectionLights::Ble:
			blue = true;
			TS::Task::disable();
			break;
		case ConnectionLights::Usb:
			green = true;
			TS::Task::disable();
			break;
		case ConnectionLights::Searching:
			blue = (elapsed % (BLE_SEARCHING_ANIMATE_PERIOD * 2)) <= BLE_SEARCHING_ANIMATE_PERIOD;
			break;
		default:
			break;
		}

		// Write the RGB state to the LED Driver.
		LedDriver->SetRGB(red, green, blue);

		return true;
	}
};


#endif