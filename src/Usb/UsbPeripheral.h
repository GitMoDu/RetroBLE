// UsbPeripheral.h

#ifndef _USB_PERIPHERAL_h
#define _USB_PERIPHERAL_h

#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <Adafruit_TinyUSB.h>

#include "UsbConfig.h"
#include "IUsbListener.h"

/// <summary>
/// Retro BLE device USB HID manager.
/// Exposes the current connection state.
/// Provides callbacks for connection events.
/// 
/// TODO: Callbacks for events dispatched by host.
/// </summary>
class UsbPeripheral
{
private:
	static constexpr uint32_t ADVERTISE_FAST_TIMEOUT_MILLIS = 30000;

public:
	UsbPeripheral()
	{}

	void Setup(const char* name = "Experimental",
		uint8_t version = 0,
		const uint8_t pollPeriod = 5,
		const uint16_t pid = 0)
	{
		TinyUSBDevice.setManufacturerDescriptor(RetroBle::Device::Manufacturer);
		TinyUSBDevice.setDeviceVersion(version);
		TinyUSBDevice.setProductDescriptor(name);
		TinyUSBDevice.setVersion(version);

		if (pid != 0)
		{
			TinyUSBDevice.setID(RetroBle::Device::VendorId, pid);
		}
	}

	const bool IsConnected()
	{
		return TinyUSBDevice.mounted();
	}
};

#endif