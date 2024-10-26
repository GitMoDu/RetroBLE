// HidToVirtualPad.h

#ifndef _HID_TO_VIRTUAL_PAD_h
#define _HID_TO_VIRTUAL_PAD_h

#include <VirtualPad.h>
#include <WriteVirtualPad.h>

#include "../Ble/BleCentral.h"
#include "XBoxControllerHid.h"


enum class HidMapTypeEnum : uint16_t
{
	XBox = 10829,
	// TODO:
	GenericGamepad = 0
};

class HidToVirtualPad : public virtual IHidListener, public WriteVirtualPad
{
private:
	static constexpr uint16_t Features = FeatureFlags::GetFlags<
		FeaturesEnum::DPad,
		FeaturesEnum::Joy1,
		FeaturesEnum::Joy2,
		FeaturesEnum::A, FeaturesEnum::B,
		FeaturesEnum::X, FeaturesEnum::Y,
		FeaturesEnum::L1, FeaturesEnum::R1,
		FeaturesEnum::L2, FeaturesEnum::R2,
		FeaturesEnum::L3, FeaturesEnum::R3>();

	static constexpr uint32_t NO_UPDATE_TIMEOUT = 3 * 1000;

public:
	HidToVirtualPad()
		: WriteVirtualPad(Features)
	{}

public:
	virtual void OnStateChange(const bool connected)
	{
		if (Connected() != connected)
		{
			Clear();
			SetConnected(connected);
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
	virtual void OnReportNotify(uint8_t* data, const uint16_t size, const uint16_t uuid) final
	{
		if (data != nullptr)
		{
#if defined(DEBUG)
			if (LastUuid != uuid)
			{
				LastUuid = uuid;
				Serial.print(F("\tConnected to \t"));
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
			SetConnected(false);
		}
	}

private:
	// TODO
	void HidMapGenericGamepad(uint8_t* data, const uint16_t len)
	{
		if (Connected())
		{
			SetConnected(false);
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
			SetJoy1(x1, y1);
			SetJoy2(x2, y2);

			// Scale to uint16_t full range.
			const uint16_t l2 = ((uint32_t)(data[8] | (uint16_t)data[9] << 8) * UINT16_MAX) / XBoxControllerHid::TriggerMax;
			const uint16_t r2 = ((uint32_t)(data[10] | (uint16_t)data[11] << 8) * UINT16_MAX) / XBoxControllerHid::TriggerMax;
			SetL2(l2);
			SetR2(r2);

			// Virtual Pad DPadEnum is derived from HID DPad and is compatible with XBox mapping.
			State.DPad = (DPadEnum)data[12];

			// First byte of buttons.
			const uint8_t buttons1 = data[13];
			SetA(GetButton<(uint8_t)XBoxControllerHid::Buttons1::A>(buttons1));
			SetB(GetButton<(uint8_t)XBoxControllerHid::Buttons1::B>(buttons1));
			SetX(GetButton<(uint8_t)XBoxControllerHid::Buttons1::X>(buttons1));
			SetY(GetButton<(uint8_t)XBoxControllerHid::Buttons1::Y>(buttons1));
			SetL1(GetButton<(uint8_t)XBoxControllerHid::Buttons1::L1>(buttons1));
			SetR1(GetButton<(uint8_t)XBoxControllerHid::Buttons1::R1>(buttons1));

			// Second byte of buttons.
			const uint8_t buttons2 = data[14];
			SetSelect(GetButton<(uint8_t)XBoxControllerHid::Buttons2::Select>(buttons2));
			SetStart(GetButton<(uint8_t)XBoxControllerHid::Buttons2::Start>(buttons2));
			SetL3(GetButton<(uint8_t)XBoxControllerHid::Buttons2::L3>(buttons2));
			SetR3(GetButton<(uint8_t)XBoxControllerHid::Buttons2::R3>(buttons2));
			SetHome(GetButton<(uint8_t)XBoxControllerHid::Buttons2::Home>(buttons2));

			// Third byte of buttons.
			const uint8_t buttons3 = data[15];
			SetShare(GetButton<(uint8_t)XBoxControllerHid::Buttons3::Share>(buttons3));

			if (!Connected())
			{
				SetConnected(true);
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
	void LogControllerState()
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
#endif
};
#endif
