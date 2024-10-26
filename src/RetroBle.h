// RetroBle.h

#ifndef _RETRO_BLE_INCLUDE_h
#define _RETRO_BLE_INCLUDE_h


#if defined(ARDUINO_Seeed_XIAO_nRF52840_Sense) || defined(ARDUINO_Seeed_XIAO_nRF52840)
#include "Platform/SeeedXIAOnRF52840.h"
#elif defined(ARDUINO_ARCH_NRF52)
#include "Platform/NologoProMicronRF52840.h"
#else
#pragma "Device not supported."
#endif


#include "Framework/RetroBleDevice.h"

#include "BatteryManager/IBatteryManager.h"
#include "BatteryManager/BatteryState.h"
#include "BatteryManager/Drivers/Bq25100Driver.h"

#include "Led/ILedDriver.h"
#include "Led/LedAnimator.h"
#include "Led/Drivers/LedDriver.h"

#include "Lights/ILightsDriver.h"
#include "Lights/Drivers/Ws2812Driver.h"

#include "Ble/IBleListener.h"
#include "Ble/BleConfig.h"
#include "Ble/BlePeripheral.h"
#include "Ble/BleCentral.h"

#include "Usb/IUsbListener.h"
#include "Usb/UsbConfig.h"
#include "Usb/UsbPeripheral.h"

#include "Usb/UsbHidGamepad.h"
#include "Usb/UsbHidKeyboard.h"

#include "HidDevice/HidGamepadTask.h"
#include "HidDevice/HidKeyboardTask.h"

#include "HidHost/HidToVirtualPad.h"

#include "Framework/UsbBleCoordinator.h"


#endif