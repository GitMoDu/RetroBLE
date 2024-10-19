// UsbPadMapperTask.h

#ifndef _USB_PAD_MAPPER_h
#define _USB_PAD_MAPPER_h

#define _TASK_OO_CALLBACKS
#include <TaskSchedulerDeclarations.h>

#include <Arduino.h>

#include <VirtualPad.h>
#include <Adafruit_TinyUSB.h>

template<typename MegaDriveVirtualPad,
	uint32_t UpdatePeriod>
class UsbPadMapperTask : private TS::Task
{
private:
	MegaDriveVirtualPad& Source;

	UsbHidGamepad& UsbGamepad;

private:
	// Report payload defined in src/class/hid/hid.h
	// - For Gamepad Button Bit Mask see  hid_gamepad_button_bm_t
	// - For Gamepad Hat    Bit Mask see  hid_gamepad_hat_t
	hid_gamepad_report_t HidReport{};
	hid_gamepad_report_t LastHidReport{};

	uint32_t LastActivity = 0;

public:
	UsbPadMapperTask(TS::Scheduler& scheduler,
		MegaDriveVirtualPad& source,
		UsbHidGamepad& usbGamepad)
		: TS::Task(UpdatePeriod, TASK_FOREVER, &scheduler, false)
		, Source(source)
		, UsbGamepad(usbGamepad)
	{}

	void Start()
	{
		TS::Task::enable();
		Source.Setup();

		HidReport.buttons = 0;
		HidReport.hat = 0;
		HidReport.x = 0;
		HidReport.y = 0;
		HidReport.z = 0;
		HidReport.rx = 0;
		HidReport.ry = 0;
		HidReport.rz = 0;

		LastHidReport.buttons = HidReport.buttons;
		LastHidReport.hat = HidReport.hat;

		LastActivity = millis();
	}

	void Stop()
	{
		Source.Setup();
		HidReport.buttons = 0;
		HidReport.hat = 0;
		TS::Task::disable();
	}

	const bool IsConnected() const
	{
		return Source.Connected();
	}

	virtual bool Callback() final
	{
		Source.Step();

		// Same mapping as 8BitDo M30 and S30.
		HidReport.buttons =
			(GAMEPAD_BUTTON_0 * Source.A())
			| (GAMEPAD_BUTTON_1 * Source.B())
			| (GAMEPAD_BUTTON_6 * Source.X())
			| (GAMEPAD_BUTTON_8 * Source.Start());

		switch (Source.DPad())
		{
		case DPadEnum::Up:
			HidReport.hat = GAMEPAD_HAT_UP;
			break;
		case DPadEnum::UpLeft:
			HidReport.hat = GAMEPAD_HAT_UP_LEFT;
			break;
		case DPadEnum::UpRight:
			HidReport.hat = GAMEPAD_HAT_UP_RIGHT;
			break;

		case DPadEnum::Down:
			HidReport.hat = GAMEPAD_HAT_DOWN;
			break;
		case DPadEnum::DownLeft:
			HidReport.hat = GAMEPAD_HAT_DOWN_LEFT;
			break;
		case DPadEnum::DownRight:
			HidReport.hat = GAMEPAD_HAT_DOWN_RIGHT;
			break;

		case DPadEnum::Left:
			HidReport.hat = GAMEPAD_HAT_LEFT;
			break;
		case DPadEnum::Right:
			HidReport.hat = GAMEPAD_HAT_RIGHT;
			break;
		case DPadEnum::None:
		default:
			HidReport.hat = GAMEPAD_HAT_CENTERED;
			break;
		}

		if (UsbGamepad.IsReady()
			&& ((LastHidReport.buttons != HidReport.buttons)
				|| (LastHidReport.hat != HidReport.hat))
			)
		{
			UsbGamepad.NotifyGamepad(HidReport);

			LastActivity = millis();
			LastHidReport.buttons = HidReport.buttons;
			LastHidReport.hat = HidReport.hat;
		}

		return true;
	}

	const uint32_t GetElapsedMillisSinceLastActivity()
	{
		return millis() - LastActivity;
	}
};
#endif