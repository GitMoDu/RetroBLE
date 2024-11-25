// HidBatteryTask.h

#ifndef _HID_BATTERY_TASK_h
#define _HID_BATTERY_TASK_h

#if defined(_TASK_OO_CALLBACKS)
#include <TSchedulerDeclarations.hpp>

#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <bluefruit.h>

#include "../BatteryManager/IBatteryManager.h"
#include "../Ble/BlePeripheral.h"


class HidBatteryTask : private TS::Task
{
private:
	BatteryManager::IBatteryManager* BMS;

	BlePeripheral& BleDev;

private:
	BatteryManager::BatteryStateStruct BatteryState{};

public:
	HidBatteryTask(TS::Scheduler& scheduler,
		BatteryManager::IBatteryManager* bms,
		BlePeripheral& bleDevice)
		: TS::Task(RetroBle::BleConfig::BATTERY_UPDATE_PERIOD_MILLIS, TASK_FOREVER, &scheduler, false)
		, BMS(bms)
		, BleDev(bleDevice)
	{
	}

	void Enable()
	{
		TS::Task::enable();
		BMS->GetBatteryState(BatteryState);
		BleDev.NotifyBattery(BatteryManager::ChargeLevelPercent(BatteryState));
	}

	void Disable()
	{
		TS::Task::disable();
	}

	virtual bool Callback() final
	{
		BMS->GetBatteryState(BatteryState);
		BleDev.NotifyBattery(BatteryManager::ChargeLevelPercent(BatteryState));

		return true;
	}
};

#endif
#endif