// UsbHidGamepad.h

#ifndef _USB_HID_GAMEPAD_h
#define _USB_HID_GAMEPAD_h

#include "IUsbListener.h"
#include "UsbConfig.h"

/// <summary>
/// TODO: Callbacks for connection events.
/// TODO: Callbacks for events dispatched by host.
/// </summary>
class UsbHidGamepad
{
private:
	// USB HID object.
	Adafruit_USBD_HID HidInstance;

private:
	IUsbListener* Listener = nullptr;

public:
	UsbHidGamepad()
		: HidInstance(RetroBle::UsbConfig::Descriptor::HidGamePad, sizeof(RetroBle::UsbConfig::Descriptor::HidGamePad), HID_ITF_PROTOCOL_NONE, 2, false)
	{}

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

	const bool NotifyGamepad(hid_gamepad_report_t& report)
	{
		return HidInstance.sendReport(0, &report, sizeof(hid_gamepad_report_t));
	}

	void SetUsbListener(IUsbListener* listener)
	{
		Listener = listener;
	}

	const bool IsReady()
	{
		return HidInstance.ready();
	}

public:
	const uint16_t OnGetReportInterrupt(uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
	{
		if (Listener != nullptr)
		{
			Listener->OnUsbBackReport(report_id, report_type, buffer, reqlen);
		}

		return 0;
	}

	void OnSetReportInterrupt(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
	{
	}
};

#endif