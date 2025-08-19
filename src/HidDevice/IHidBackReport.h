#ifndef _I_HID_BACK_REPORT_h
#define _I_HID_BACK_REPORT_h

#include <stdint.h>

namespace RetroBle
{
	namespace HidBackReport
	{
		enum class IdEnum : uint8_t
		{
			KeyboardLights = 0,
			Rumble = 0xFF - 1
		};

		struct IListener
		{
			virtual void OnBackReport(uint8_t report_id, uint8_t report_type, uint8_t const* buffer, uint16_t bufsize) = 0;
		};
	}
}
#endif