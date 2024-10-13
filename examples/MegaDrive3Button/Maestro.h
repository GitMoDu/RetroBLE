// Maestro.h

#ifndef _CONNECTIONWATCHER_h
#define _CONNECTIONWATCHER_h

#define _TASK_OO_CALLBACKS
#include <TaskSchedulerDeclarations.h>

#include <RetroBle.h>
#include "Device.h"

template<typename UsbPadMapperType, typename BlePadMapperType>
class MaestroTask : public virtual IBleListener, private TS::Task
{
private:
	enum class StateEnum
	{
		Sleep,
		Booting,
		Waking,
		BleAdvertise,
		BleConnected,
		UsbConnected
	};

private:
	BatteryManager::IBatteryManager* BMS;

	LedAnimatorTask& Lights;

	UsbPeripheral& UsbDev;
	UsbPadMapperType& UsbPad;

	BlePeripheral& BleDev;
	BlePadMapperType& BlePad;

private:
	BatteryManager::BatteryStateStruct BatteryState{};

	uint32_t LastActivity = 0;

	StateEnum State = StateEnum::Booting;

public:
	MaestroTask(TS::Scheduler& scheduler,
		BatteryManager::IBatteryManager* bms,
		LedAnimatorTask& lights,
		UsbPeripheral& usbDevice,
		UsbPadMapperType& usbPad,
		BlePeripheral& bleDevice,
		BlePadMapperType& blePad
	)
		: IBleListener()
		, TS::Task(0, TASK_FOREVER, &scheduler, false)
		, BMS(bms)
		, Lights(lights)
		, UsbDev(usbDevice)
		, UsbPad(usbPad)
		, BleDev(bleDevice)
		, BlePad(blePad)
	{
	}

	void Start()
	{
		if (BMS != nullptr)
		{
			State = StateEnum::Booting;
			TS::Task::enableDelayed(0);

			BleDev.SetBleListener(this);

			//UsbPad.SetUsbListener(this);
		}
	}

	virtual bool Callback() final
	{
		switch (State)
		{
		case StateEnum::Booting:
			State = StateEnum::Waking;
			BatteryState.Charging = false;
			Lights.Start();
			TS::Task::delay(0);
			break;
		case StateEnum::Waking:
			BMS->GetBatteryState(BatteryState);
			Lights.SetDrawMode(LedAnimatorTask::ConnectionLights::Off, BatteryState.Charging);
			if (UsbConnected())
			{
				BleDev.Stop();
				BlePad.Stop();
				UsbPad.Start();
				TS::Task::delay(0);
				State = StateEnum::UsbConnected;
			}
			else
			{
				UsbPad.Stop();
				BleDev.Start();
				TS::Task::delay(0);
				State = StateEnum::BleAdvertise;
			}
			break;
		case StateEnum::BleAdvertise:

			if (UsbConnected())
			{
				// Disconnect from BLE if USB is connected.
				BleDev.Stop();
				BlePad.Stop();
				UsbPad.Start();
				TS::Task::delay(0);
				State = StateEnum::UsbConnected;
			}
			else if (BleDev.IsConnected())
			{
				State = StateEnum::BleConnected;
				Lights.SetDrawMode(LedAnimatorTask::ConnectionLights::Ble, BatteryState.Charging);
				BlePad.Start();
				TS::Task::delay(0);

				
			}
			else if (BleDev.IsAdvertising()
				&& (millis() - LastActivity < RetroBle::BleConfig::ADVERTISE_NO_ACTIVITY_TIMEOUT_MILLIS))
			{
				BMS->GetBatteryState(BatteryState);
				Lights.SetDrawMode(LedAnimatorTask::ConnectionLights::Searching, BatteryState.Charging);
				TS::Task::delay(RetroBle::BleConfig::BATTERY_UPDATE_PERIOD_MILLIS);
			}
			else
			{
				// Automatic advertising time out.
				State = StateEnum::Sleep;
				TS::Task::delay(0);
			}
			break;
		case StateEnum::BleConnected:
			if (UsbConnected())
			{
				// Disconnect from BLE if USB is connected.
				BleDev.Stop();
				BlePad.Stop();
				UsbPad.Start();
				TS::Task::delay(0);
				State = StateEnum::UsbConnected;
			}
			else if (BleDev.IsConnected())
			{
				if (BlePad.GetElapsedMillisSinceLastActivity() < RetroBle::BleConfig::CONNECTED_NO_ACTIVITY_TIMEOUT_MILLIS
					&& !BlePad.IsStartLongPressed(Device::GamePad::LONG_PRESS_POWER_OFF_PERIOD_MILLIS))
				{
					BMS->GetBatteryState(BatteryState);
					Lights.SetDrawMode(LedAnimatorTask::ConnectionLights::Ble, BatteryState.Charging);
					TS::Task::delay(RetroBle::BleConfig::BATTERY_UPDATE_PERIOD_MILLIS);

					BleDev.NotifyBattery(BatteryManager::ChargeLevelPercent(BatteryState));

					//TODO:
					//if (!BlePad.IsConnected())
					//{
					//	BlePad.Clear();
					//}
				}
				else
				{
					// No activity time-out or long press to shutdown.
					State = StateEnum::Sleep;
					TS::Task::delay(0);
				}
			}
			else
			{
				State = StateEnum::BleAdvertise;
				Lights.SetDrawMode(LedAnimatorTask::ConnectionLights::Searching, BatteryState.Charging);
				BlePad.Stop();
				TS::Task::delay(0);
			}
			break;
		case StateEnum::UsbConnected:
			if (UsbConnected())
			{
				// No time-out when connected through USB.
				BMS->GetBatteryState(BatteryState);
				Lights.SetDrawMode(LedAnimatorTask::ConnectionLights::Usb, BatteryState.Charging);
				TS::Task::delay(RetroBle::BleConfig::BATTERY_UPDATE_PERIOD_MILLIS);

				//TODO:
				//if (!BlePad.IsConnected())
				//{
				//	BlePad.Clear();
				//}
			}
			else
			{
				// USB disconnected, restore BLE advertising.
				UsbPad.Stop();
				State = StateEnum::Waking;
				TS::Task::delay(0);
			}
			break;
		case StateEnum::Sleep:
		default:
			// Double check BLE is disabled.
			BleDev.Stop();

			// Stop dynamic tasks.
			BlePad.Stop();
			UsbPad.Stop();
			BMS->Stop();

			// Update lights.
			Lights.SetDrawMode(LedAnimatorTask::ConnectionLights::Off, BatteryState.Charging);

			///////// Sleep... ////////
			ExecuteLowPowerMode();
			///////////////////////////

			// Just woke up!
			// Restore wake/input pin configuration.
			UsbPad.Stop();
			BlePad.Stop();
			BMS->Stop();

			// Restart BMS
			BMS->Start();

			// Restart in advertising mode.
			State = StateEnum::Waking;
			TS::Task::delay(0);
			break;
		}

		return true;
	}

	virtual void OnBleStateChange() final
	{
		OnActivity();
	}

private:
	void OnActivity()
	{
		LastActivity = millis();
		TS::Task::delay(0);
	}

	const bool UsbConnected()
	{
		return UsbDev.IsConnected(); //&& UsbPad.IsConnected();
	}

	void ExecuteLowPowerMode()
	{
		// Wake up gamepad pin.
		// Pulled down and wakes up when externally connected to 3.3V.
		pinMode(Device::GamePad::WakePin, INPUT_PULLDOWN_SENSE);

		bool charging = BatteryState.Charging;

		BMS->GetBatteryState(BatteryState);

		if (charging == BatteryState.Charging)
		{
			// Wake on battery charge change.
			if (!BatteryState.Charging)
			{
				pinMode(Device::BMS::WakePin, INPUT_SENSE_LOW);	// Wakes up when externally connected to ground.
			}
			else
			{
				pinMode(Device::BMS::WakePin, INPUT_SENSE_HIGH);// Wakes up when externally connected to 3.3V.
			}

			//noInterrupts();
			//if (digitalRead(Device::BMS::WakePin))
			//{
			//	pinMode(Device::BMS::WakePin, INPUT_SENSE_LOW);	// Wakes up when externally connected to ground.
			//}
			//else
			//{
			//	pinMode(Device::BMS::WakePin, INPUT_SENSE_HIGH);// Wakes up when externally connected to 3.3V.
			//}
			//interrupts();

			// Power down nRF52: puts the whole nRF52 to deep sleep (no Bluetooth).  If no sense pins are setup (or other hardware interrupts), the nrf52 will not wake up.
			sd_power_system_off();
		}
		else
		{
			// Sleep failed, battery charging state has changed, try again.
		}
	}
};
#endif
