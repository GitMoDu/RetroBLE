// MegaDriveMapperTask.h

#ifndef _MEGA_DRIVE_MAPPER_TASK_h
#define _MEGA_DRIVE_MAPPER_TASK_h

#include <RetroBle.h>

/// <summary>
/// Maps VirtualPad to native RetroArch's RetroPad, targeting the Mega Drive 3 button layout.
/// See more at https://docs.libretro.com/library/genesis_plus_gx/#joypad
/// </summary>
/// <typeparam name="MegaDriveVirtualPadType"></typeparam>
/// <typeparam name="PowerDownHoldDuration"></typeparam>
template<typename MegaDriveVirtualPadType,
	const uint32_t PowerDownHoldDuration>
class MegaDriveMapperTask : public HidGamepadTask
{
private:
	MegaDriveVirtualPadType& Source;
	ButtonParser::ActionTimed StartHold{};

private:
	virtual_pad_state_t SleepControllerState{};
	virtual_pad_state_t WakeControllerState{};

public:
	MegaDriveMapperTask(TS::Scheduler& scheduler,
		MegaDriveVirtualPadType& padSource,
		UsbHidGamepad& usbGamepad,
		BLEHidGamepad& bleGamepad,
		const uint32_t usbUpdatePeriod = 5,
		const uint32_t bleUpdatePeriod = 15)
		: HidGamepadTask(scheduler, usbGamepad, bleGamepad, usbUpdatePeriod, bleUpdatePeriod)
		, Source(padSource)
	{}

public:
	virtual const bool IsPowerDownRequested() final
	{
		return StartHold.ActionDown(PowerDownHoldDuration);
	}

	/// <summary>
	/// Sleep peripheral and setup wake event.
	/// </summary>
	/// <returns>False if sleep isn't possible right now.</returns>
	virtual const bool WakeOnInterrupt() final
	{
		return Source.WakeOnInterrupt();
	}

	/// <summary>
	/// Device has just woken up from sleep, restore peripheral.
	/// </summary>
	virtual void OnWakeUp() final
	{
		StartHold.Clear();
		Source.Read();
		Source.OnWakeUp();
	}

protected:
	virtual void UpdateState(hid_gamepad_report_t& hidReport) final
	{
		Source.Read();

		StartHold.Parse(millis(), Source.Start());

		// Mapped to native RetroArch's RetroPad.
		hidReport.buttons =
			(GAMEPAD_BUTTON_Y * Source.A())
			| (GAMEPAD_BUTTON_B * Source.B())
			| (GAMEPAD_BUTTON_A * Source.R3())
			| (GAMEPAD_BUTTON_START * Source.Start());

		switch (Source.DPad())
		{
		case DPadEnum::Up:
			hidReport.hat = GAMEPAD_HAT_UP;
			break;
		case DPadEnum::UpLeft:
			hidReport.hat = GAMEPAD_HAT_UP_LEFT;
			break;
		case DPadEnum::UpRight:
			hidReport.hat = GAMEPAD_HAT_UP_RIGHT;
			break;
		case DPadEnum::Down:
			hidReport.hat = GAMEPAD_HAT_DOWN;
			break;
		case DPadEnum::DownLeft:
			hidReport.hat = GAMEPAD_HAT_DOWN_LEFT;
			break;
		case DPadEnum::DownRight:
			hidReport.hat = GAMEPAD_HAT_DOWN_RIGHT;
			break;
		case DPadEnum::Left:
			hidReport.hat = GAMEPAD_HAT_LEFT;
			break;
		case DPadEnum::Right:
			hidReport.hat = GAMEPAD_HAT_RIGHT;
			break;
		case DPadEnum::None:
		default:
			hidReport.hat = GAMEPAD_HAT_CENTERED;
			break;
		}
	}
};

#endif