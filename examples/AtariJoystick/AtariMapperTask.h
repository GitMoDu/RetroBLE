// AtariMapperTask.h

#ifndef _ATARI_MAPPER_TASK_h
#define _ATARI_MAPPER_TASK_h

#include <RetroBle.h>

/// <summary>
/// Maps VirtualPad to native RetroArch's RetroPad, targeting the Atari joystick controller.
/// </summary>
/// <typeparam name="AtariVirtualPadType"></typeparam>
template<typename AtariVirtualPadType>
class AtariMapperTask : public HidGamepadTask
{
private:
	AtariVirtualPadType& Source;

private:
	VirtualPad::button_pad_state_t SleepControllerState{};
	VirtualPad::button_pad_state_t WakeControllerState{};

public:
	AtariMapperTask(TS::Scheduler& scheduler,
		AtariVirtualPadType& padSource,
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
		return false;
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
		Source.Read();
		Source.OnWakeUp();
	}

protected:
	virtual void UpdateState(hid_gamepad_report_t& hidReport) final
	{
		Source.Read();

		// Mapped to native RetroArch's RetroPad.
		hidReport.buttons = GAMEPAD_BUTTON_A * Source.A();

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