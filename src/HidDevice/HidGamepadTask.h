// HidGamepadTask.h

#ifndef _HID_GAMEPAD_TASK_h
#define _HID_GAMEPAD_TASK_h

#if defined(_TASK_OO_CALLBACKS)
#include <TSchedulerDeclarations.hpp>

#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <bluefruit.h>

#include "../Usb/UsbHidGamepad.h"
#include "../BatteryManager/ISleep.h"
#include "IHidDevice.h"

/// <summary>
/// Abstract task for Gamepad HID reporting, with fixed period update.
/// Combined HID reporting, for BLE or USB Gamepad.
/// Must implement ISleep interface for power life-cycle.
/// Must implement virtual methods.
///		UpdateState - Update controller state and populate HID report.
///		IsPowerDownRequested - Controller has requested device to power down.
/// </summary>
class HidGamepadTask : public virtual IHidDevice, private TS::Task
{
private:
	UsbHidGamepad& UsbGamepad;
	BLEHidGamepad& BleGamepad;

private:
	const uint32_t BlePeriod;

private:
	hid_gamepad_report_t HidReport{};
	hid_gamepad_report_t LastHidReport{};

private:
	uint32_t LastActivity = 0;
	TargetEnum Target = TargetEnum::None;

protected:
	virtual void UpdateState(hid_gamepad_report_t& hidReport) {}

	/// <summary>
	/// ISleep interface.
	/// </summary>
public:
	virtual const bool WakeOnInterrupt() { return false; }
	virtual void OnWakeUp() { }

	/// <summary>
	/// IHidDevice interface.
	/// </summary>
public:
	virtual const bool IsPowerDownRequested() { return false; }

public:
	HidGamepadTask(TS::Scheduler& scheduler,
		UsbHidGamepad& usbGamepad,
		BLEHidGamepad& bleGamepad,
		const uint32_t bleUpdatePeriod = 15)
		: IHidDevice()
		, TS::Task(TASK_IMMEDIATE, TASK_FOREVER, &scheduler, false)
		, UsbGamepad(usbGamepad)
		, BleGamepad(bleGamepad)
		, BlePeriod(bleUpdatePeriod)
	{}

	void OnWakeInterrupt()
	{
		LastActivity = millis();
	}

	virtual bool Callback() final
	{
		UpdateState(HidReport);

		switch (Target)
		{
		case TargetEnum::Usb:
			if (UsbGamepad.IsReady())
			{
				UsbGamepad.NotifyGamepad(HidReport);
			}
			TS::Task::delay(1);
			break;
		case TargetEnum::Ble:
			BleGamepad.report(&HidReport);
			TS::Task::delay(BlePeriod);
			break;
		case TargetEnum::None:
		default:
			TS::Task::delay(BlePeriod);
			break;
		}

		if ((LastHidReport.buttons != HidReport.buttons)
			|| (LastHidReport.hat != HidReport.hat))
		{
			LastHidReport.buttons = HidReport.buttons;
			LastHidReport.hat = HidReport.hat;

			OnActivity();
		}

		return true;
	}

	virtual void SetTarget(TargetEnum target) final
	{
		if (Target != target)
		{
			Target = target;
			TS::Task::enableDelayed(0);
		}
	}

	virtual const uint32_t GetElapsedMillisSinceLastActivity() const final
	{
		return millis() - LastActivity;
	}

protected:
	void OnActivity()
	{
		LastActivity = millis();
	}
};
#endif
#endif