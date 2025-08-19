#ifndef _I_HID_BACK_REPORT_h
#define _I_HID_BACK_REPORT_h

#include <stdint.h>

namespace RetroBle
{
	namespace HidBackReport
	{
		enum class SourceEnum
		{
			Usb,
			Ble
		};
		enum class IdEnum : uint16_t
		{
			KeyboardLights = 0xFF - 1,
			Rumble = 0xFF - 2
		};

		struct IListener
		{
			virtual void OnBackReport(SourceEnum backReportSource, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) = 0;
		};
	}	
}
#endif