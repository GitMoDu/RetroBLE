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
class HidToVirtualPad : public virtual IHidListener, public VirtualPad::MotionVirtualPad<hidConfigurationCode>
{
private:
	using Base = VirtualPad::MotionVirtualPad<hidConfigurationCode>;

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
			SetConnected(*this, connected);
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
			SetConnected(*this, false);
		}
	}

private:
	// TODO
	void HidMapGenericGamepad(uint8_t* data, const uint16_t len)
	{
		if (Base::Connected())
		{
			SetConnected(*this, false);
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
			SetJoy1(*this, x1, y1);
			SetJoy2(*this, x2, y2);

			// Scale to uint16_t full range.
			const uint16_t l2 = ((uint32_t)(data[8] | (uint16_t)data[9] << 8) * UINT16_MAX) / XBoxControllerHid::TriggerMax;
			const uint16_t r2 = ((uint32_t)(data[10] | (uint16_t)data[11] << 8) * UINT16_MAX) / XBoxControllerHid::TriggerMax;
			SetL2(*this, l2);
			SetR2(*this, r2);

			// Virtual Pad DPadEnum is derived from HID DPad and is compatible with XBox mapping.
			Base::dPad = VirtualPad::DPadEnum(data[12]);

			// First byte of buttons.
			const uint8_t buttons1 = data[13];
			SetA(*this, GetButton<(uint8_t)XBoxControllerHid::Buttons1::A>(buttons1));
			SetB(*this, GetButton<(uint8_t)XBoxControllerHid::Buttons1::B>(buttons1));
			SetX(*this, GetButton<(uint8_t)XBoxControllerHid::Buttons1::X>(buttons1));
			SetY(*this, GetButton<(uint8_t)XBoxControllerHid::Buttons1::Y>(buttons1));
			SetL1(*this, GetButton<(uint8_t)XBoxControllerHid::Buttons1::L1>(buttons1));
			SetR1(*this, GetButton<(uint8_t)XBoxControllerHid::Buttons1::R1>(buttons1));

			// Second byte of buttons.
			const uint8_t buttons2 = data[14];
			SetSelect(*this, GetButton<(uint8_t)XBoxControllerHid::Buttons2::Select>(buttons2));
			SetStart(*this, GetButton<(uint8_t)XBoxControllerHid::Buttons2::Start>(buttons2));
			SetL3(*this, GetButton<(uint8_t)XBoxControllerHid::Buttons2::L3>(buttons2));
			SetR3(*this, GetButton<(uint8_t)XBoxControllerHid::Buttons2::R3>(buttons2));
			SetHome(*this, GetButton<(uint8_t)XBoxControllerHid::Buttons2::Home>(buttons2));

			// Third byte of buttons.
			const uint8_t buttons3 = data[15];
			SetShare(*this, GetButton<(uint8_t)XBoxControllerHid::Buttons3::Share>(buttons3));

			if (!Base::Connected())
			{
				SetConnected(*this, true);
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
		if (Connected())
		{
			Serial.println();
			Serial.print(F("Navigation\t"));
			if (Home())
			{
				Serial.print(F("Home "));
			}
			if (GetAccept())
			{
				Serial.print(F("Accept "));
			}
			if (GetReject())
			{
				Serial.print(F("Reject "));
			}
			Serial.println();

			Serial.print(F("DPad\t"));
			switch (DPad())
			{
			case DPadEnum::Up:
				Serial.println(F("Up"));
				break;
			case DPadEnum::UpRight:
				Serial.println(F("UpRight"));
				break;
			case DPadEnum::Right:
				Serial.println(F("Right"));
				break;
			case DPadEnum::DownRight:
				Serial.println(F("DownRight"));
				break;
			case DPadEnum::Down:
				Serial.println(F("Down"));
				break;
			case DPadEnum::DownLeft:
				Serial.println(F("DownLeft"));
				break;
			case DPadEnum::Left:
				Serial.println(F("Left"));
				break;
			case DPadEnum::UpLeft:
				Serial.println(F("UpLeft"));
				break;
			case DPadEnum::None:
			default:
				Serial.println(F("Center"));
				break;
			}

			Serial.print(F("Main Buttons\t"));

			if (FeatureA())
			{
				if (A())
				{
					Serial.print(F("A  "));
				}
				else
				{
					Serial.print(F("__ "));
				}
			}

			if (FeatureB())
			{
				if (B())
				{
					Serial.print(F("B  "));
				}
				else
				{
					Serial.print(F("__ "));
				}
			}

			if (FeatureX())
			{
				if (X())
				{
					Serial.print(F("X  "));
				}
				else
				{
					Serial.print(F("__ "));
				}
			}

			if (FeatureY())
			{
				if (Y())
				{
					Serial.print(F("Y  "));
				}
				else
				{
					Serial.print(F("__ "));
				}
			}

			if (FeatureL1())
			{
				if (L1())
				{
					Serial.print(F("L1 "));
				}
				else
				{
					Serial.print(F("__ "));
				}
			}

			if (FeatureR1())
			{
				if (R1())
				{
					Serial.print(F("R1 "));
				}
				else
				{
					Serial.print(F("__ "));
				}
			}

			if (FeatureL3())
			{
				if (L3())
				{
					Serial.print(F("L3 "));
				}
				else
				{
					Serial.print(F("__ "));
				}
			}

			if (FeatureR3())
			{
				if (R3())
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
			if (Start())
			{
				Serial.print(F("Start "));
			}
			if (Select())
			{
				Serial.print(F("Select "));
			}
			if (Home())
			{
				Serial.print(F("Home "));
			}
			if (Share())
			{
				Serial.print(F("Share "));
			}
			Serial.println();

			Serial.print(F("Joy1(x,y)\t("));
			Serial.print(Joy1X());
			Serial.print(',');
			Serial.print(Joy1Y());
			Serial.println(')');

			if (FeatureJoy2())
			{
				Serial.print(F("Joy2(x,y)\t("));
				Serial.print(Joy2X());
				Serial.print(',');
				Serial.print(Joy2Y());
				Serial.println(')');
			}

			if (FeatureL2())
			{
				Serial.print(F("L2\t"));
				Serial.println(L2());
			}

			if (FeatureR2())
			{
				Serial.print(F("R2\t"));
				Serial.println(R2());
			}
		}
	}

	void LogFeatures()
	{
		Serial.println(F("Features: "));

		if (FeatureStart() || FeatureSelect() || FeatureHome() || FeatureShare())
		{
			Serial.print(F("\tMenu Buttons: "));
			if (FeatureStart()) { Serial.print(F("Start ")); }
			if (FeatureSelect()) { Serial.print(F("Select ")); }
			if (FeatureHome()) { Serial.print(F("Home ")); }
			if (FeatureShare()) { Serial.print(F("Share ")); }
			Serial.println();
		}
		else
		{
			Serial.println(F("\tNo Menu buttons"));
		}

		if (FeatureJoy1() || FeatureJoy2())
		{
			Serial.print(F("\tJoysticks: "));
			if (FeatureJoy1()) { Serial.print(F("Joy1 ")); }
			if (FeatureJoy2()) { Serial.print(F("Joy2 ")); }
			Serial.println();
		}
		else
		{
			Serial.println(F("\tNo Joysticks"));
		}

		if (FeatureDPad())
		{
			Serial.print(F("\tDpad"));
		}
		else
		{
			Serial.print(F("\tNo DPad"));
		}
		Serial.println();

		if (FeatureA() || FeatureB() || FeatureX() || FeatureY())
		{
			Serial.print(F("\tFace Buttons: "));
			if (FeatureA()) { Serial.print(F("A ")); }
			if (FeatureB()) { Serial.print(F("B ")); }
			if (FeatureX()) { Serial.print(F("X ")); }
			if (FeatureY()) { Serial.print(F("Y ")); }
			Serial.println();
		}
		else
		{
			Serial.println(F("\tNo Face buttons"));
		}

		if (FeatureL2() || FeatureR2())
		{
			Serial.print(F("\tJoystick Buttons: "));
			if (FeatureL3()) { Serial.print(F("L3 ")); }
			if (FeatureR3()) { Serial.print(F("R3 ")); }
			Serial.println();
		}
		else
		{
			Serial.println(F("\tNo Joystick buttons"));
		}

		if (FeatureL1() || FeatureR1())
		{
			Serial.print(F("\tShoulder buttons: "));
			if (FeatureL1()) { Serial.print(F("L1 ")); }
			if (FeatureR1()) { Serial.print(F("R1 ")); }
			Serial.println();
		}
		else
		{
			Serial.println(F("\tNo Shoulder buttons"));
		}

		if (FeatureL2() || FeatureR2())
		{
			Serial.print(F("\tTriggers: "));
			if (FeatureL2()) { Serial.print(F("L2 ")); }
			if (FeatureR2()) { Serial.print(F("R2 ")); }
			Serial.println();
		}
		else
		{
			Serial.println(F("\tNo triggers"));
		}
		Serial.println();
	}

	void LogPropertiesNavigation()
	{
		Serial.println(F("Properties: "));
		if (PropertyJoy1Digital())
		{
			Serial.println(F("\tJoy1 is Digital"));
		}

		if (PropertyJoy2Digital())
		{
			Serial.println(F("\tJoy2 is Digital"));
		}

		if (PropertyL2R2Digital())
		{
			Serial.println(F("\tL1 and R1 are Digital"));
		}

		Serial.println();

		Serial.println(F("Navigation: "));
		switch (VirtualPadConfiguration::GetNavigation(ConfigurationCode))
		{
		case NavigationEnum::BA:
			Serial.println(F("BA"));
			break;
		case NavigationEnum::XA:
			Serial.println(F("XA"));
			break;
		case NavigationEnum::BY:
			Serial.println(F("BY"));
			break;
		case NavigationEnum::AY:
			Serial.println(F("AY"));
			break;
		case NavigationEnum::BX:
			Serial.println(F("BX"));
			break;
		case NavigationEnum::AX:
			Serial.println(F("AX"));
			break;
		case NavigationEnum::XB:
			Serial.println(F("XB"));
			break;
		default:
		case NavigationEnum::AB:
			Serial.println(F("AB"));
			break;
		}
	}
#endif
};
#endif
