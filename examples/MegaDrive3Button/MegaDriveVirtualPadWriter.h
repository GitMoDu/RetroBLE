// MegaDriveVirtualPadWriter.h

#ifndef _MEGA_DRIVE_VIRTUAL_WRITER_PAD_h
#define _MEGA_DRIVE_VIRTUAL_WRITER_PAD_h

#include <stdint.h>

#include <WriteVirtualPad.h>

template<typename ControllerPin>
class MegaDriveVirtualPadWriter : public WriteVirtualPad
{
private:
	static constexpr uint32_t Features = VirtualPadConfiguration::GetFeatureFlags<
		FeaturesEnum::DPad,
		FeaturesEnum::Start,
		FeaturesEnum::A, FeaturesEnum::B,
		FeaturesEnum::X>();

	static constexpr uint8_t PIN_INPUT_MODE = INPUT_PULLDOWN;

public:
	MegaDriveVirtualPadWriter()
		: WriteVirtualPad(Features)
	{}

	void Setup()
	{
		pinMode((uint8_t)ControllerPin::Up, PIN_INPUT_MODE);
		pinMode((uint8_t)ControllerPin::Down, PIN_INPUT_MODE);
		pinMode((uint8_t)ControllerPin::InfoLeft, PIN_INPUT_MODE);
		pinMode((uint8_t)ControllerPin::InfoRight, PIN_INPUT_MODE);
		pinMode((uint8_t)ControllerPin::AB, PIN_INPUT_MODE);
		pinMode((uint8_t)ControllerPin::StartC, PIN_INPUT_MODE);

		pinMode((uint8_t)ControllerPin::Select, OUTPUT);
		digitalWrite((uint8_t)ControllerPin::Select, LOW);
	}

	void Step()
	{
		digitalWrite((uint8_t)ControllerPin::Select, LOW);
		if (digitalRead((uint8_t)ControllerPin::InfoLeft) || digitalRead((uint8_t)ControllerPin::InfoRight))
		{
			if (Connected())
			{
				SetConnected(false);
			}
		}
		else if (!Connected())
		{
			SetConnected(true);
		}

		WriteVirtualPad::SetA(digitalRead((uint8_t)ControllerPin::AB));
		WriteVirtualPad::SetStart(digitalRead((uint8_t)ControllerPin::StartC));

		digitalWrite((uint8_t)ControllerPin::Select, HIGH);
		WriteVirtualPad::SetB(digitalRead((uint8_t)ControllerPin::AB));
		WriteVirtualPad::SetX(digitalRead((uint8_t)ControllerPin::StartC));

		WriteVirtualPad::SetDPad(
			digitalRead((uint8_t)ControllerPin::Up),
			digitalRead((uint8_t)ControllerPin::Down),
			digitalRead((uint8_t)ControllerPin::InfoLeft),
			digitalRead((uint8_t)ControllerPin::InfoRight)
		);

		digitalWrite((uint8_t)ControllerPin::Select, LOW);
	}
};
#endif
