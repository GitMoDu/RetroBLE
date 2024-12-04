/*
* Retro BLE Pad implementation for Atari 2600 controller.
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
*	- Task Scheduler https://github.com/arkhipenko/TaskScheduler
*/

//#define DEBUG

#define _TASK_OO_CALLBACKS

#include <TScheduler.hpp>


#include <Arduino.h>
#include <RetroBle.h>

#include "Device.h"

#include "AtariSleepyPad.h"
#include "AtariMapperTask.h"

// Process scheduler.
TS::Scheduler SchedulerBase;
// 

//// HARDWARE DRIVERS
// Driver for onboard RGB LED.
LedDriver<Device::Led::Pin, Device::Led::OnState> Led{};

// Driver for onboard battery manager.
Bq25100Driver <Device::BMS::UpdatePeriodMillis,
	Device::BMS::HistorySize,
	Device::BMS::Pin,
	Device::BMS::Calibration> BMS(SchedulerBase);

// USB driver.
UsbHidGamepad UsbGamepad{};
UsbPeripheral UsbDev{};

// Bluetooth driver.
BLEHidGamepad BleGamepad{};
BlePeripheral BleDev{};

// Mega Driver Controller driver.
using AtariVirtualPadType = AtariSleepyPad<Device::AtariController::Pin, Device::AtariController::WakePin>;
AtariVirtualPadType AtariVirtualPadWrite{};
//

//// SOFTWARE TASKS
// Pad read and notify task.
AtariMapperTask<AtariVirtualPadType>GamepadMapper(
	SchedulerBase,
	AtariVirtualPadWrite,
	UsbGamepad, BleGamepad,
	Device::BLE::UpdatePeriodMillis);

// LED animation task.
LedAnimatorTask PadLights(SchedulerBase, &Led);

// Coordinator task.
UsbBleCoordinator Coordinator(SchedulerBase, &BMS, &PadLights, &GamepadMapper, UsbDev, BleDev);
//

void setup()
{
#if defined(DEBUG)
	Serial.begin(Device::Debug::SERIAL_BAUD_RATE);

	// Blocking wait for connection when debug mode is enabled via IDE
	while (!Serial) delay(10);

	Serial.println(F("MegaDrive start"));
#endif

	// Disable unused pins.
	PinSetup();

	// Battery management setup.
	BMS.Setup();
	BMS.Start();

	// Onboard LED setup.
	Led.Setup();
	PadLights.Start();

	// Setup controller driver.
	AtariVirtualPadWrite.Setup(OnButtonInterrupt);

	// BLE setup.
	BleDev.Setup(BleGamepad,
		connect_callback, disconnect_callback,
		advertise_stop_callback, ble_event_callback,
		Device::BLE::Appearance,
		Device::BLE::ConnectionIntervalMin,
		Device::BLE::ConnectionIntervalMax,
		Device::Name,
		Device::Version::Name);

	// USB setup.
	UsbDev.Setup(Device::Name, Device::Version::Code,
		Device::USB::UpdatePeriodMillis,
		Device::USB::ProductId);
	UsbGamepad.Setup(Device::Name, get_report_callback, set_report_callback);

	// Start the device coordinator.
	if (!Coordinator.Start())
	{
#if defined(DEBUG)
		Serial.println(F("Error starting coordinator."));
#endif
	}
}

void loop()
{
	SchedulerBase.execute();
}

void PinSetup()
{
	for (uint8_t i = 0; i < sizeof(Device::Unused::Pins); i++)
	{
		const uint8_t pin = Device::Unused::Pins[i];
		pinMode(pin, INPUT);
		digitalWrite(pin, LOW);
	}
}

// Event callbacks.
void ble_event_callback(ble_evt_t* bleEvent)
{
	BleDev.OnBleEventInterrupt(bleEvent);
}

void advertise_stop_callback()
{}

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

void OnButtonInterrupt()
{
	GamepadMapper.OnWakeInterrupt();
}
