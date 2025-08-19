// UsbHidKeyboard.h

#ifndef _USB_HID_KEYBOARD_h
#define _USB_HID_KEYBOARD_h

#include "IUsbListener.h"
#include "UsbConfig.h"

/// <summary>
/// Retro BLE device USB HID manager.
/// Exposes the current connection state.
/// Provides callbacks for connection events.
/// 
/// TODO: Callbacks for events dispatched by host.
/// </summary>
class UsbHidKeyboard
{
private:
	// USB HID object.
	Adafruit_USBD_HID HidInstance;

private:
	IUsbListener* Listener = nullptr;

public:
	UsbHidKeyboard()
		: HidInstance(RetroBle::UsbConfig::Descriptor::HidKeyboard, sizeof(RetroBle::UsbConfig::Descriptor::HidKeyboard),
			HID_ITF_PROTOCOL_KEYBOARD, 2, false)
	{
	}

	void Setup(const char* description,
		uint16_t(*onGetReportInterrupt)(uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) = nullptr,
		void (*onSetReportInterrupt)(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) = nullptr)
	{
		HidInstance.setStringDescriptor(description);

		// TODO: find string size from null terminator.
		HidInstance.getInterfaceDescriptor(0, (uint8_t*)description, 16);

		if (onGetReportInterrupt != nullptr
			&& onSetReportInterrupt != nullptr)
		{
			HidInstance.enableOutEndpoint(true);
			HidInstance.setReportCallback(onGetReportInterrupt, onSetReportInterrupt);
		}
		else
		{
			HidInstance.enableOutEndpoint(false);
		}

		HidInstance.setPollInterval(RetroBle::UsbConfig::PollPeriod);

		HidInstance.begin();
	}

	const bool NotifyKeyboard(hid_keyboard_report_t& report)
	{
		//return false;
		return HidInstance.sendReport(0, &report, sizeof(hid_keyboard_report_t));
	}

	void SetUsbListener(IUsbListener* listener)
	{
		Listener = listener;
	}

	bool IsReady()
	{
		return HidInstance.ready();
	}

public:
	const uint16_t OnGetReportInterrupt(uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
	{
		return 0;
	}

	void OnSetReportInterrupt(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
	{
		if (Listener != nullptr)
		{
			Listener->OnUsbBackReport(report_id, report_type, buffer, bufsize);
		}
	}
};

#endif