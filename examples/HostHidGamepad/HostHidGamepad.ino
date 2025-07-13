/*
* Retro BLE Pad host (BLE Central).
*
*
* Dependencies:
*   - Core:
*   - Reference Central: https://github.com/asukiaaa/nrf52-bluefluit-xbox-controller-practice/tree/master
*	- Uart interface:  https://github.com/GitMoDu/UartInterface
*	- Pad abstraction:  https://github.com/GitMoDu/VirtualPad
*/

//#define DEBUG

#define _TASK_OO_CALLBACKS
#include <TScheduler.hpp>

#include <RetroBle.h>
#include "Device.h"

#include <VirtualPadUartInterface.h>

// Process scheduler.
TS::Scheduler SchedulerBase{};
// 

// Pad state source.
HidToVirtualPad<Device::VirtualPadUartInterface::ConfigurationCode> Pad{};

// Host (central).
BleCentral Central(&Pad);

// Uart server.
#if defined(DEBUG1)
VirtualPadUartInterface::ServerTask<Adafruit_USBD_CDC> UartServer(SchedulerBase, Serial, Pad);
#else
VirtualPadUartInterface::ServerTask<Uart> UartServer(SchedulerBase, Serial1, Pad);
#endif


void setup()
{
#if defined(DEBUG)
	Serial.begin(Device::Debug::SERIAL_BAUD_RATE);

	// Blocking wait for connection when debug mode is enabled via IDE
	while (!Serial)
		delay(10);

	Pad.LogFeatures();
	Pad.LogPropertiesNavigation();
#endif

	// Disable unused pins.
	PinSetup();

	// Setup BLE Central.
	Central.Setup(connect_callback, disconnect_callback,
		scan_callback,
		connection_secured_callback,
		report_notification_callback);


	if (!UartServer.Setup())
	{
		while (true)
			;;
	}

	// Start pushing out state updates on UART.
	UartServer.Start();

#if defined(DEBUG)
	Serial.println(F("Host Hid Gamepad start"));
#endif
}

void loop()
{
	if (Serial1.available())
	{
		UartServer.OnSerialEvent();
	}

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

void connect_callback(uint16_t conn_handle)
{
	Central.OnConnect(conn_handle);
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
	Central.OnDisconnect(conn_handle, reason);
}

void scan_callback(ble_gap_evt_adv_report_t* report)
{
	Central.OnScanCallback(report);
}

void connection_secured_callback(uint16_t conn_handle)
{
	Central.OnConnectionSecured(conn_handle);
}

void report_notification_callback(BLEClientCharacteristic* chr, uint8_t* data, uint16_t len)
{
	Central.OnReportNotify(chr, data, len);
}
