/*
* Retro BLE BMS driver tester.
*
* Dependencies:
*	- Core https://github.com/Seeed-Studio/OSHW-XIAO-Series
*	- Task Scheduler https://github.com/arkhipenko/TaskScheduler
*/

#define _TASK_OO_CALLBACKS
#include <TScheduler.hpp>


#include <Arduino.h>
#include <RetroBle.h>

#include "Device.h"
#include "BmsPoller.h"

// Process scheduler.
TS::Scheduler SchedulerBase;
// 

//// HARDWARE DRIVERS
Bq25100Driver<Device::BMS::UpdatePeriodMillis, Device::BMS::HistorySize, Device::BMS::Pin> BMS(SchedulerBase);
//CustomBms<Device::BMS::UpdatePeriodMillis, Device::BMS::HistorySize> BMS(SchedulerBase);
//

// Polls BMS for the battery state and logs it to serial.
BmsPoller Poller(SchedulerBase, &BMS);

void setup()
{
	Serial.begin(Device::Debug::SERIAL_BAUD_RATE);

	// Blocking wait for connection when debug mode is enabled via IDE
	while (!Serial) delay(10);

	Serial.println(F("BMS Tester start"));
	Serial.flush();

	// Battery management setup.
	BMS.Setup();

	// Start BMS.
	BMS.Start();
}

void loop()
{
	SchedulerBase.execute();
}

