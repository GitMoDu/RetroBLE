#ifndef _HID_TO_VIRTUAL_PAD_h
#define _HID_TO_VIRTUAL_PAD_h

#include <VirtualPad.h>

#include "../Ble/BleCentral.h"
#include "XBoxControllerHid.h"


enum class HidMapTypeEnum : uint16_t
{
	XBox = 10829,
	// TODO:
	GenericGamepad = 0
};


/// <summary>
/// HID source controller, mapped to Virtual Pad.
/// </summary>
template<uint32_t hidConfigurationCode>
class HidToVirtualPad : public virtual IHidListener, public VirtualPad::AnalogVirtualPad<hidConfigurationCode>
{
private:
	using Base = VirtualPad::AnalogVirtualPad<hidConfigurationCode>;

public:
	HidToVirtualPad()
		: IHidListener()
		, Base()
	{
	}

public:
	virtual void OnStateChange(const bool connected)
	{
		if (Base::Connected() != connected)
		{
			Base::Clear();
			Base::SetConnected(connected);
		}
	}

#if defined(DEBUG)
	uint16_t LastUuid = UINT16_MAX;
#endif

	/// <summary>
	/// Dynamically maps the report data to VirtualPad, matched by the UUID.
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <param name="uuid"></param>
	virtual void OnControllerNotify(uint8_t* data, const uint16_t size, const uint16_t uuid) final
	{
		if (data != nullptr)
		{
#if defined(DEBUG)
			if (LastUuid != uuid)
			{
				LastUuid = uuid;
				Serial.print(F("\tProfile switched to \t"));
				Serial.println(uuid);
			}
#endif

			// Check for UUID match.
			switch ((HidMapTypeEnum)uuid)
			{
			case HidMapTypeEnum::XBox:
				HidMapXBox(data, size);
				break;
			case HidMapTypeEnum::GenericGamepad:
			default:
				HidMapGenericGamepad(data, size);
				break;
			}
		}
		else
		{
			Base::SetConnected(false);
		}
	}

private:
	// TODO
	void HidMapGenericGamepad(uint8_t* data, const uint16_t len)
	{
		if (Base::Connected())
		{
			Base::SetConnected(false);
		}
	}

	void HidMapXBox(uint8_t* data, const uint16_t len)
	{
		if (len >= XBoxControllerHid::DataSize)
		{
			// Bit aligned conversion.
			const int16_t x1 = (data[0] | (uint16_t)data[1] << 8) + (uint16_t)INT16_MAX + 1;
			const int16_t y1 = -((data[2] | (uint16_t)data[3] << 8) - (uint16_t)INT16_MAX);
			const int16_t x2 = (data[4] | (uint16_t)data[5] << 8) - (uint16_t)INT16_MAX - 1;
			const int16_t y2 = -((data[6] | (uint16_t)data[7] << 8) - (uint16_t)INT16_MAX);
			Base::SetJoy1(x1, y1);
			Base::SetJoy2(x2, y2);

			// Scale to uint16_t full range.
			const uint16_t l2 = ((uint32_t)(data[8] | (uint16_t)data[9] << 8) * UINT16_MAX) / XBoxControllerHid::TriggerMax;
			const uint16_t r2 = ((uint32_t)(data[10] | (uint16_t)data[11] << 8) * UINT16_MAX) / XBoxControllerHid::TriggerMax;
			Base::SetL2(l2);
			Base::SetR2(r2);

			// Virtual Pad DPadEnum is derived from HID DPad and is compatible with XBox mapping.
			Base::dPad = VirtualPad::DPadEnum(data[12]);

			// First byte of buttons.
			const uint8_t buttons1 = data[13];
			Base::SetA(GetButton<(uint8_t)XBoxControllerHid::Buttons1::A>(buttons1));
			Base::SetB(GetButton<(uint8_t)XBoxControllerHid::Buttons1::B>(buttons1));
			Base::SetX(GetButton<(uint8_t)XBoxControllerHid::Buttons1::X>(buttons1));
			Base::SetY(GetButton<(uint8_t)XBoxControllerHid::Buttons1::Y>(buttons1));
			Base::SetL1(GetButton<(uint8_t)XBoxControllerHid::Buttons1::L1>(buttons1));
			Base::SetR1(GetButton<(uint8_t)XBoxControllerHid::Buttons1::R1>(buttons1));

			// Second byte of buttons.
			const uint8_t buttons2 = data[14];
			Base::SetSelect(GetButton<(uint8_t)XBoxControllerHid::Buttons2::Select>(buttons2));
			Base::SetStart(GetButton<(uint8_t)XBoxControllerHid::Buttons2::Start>(buttons2));
			Base::SetL3(GetButton<(uint8_t)XBoxControllerHid::Buttons2::L3>(buttons2));
			Base::SetR3(GetButton<(uint8_t)XBoxControllerHid::Buttons2::R3>(buttons2));
			Base::SetHome(GetButton<(uint8_t)XBoxControllerHid::Buttons2::Home>(buttons2));

			// Third byte of buttons.
			const uint8_t buttons3 = data[15];
			Base::SetShare(GetButton<(uint8_t)XBoxControllerHid::Buttons3::Share>(buttons3));

			if (!Base::Connected())
			{
				Base::SetConnected(true);
			}
		}
	}

private:
	template<uint8_t BitShift>
	static constexpr bool GetButton(const uint8_t value)
	{
		return (value >> (uint8_t)BitShift) & 1;
	}

#if defined(DEBUG)
public:
	void LogState()
	{
		if (Base::Connected())
		{
			Serial.println();
			Serial.print(F("Navigation\t"));
			if (Base::Home())
			{
				Serial.print(F("Home "));
			}
			if (Base::GetAccept())
			{
				Serial.print(F("Accept "));
			}
			if (Base::GetReject())
			{
				Serial.print(F("Reject "));
			}
			Serial.println();

			Serial.print(F("DPad\t"));
			switch (Base::DPad())
			{
			case VirtualPad::DPadEnum::Up:
				Serial.println(F("Up"));
				break;
			case VirtualPad::DPadEnum::UpRight:
				Serial.println(F("UpRight"));
				break;
			case VirtualPad::DPadEnum::Right:
				Serial.println(F("Right"));
				break;
			case VirtualPad::DPadEnum::DownRight:
				Serial.println(F("DownRight"));
				break;
			case VirtualPad::DPadEnum::Down:
				Serial.println(F("Down"));
				break;
			case VirtualPad::DPadEnum::DownLeft:
				Serial.println(F("DownLeft"));
				break;
			case VirtualPad::DPadEnum::Left:
				Serial.println(F("Left"));
				break;
			case VirtualPad::DPadEnum::UpLeft:
				Serial.println(F("UpLeft"));
				break;
			case VirtualPad::DPadEnum::None:
			default:
				Serial.println(F("Center"));
				break;
			}

			Serial.print(F("Main Buttons\t"));

			if (Base::FeatureA())
			{
				if (Base::A())
				{
					Serial.print(F("A  "));
				}
				else
				{
					Serial.print(F("__ "));
				}
			}

			if (Base::FeatureB())
			{
				if (Base::B())
				{
					Serial.print(F("B  "));
				}
				else
				{
					Serial.print(F("__ "));
				}
			}

			if (Base::FeatureX())
			{
				if (Base::X())
				{
					Serial.print(F("X  "));
				}
				else
				{
					Serial.print(F("__ "));
				}
			}

			if (Base::FeatureY())
			{
				if (Base::Y())
				{
					Serial.print(F("Y  "));
				}
				else
				{
					Serial.print(F("__ "));
				}
			}

			if (Base::FeatureL1())
			{
				if (Base::L1())
				{
					Serial.print(F("L1 "));
				}
				else
				{
					Serial.print(F("__ "));
				}
			}

			if (Base::FeatureR1())
			{
				if (Base::R1())
				{
					Serial.print(F("R1 "));
				}
				else
				{
					Serial.print(F("__ "));
				}
			}

			if (Base::FeatureL3())
			{
				if (Base::L3())
				{
					Serial.print(F("L3 "));
				}
				else
				{
					Serial.print(F("__ "));
				}
			}

			if (Base::FeatureR3())
			{
				if (Base::R3())
				{
					Serial.print(F("R3 "));
				}
				else
				{
					Serial.print(F("__ "));
				}
			}
			Serial.println();

			Serial.print(F("Menu Buttons\t"));
			if (Base::Start())
			{
				Serial.print(F("Start "));
			}
			if (Base::Select())
			{
				Serial.print(F("Select "));
			}
			if (Base::Home())
			{
				Serial.print(F("Home "));
			}
			if (Base::Share())
			{
				Serial.print(F("Share "));
			}
			Serial.println();

			Serial.print(F("Joy1(x,y)\t("));
			Serial.print(Base::Joy1X());
			Serial.print(',');
			Serial.print(Base::Joy1Y());
			Serial.println(')');

			if (Base::FeatureJoy2())
			{
				Serial.print(F("Joy2(x,y)\t("));
				Serial.print(Base::Joy2X());
				Serial.print(',');
				Serial.print(Base::Joy2Y());
				Serial.println(')');
			}

			if (Base::FeatureL2())
			{
				Serial.print(F("L2\t"));
				Serial.println(Base::L2());
			}

			if (Base::FeatureR2())
			{
				Serial.print(F("R2\t"));
				Serial.println(Base::R2());
			}
		}
	}

	void LogFeatures()
	{
		Serial.println(F("Features: "));

		if (Base::FeatureStart() || Base::FeatureSelect() || Base::FeatureHome() || Base::FeatureShare())
		{
			Serial.print(F("\tMenu Buttons: "));
			if (Base::FeatureStart()) { Serial.print(F("Start ")); }
			if (Base::FeatureSelect()) { Serial.print(F("Select ")); }
			if (Base::FeatureHome()) { Serial.print(F("Home ")); }
			if (Base::FeatureShare()) { Serial.print(F("Share ")); }
			Serial.println();
		}
		else
		{
			Serial.println(F("\tNo Menu buttons"));
		}

		if (Base::FeatureJoy1() || Base::FeatureJoy2())
		{
			Serial.print(F("\tJoysticks: "));
			if (Base::FeatureJoy1()) { Serial.print(F("Joy1 ")); }
			if (Base::FeatureJoy2()) { Serial.print(F("Joy2 ")); }
			Serial.println();
		}
		else
		{
			Serial.println(F("\tNo Joysticks"));
		}

		if (Base::FeatureDPad())
		{
			Serial.print(F("\tDpad"));
		}
		else
		{
			Serial.print(F("\tNo DPad"));
		}
		Serial.println();

		if (Base::FeatureA() || Base::FeatureB() || Base::FeatureX() || Base::FeatureY())
		{
			Serial.print(F("\tFace Buttons: "));
			if (Base::FeatureA()) { Serial.print(F("A ")); }
			if (Base::FeatureB()) { Serial.print(F("B ")); }
			if (Base::FeatureX()) { Serial.print(F("X ")); }
			if (Base::FeatureY()) { Serial.print(F("Y ")); }
			Serial.println();
		}
		else
		{
			Serial.println(F("\tNo Face buttons"));
		}

		if (Base::FeatureL2() || Base::FeatureR2())
		{
			Serial.print(F("\tJoystick Buttons: "));
			if (Base::FeatureL3()) { Serial.print(F("L3 ")); }
			if (Base::FeatureR3()) { Serial.print(F("R3 ")); }
			Serial.println();
		}
		else
		{
			Serial.println(F("\tNo Joystick buttons"));
		}

		if (Base::FeatureL1() || Base::FeatureR1())
		{
			Serial.print(F("\tShoulder buttons: "));
			if (Base::FeatureL1()) { Serial.print(F("L1 ")); }
			if (Base::FeatureR1()) { Serial.print(F("R1 ")); }
			Serial.println();
		}
		else
		{
			Serial.println(F("\tNo Shoulder buttons"));
		}

		if (Base::FeatureL2() || Base::FeatureR2())
		{
			Serial.print(F("\tTriggers: "));
			if (Base::FeatureL2()) { Serial.print(F("L2 ")); }
			if (Base::FeatureR2()) { Serial.print(F("R2 ")); }
			Serial.println();
		}
		else
		{
			Serial.println(F("\tNo triggers"));
		}

		if (Base::FeatureMotion())
		{
			Serial.print(F("\tMotion: "));
			Serial.print(F("Accelerometer "));
			Serial.print(F("Gyro "));
			Serial.println();
		}
		else
		{
			Serial.println(F("\tNo motion"));
		}
		Serial.println();
	}

	void LogPropertiesNavigation()
	{
		Serial.println(F("Properties: "));
		if (Base::PropertyJoy1Digital())
		{
			Serial.println(F("\tJoy1 is Digital"));
		}

		if (Base::PropertyJoy2Digital())
		{
			Serial.println(F("\tJoy2 is Digital"));
		}

		if (Base::PropertyL2R2Digital())
		{
			Serial.println(F("\tL1 and R1 are Digital"));
		}

		Serial.println();

		Serial.println(F("Navigation: "));

		switch (Base::ConfigurationNavigation())
		{
		case VirtualPad::Configuration::NavigationEnum::BA:
			Serial.println(F("BA"));
			break;
		case VirtualPad::Configuration::NavigationEnum::XA:
			Serial.println(F("XA"));
			break;
		case VirtualPad::Configuration::NavigationEnum::BY:
			Serial.println(F("BY"));
			break;
		case VirtualPad::Configuration::NavigationEnum::AY:
			Serial.println(F("AY"));
			break;
		case VirtualPad::Configuration::NavigationEnum::BX:
			Serial.println(F("BX"));
			break;
		case VirtualPad::Configuration::NavigationEnum::AX:
			Serial.println(F("AX"));
			break;
		case VirtualPad::Configuration::NavigationEnum::XB:
			Serial.println(F("XB"));
			break;
		default:
		case VirtualPad::Configuration::NavigationEnum::AB:
			Serial.println(F("AB"));
			break;
		}
	}
#endif
};
#endif
