/*
* Retro BLE Pad implementation for MegaDrive/Genesis 3-Button controller.
*
* Retro BLE features
*	- USB HID interface, takes precedence over any BLE connection.
*	- BLE interface with ~12ms update period.
*	- Battery level state.
*	- State LEDs.
*
* Dependencies:
*	- Core https://github.com/Seeed-Studio/OSHW-XIAO-Series
*	- BLE https://github.com/adafruit/Adafruit_nRF52_Arduino/blob/master/libraries/Bluefruit52Lib/src/bluefruit.h
*	- USB https://github.com/adafruit/Adafruit_TinyUSB_Arduino
*/

//#define DEBUG

#define _TASK_OO_CALLBACKS
#define _TASK_SLEEP_ON_IDLE_RUN
#include <TScheduler.hpp>

#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <bluefruit.h>


#include <Arduino.h>
#include <RetroBle.h>
#include "Device.h"

#include "MegaDriveVirtualPadWriter.h"
#include "BlePadMapper.h"
#include "UsbPadMapper.h"

#include "Maestro.h"


// Process scheduler.
TS::Scheduler SchedulerBase;
// 

//// HARDWARE DRIVERS
// Driver for onboard RGB LED.
LedDriver<Device::Led::Pin, Device::Led::OnState> Led{};

// Driver for onboard battery manager.
Bq25100Driver < Device::BMS::UpdatePeriodMillis,
	Device::BMS::HistorySize,
	Device::BMS::Pin,
	Device::BMS::Calibration> BMS(SchedulerBase);

// Mega Driver Controller reader.
using MegaDriveVirtualPadType = MegaDriveVirtualPadWriter<Device::MegaDriveController::Pin>;
MegaDriveVirtualPadType MegaDriveVirtualPadWrite{};
//

// Bluetooth driver.
BLEHidGamepad BleGamepad{};
BlePeripheral BleDev{};
//

//
UsbHidGamepad UsbGamepad{};
UsbPeripheral UsbDev{};
//

//// SOFTWARE TASKS
// Pad read and notify task.
using UsbMapperType = UsbPadMapperTask<MegaDriveVirtualPadType, Device::USB::UpdatePeriodMillis>;
UsbMapperType UsbPadMapper(SchedulerBase, MegaDriveVirtualPadWrite, UsbDev, UsbGamepad);

using BlePadMapperType = BlePadMapperTask<MegaDriveVirtualPadType, Device::BLE::UpdatePeriodMillis>;
BlePadMapperType BlePadMapper(SchedulerBase, MegaDriveVirtualPadWrite, BleGamepad);

// LED animation task.
LedAnimatorTask PadLights(SchedulerBase, &Led);

// Coordinator task.
MaestroTask<UsbMapperType, BlePadMapperType> Maestro(SchedulerBase, &BMS, PadLights, UsbDev, UsbPadMapper, BleDev, BlePadMapper);
//

void setup()
{
#if defined(DEBUG)
	Serial.begin(Device::Debug::SERIAL_BAUD_RATE);

	// Blocking wait for connection when debug mode is enabled via IDE
	while (!Serial) delay(10);
#endif

	// Disable unused pins.
	PinSetup();

	// Battery management pin setup.
	BMS.Setup();

	// Onboard LED pin setup.
	Led.Setup();

	// Setup controller pins.
	MegaDriveVirtualPadWrite.Setup();

	// BLE setup.
	BleDev.Setup(BleGamepad,
		connect_callback, disconnect_callback,
		advertise_stop_callback, ble_event_callback,
		Device::BLE::Appearance,
		Device::Name, Device::Version::Name);

	// USB setup.
	UsbDev.Setup(Device::Name, Device::Version::Code,
		Device::USB::UpdatePeriodMillis,
		Device::USB::ProductId);
	UsbGamepad.Setup(Device::Name, get_report_callback, set_report_callback);

	//TODO:
	//UsbGamepad.SetUsbListener();

	// Start Maestro, the device coordinator.
	Maestro.Start();
}

void loop()
{
	SchedulerBase.execute();
}

void PinSetup()
{
	for (uint8_t i = 0; i < sizeof(Device::Unused::Pins); i++)
	{
		pinMode(Device::Unused::Pins[i], INPUT);
	}
}

// Event callbacks.
void ble_event_callback(ble_evt_t* bleEvent)
{
	BleDev.OnBleEventInterrupt(bleEvent);
}

void advertise_stop_callback()
{
	BleDev.OnAdvertiseStopInterrupt();
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
	BleDev.OnDisconnectInterrupt(conn_handle, reason);
}

void connect_callback(uint16_t conn_handle)
{
	BleDev.OnConnectInterrupt(conn_handle);
}

uint16_t get_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
	return UsbGamepad.OnGetReportInterrupt(report_id, report_type, buffer, reqlen);
}

void set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
	UsbGamepad.OnSetReportInterrupt(report_id, report_type, buffer, bufsize);
}