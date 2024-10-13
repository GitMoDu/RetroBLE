// IUsbListener.h

#ifndef _I_USB_LISTENER_h
#define _I_USB_LISTENER_h

#include <stdint.h>
#include "UsbConfig.h"

class IUsbListener
{
public:
	virtual void OnUsbStateChange() {}

	virtual void OnUsbBackReport(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {}
};
#endif