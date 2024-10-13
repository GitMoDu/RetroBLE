// UsbConfig.h

#ifndef _USB_CONFIG_h
#define _USB_CONFIG_h

#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <Adafruit_TinyUSB.h>

namespace RetroBle
{
	namespace UsbConfig
	{
		const uint8_t PollPeriod = 2;

		/// <summary>
		/// HID report descriptor using TinyUSB's template.
		/// Single Report (no ID) descriptor.
		/// </summary>
		namespace Descriptor
		{
			static constexpr uint8_t HidGamePad[] = { TUD_HID_REPORT_DESC_GAMEPAD() };
			static constexpr uint8_t HidKeyboard[] = { TUD_HID_REPORT_DESC_KEYBOARD() };  
			static constexpr uint8_t HidSystemControl[] = { TUD_HID_REPORT_DESC_SYSTEM_CONTROL() };
		}
	}
}
#endif