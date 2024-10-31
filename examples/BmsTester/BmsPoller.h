// BmsPoller.h

#ifndef _BMSPOLLER_h
#define _BMSPOLLER_h

#include <RetroBle.h>

class BmsPoller : private TS::Task
{
private:
	BatteryManager::IBatteryManager* Bms;

private:
	BatteryManager::BatteryStateStruct BatteryState{};

public:
	BmsPoller(TS::Scheduler& scheduler, BatteryManager::IBatteryManager* bms)
		: TS::Task(RetroBle::BleConfig::BATTERY_UPDATE_PERIOD_MILLIS, TASK_FOREVER, &scheduler, true)
		, Bms(bms)
	{}

	virtual bool Callback() final
	{
		Bms->GetBatteryState(BatteryState);

		if (BatteryState.Charging)
		{
			Serial.println(F("Charging"));
		}
		else
		{
			Serial.println(F("Not Charging"));
		}

		Serial.print(F("Level: "));
		Serial.print(BatteryManager::ChargeLevelPercent(BatteryState.ChargeLevel));
		Serial.println(F(" %"));

		return true;
	}
};


#endif

