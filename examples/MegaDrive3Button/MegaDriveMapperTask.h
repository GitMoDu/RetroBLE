// MegaDriveMapperTask.h

#ifndef _MEGA_DRIVE_MAPPER_TASK_h
#define _MEGA_DRIVE_MAPPER_TASK_h

#include <RetroBle.h>
#include <VirtualPad.h>

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
	VirtualPad::ButtonParser::ActionTimed StartHold{};

private:
	VirtualPad::button_pad_state_t SleepControllerState{};
	VirtualPad::button_pad_state_t WakeControllerState{};

public:
	MegaDriveMapperTask(TS::Scheduler& scheduler,
		MegaDriveVirtualPadType& padSource,
		UsbHidGamepad& usbGamepad,
		BLEHidGamepad& bleGamepad,
		const uint32_t bleUpdatePeriod = 15)
		: HidGamepadTask(scheduler, usbGamepad, bleGamepad, bleUpdatePeriod)
		, Source(padSource)
	{
	}

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
		case VirtualPad::DPadEnum::Up:
			hidReport.hat = GAMEPAD_HAT_UP;
			break;
		case VirtualPad::DPadEnum::UpLeft:
			hidReport.hat = GAMEPAD_HAT_UP_LEFT;
			break;
		case VirtualPad::DPadEnum::UpRight:
			hidReport.hat = GAMEPAD_HAT_UP_RIGHT;
			break;
		case VirtualPad::DPadEnum::Down:
			hidReport.hat = GAMEPAD_HAT_DOWN;
			break;
		case VirtualPad::DPadEnum::DownLeft:
			hidReport.hat = GAMEPAD_HAT_DOWN_LEFT;
			break;
		case VirtualPad::DPadEnum::DownRight:
			hidReport.hat = GAMEPAD_HAT_DOWN_RIGHT;
			break;
		case VirtualPad::DPadEnum::Left:
			hidReport.hat = GAMEPAD_HAT_LEFT;
			break;
		case VirtualPad::DPadEnum::Right:
			hidReport.hat = GAMEPAD_HAT_RIGHT;
			break;
		case VirtualPad::DPadEnum::None:
		default:
			hidReport.hat = GAMEPAD_HAT_CENTERED;
			break;
		}
	}
};

#endif