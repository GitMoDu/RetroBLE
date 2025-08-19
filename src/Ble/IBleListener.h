#ifndef _I_BLE_LISTENER_h
#define _I_BLE_LISTENER_h

#include <stdint.h>

class IBleListener
{
public:
	virtual void OnBleStateChange() = 0;

	virtual void OnBleBackReport(uint8_t report_id, uint8_t report_type, uint8_t const* buffer, uint16_t bufsize) = 0;
};
#endif

