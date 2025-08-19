#ifndef _HID_KEYBOARD_TASK_h
#define _HID_KEYBOARD_TASK_h

#if defined(_TASK_OO_CALLBACKS)
#include <TSchedulerDeclarations.hpp>

#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <bluefruit.h>

#include "../Usb/UsbHidKeyboard.h"
#include "../BatteryManager/ISleep.h"
#include "IHidDevice.h"

/// <summary>
/// Abstract task for Keyboard HID reporting, with fixed period.
/// Combined HID reporting, for BLE or USB Keyboard.
/// Inherited classes must implement virtual methods.
///////		OnStart - Setup pin and prepare for runtime operation.
///////		OnStop - Stop any active operation and setup pins for wake on interrupt.
///		UpdateState - Update keyboard state and populate HID report.
///		IsPowerDownRequested - Keyboard has requested device to power down.
/// </summary>
class HidKeyboardTask : public virtual IHidDevice, private TS::Task
{
private:
	UsbHidKeyboard& UsbKeyboard;
	BLEHidAdafruit& BleKeyboard;

private:
	const uint32_t UsbPeriod;
	const uint32_t BlePeriod;

private:
	hid_keyboard_report_t HidReport{};
	hid_keyboard_report_t LastHidReport{};

private:
	//void (*WakeInterrupt)() = nullptr; //TODO:
	uint32_t LastActivity = 0;
	TargetEnum Target = TargetEnum::None;

protected:
	virtual void UpdateState(hid_keyboard_report_t& hidReport) {}

	/// <summary>
	/// ISleep interface.
	/// </summary>
public:
	virtual bool WakeOnInterrupt() { return false; }
	virtual void OnWakeUp() {}

	/// <summary>
	/// IHidDevice interface.
	/// </summary>
public:
	virtual bool IsPowerDownRequested() const { return false; }

public:
	HidKeyboardTask(TS::Scheduler& scheduler,
		UsbHidKeyboard& usbKeyboard,
		BLEHidAdafruit& bleKeyboard,
		const uint32_t usbUpdatePeriod = 5,
		const uint32_t bleUpdatePeriod = 15)
		: IHidDevice()
		, TS::Task(TASK_IMMEDIATE, TASK_FOREVER, &scheduler, false)
		, UsbKeyboard(usbKeyboard)
		, BleKeyboard(bleKeyboard)
		, UsbPeriod(usbUpdatePeriod)
		, BlePeriod(bleUpdatePeriod)
	{
	}

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
			if (UsbKeyboard.IsReady())
			{
				UsbKeyboard.NotifyKeyboard(HidReport);
			}
			TS::Task::delay(UsbPeriod);
			break;
		case TargetEnum::Ble:
			BleKeyboard.keyboardReport(&HidReport);
			TS::Task::delay(BlePeriod);
			break;
		case TargetEnum::None:
		default:
			TS::Task::delay(BlePeriod);
			break;
		}

		if ((LastHidReport.modifier != HidReport.modifier))
		{
			LastHidReport.modifier = HidReport.modifier;
			memcpy(LastHidReport.keycode, HidReport.keycode, sizeof(hid_keyboard_report_t::keycode));

			OnActivity();
			return true;
		}

		for (size_t i = 0; i < sizeof(hid_keyboard_report_t::keycode); i++)
		{
			if ((LastHidReport.keycode[i] != HidReport.keycode[i]))
			{
				LastHidReport.modifier = HidReport.modifier;
				memcpy(LastHidReport.keycode, HidReport.keycode, sizeof(hid_keyboard_report_t::keycode));

				OnActivity();
				return true;
			}
		}

		return true;
	}

	void SetTarget(TargetEnum target) final
	{
		if (Target != target)
		{
			Target = target;
			TS::Task::enableDelayed(0);
		}
	}

	uint32_t GetElapsedMillisSinceLastActivity() const final
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