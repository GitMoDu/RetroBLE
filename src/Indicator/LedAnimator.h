// LedAnimator.h

#ifndef _LED_ANIMATOR_h
#define _LED_ANIMATOR_h

#if defined(_TASK_OO_CALLBACKS)
#include <TSchedulerDeclarations.hpp>

#include "IIndicator.h"
#include "ILedDriver.h"

/// <summary>
/// Animates onboard RGB LED according to the GamePad connection, battery level and charge state.
/// </summary>
class LedAnimatorTask : public virtual IIndicator, private TS::Task
{
private:
	static constexpr uint32_t BLE_SEARCHING_ANIMATE_PERIOD = 250 / 2;

private:
	ILedDriver* LedDriver;

private:
	uint32_t AnimationStart = 0;
	const uint32_t UpdatePeriod;

	struct DrawStateStruct
	{
		IIndicator::StateEnum State = IIndicator::StateEnum::Off;
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

	virtual void SetDrawMode(const IIndicator::StateEnum indicatorState, const bool charging) final
	{
		if (DrawState.State != indicatorState)
		{
			AnimationStart = millis();
		}

		DrawState.Charging = charging;
		DrawState.State = indicatorState;
		UpdateLedState();

		TS::Task::enableIfNot();
	}

	virtual bool Callback() final
	{
		UpdateLedState();

		switch (DrawState.State)
		{
		case IIndicator::StateEnum::Off:
			TS::Task::disable();
			break;
		case IIndicator::StateEnum::Ble:
		case IIndicator::StateEnum::Usb:
		case IIndicator::StateEnum::Searching:
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

		switch (DrawState.State)
		{
		case IIndicator::StateEnum::Off:
			break;
		case IIndicator::StateEnum::Ble:
			blue = true;
			break;
		case IIndicator::StateEnum::Usb:
			if (!red)
			{
				green = true;
			}
			break;
		case IIndicator::StateEnum::Searching:
			blue = (elapsed % (BLE_SEARCHING_ANIMATE_PERIOD * 2)) <= BLE_SEARCHING_ANIMATE_PERIOD;
			break;
		default:
			break;
		}

		// Write the RGB state to the LED Driver.
		if (LedDriver != nullptr)
		{
			LedDriver->SetRGB(red, green, blue);
		}
	}
};
#endif
#endif