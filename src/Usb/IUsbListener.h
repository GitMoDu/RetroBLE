#ifndef _I_USB_LISTENER_h
#define _I_USB_LISTENER_h

#include <stdint.h>
#include "UsbConfig.h"

struct IUsbListener
{
	virtual void OnUsbStateChange() = 0;

	virtual void OnUsbBackReport(uint8_t report_id, uint8_t report_type, uint8_t const* buffer, uint16_t bufsize) = 0;
};
#endif