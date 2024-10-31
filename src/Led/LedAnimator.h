// LedAnimator.h

#ifndef _LED_ANIMATOR_h
#define _LED_ANIMATOR_h

#if defined(_TASK_OO_CALLBACKS)
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
	static constexpr uint32_t BLE_SEARCHING_ANIMATE_PERIOD = 250 / 2;

private:
	ILedDriver* LedDriver;

private:
	uint32_t AnimationStart = 0;
	const uint32_t UpdatePeriod;

	struct DrawStateStruct
	{
		ConnectionLights ConnectionState = ConnectionLights::Off;
		bool Charging = false;
	} DrawState;

public:
	LedAnimatorTask(TS::Scheduler& scheduler,
		ILedDriver* ledDriver,
		const uint32_t updatePeriod = 5)
		: TS::Task(TASK_IMMEDIATE, TASK_FOREVER, &scheduler, false)
		, LedDriver(ledDriver)
		, UpdatePeriod(updatePeriod)
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

	virtual void SetDrawMode(const ConnectionLights connectionState, const bool charging)
	{
		if (DrawState.ConnectionState != connectionState)
		{
			AnimationStart = millis();
		}

		DrawState.Charging = charging;
		DrawState.ConnectionState = connectionState;
		UpdateLedState();

		TS::Task::enableIfNot();
	}

	virtual bool Callback() final
	{
		UpdateLedState();

		switch (DrawState.ConnectionState)
		{
		case ConnectionLights::Off:
			TS::Task::disable();
			break;
		case ConnectionLights::Ble:
		case ConnectionLights::Usb:
		case ConnectionLights::Searching:
			TS::Task::delay(UpdatePeriod);
		default:
			break;
		}

		return true;
	}

private:
	void UpdateLedState()
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
			break;
		case ConnectionLights::Ble:
			blue = true;
			break;
		case ConnectionLights::Usb:
			if (!red)
			{
				green = true;
			}
			break;
		case ConnectionLights::Searching:
			blue = (elapsed % (BLE_SEARCHING_ANIMATE_PERIOD * 2)) <= BLE_SEARCHING_ANIMATE_PERIOD;
			break;
		default:
			break;
		}

		// Write the RGB state to the LED Driver.
		LedDriver->SetRGB(red, green, blue);
	}
};
#endif
#endif