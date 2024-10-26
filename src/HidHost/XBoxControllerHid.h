// XBoxControllerHid.h

#ifndef _XBOX_CONTROLLER_HID_h
#define _XBOX_CONTROLLER_HID_h

#include <stdint.h>

namespace XBoxControllerHid
{
	static constexpr uint8_t DataSize = 16;

	static constexpr uint16_t TriggerMax = 1023;

	static constexpr uint16_t JoyStickDeadZone = 1500;

	enum class DPad : uint8_t
	{
		None = 0,
		Up = 1,
		UpRight = 2,
		Right = 3,
		DownRight = 4,
		Down = 5,
		DownLeft = 6,
		Left = 7,
		UpLeft = 8,
		DPadEnumCount
	};

	enum class Buttons1 : uint8_t
	{
		A = 0,
		B = 1,
		X = 3,
		Y = 4,
		L1 = 6,
		R1 = 7
	};

	enum class Buttons2 : uint8_t
	{
		Select = 2,
		Start = 3,
		Home = 4,
		L3 = 5,
		R3 = 6,
		Unknown1,
		Unknown2
	};

	enum class Buttons3 : uint8_t
	{
		Share = 0,
		Unknown1,
		Unknown2,
		Unknown3,
		Unknown4,
		Unknown5,
		Unknown6,
		Unknown7
	};
}
#endif
