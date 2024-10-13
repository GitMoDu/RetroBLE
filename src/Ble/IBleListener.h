// IBleListener.h

#ifndef _I_BLE_LISTENER_h
#define _I_BLE_LISTENER_h

#include <stdint.h>

class IBleListener
{
public:
	virtual void OnBleStateChange() {}
};
#endif

