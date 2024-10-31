// UsbBleCoordinator.h

#ifndef _USB_BLE_COORDINATOR_h
#define _USB_BLE_COORDINATOR_h

#if defined(_TASK_OO_CALLBACKS)
#include <TSchedulerDeclarations.hpp>

#include "../BatteryManager/IBatteryManager.h"
#include "../Indicator/IIndicator.h"

#include "../Usb/UsbPeripheral.h"
#include "../Ble/BlePeripheral.h"
#include "../HidDevice/IHidDevice.h"

class UsbBleCoordinator : public virtual IUsbListener, public virtual IBleListener, private TS::Task
{
private:
	enum class StateEnum
	{
		Sleep,
		Booting,
		Waking,
		BleAdvertise,
		BleConnected,
		UsbConnected,
		PowerDown
	};

private:
	UsbPeripheral& UsbDev;
	BlePeripheral& BleDev;

private:
	IIndicator* Lights;
	IHidDevice* HidMapper;
	BatteryManager::IBatteryManager* BMS;

private:
	BatteryManager::BatteryStateStruct BatteryState{};
	uint32_t BleStart = 0;
	StateEnum State = StateEnum::Booting;

public:
	UsbBleCoordinator(TS::Scheduler& scheduler,
		BatteryManager::IBatteryManager* bms,
		IIndicator* lights,
		IHidDevice* hidMapper,
		UsbPeripheral& usbDevice,
		BlePeripheral& bleDevice)
		: IBleListener()
		, TS::Task(0, TASK_FOREVER, &scheduler, false)
		, UsbDev(usbDevice)
		, BleDev(bleDevice)
		, Lights(lights)
		, HidMapper(hidMapper)
		, BMS(bms)
	{}

	const bool Start()
	{
		if (BMS != nullptr
			&& Lights != nullptr
			&& HidMapper != nullptr)
		{
			State = StateEnum::Booting;
			TS::Task::enableDelayed(0);

			BleDev.SetBleListener(this);
			//usbDevice.SetUsbListener(this);

			return true;
		}
		return false;
	}

	virtual bool Callback() final
	{
		switch (State)
		{
		case StateEnum::Booting:
			State = StateEnum::Waking;
			BatteryState.Charging = false;
			Lights->SetDrawMode(IIndicator::StateEnum::Off, BatteryState.Charging);
			TS::Task::delay(0);
			break;
		case StateEnum::Waking:
			BMS->GetBatteryState(BatteryState);
			Lights->SetDrawMode(IIndicator::StateEnum::Off, BatteryState.Charging);
			if (UsbDev.IsConnected())
			{
				HidMapper->SetTarget(IHidDevice::TargetEnum::Usb);
				BleDev.Stop();
				TS::Task::delay(0);
				State = StateEnum::UsbConnected;
			}
			else
			{
				HidMapper->SetTarget(IHidDevice::TargetEnum::None);
				BleDev.Start();
				BleStart = millis();
				TS::Task::delay(0);
				State = StateEnum::BleAdvertise;
			}
			break;
		case StateEnum::BleAdvertise:
			if (UsbDev.IsConnected()) // Disconnect from BLE if USB is connected.				
			{
				HidMapper->SetTarget(IHidDevice::TargetEnum::Usb);
				BleDev.Stop();
				TS::Task::delay(0);
				State = StateEnum::UsbConnected;
			}
			else if (BleDev.IsConnected()) // BLE connected.
			{
				Lights->SetDrawMode(IIndicator::StateEnum::Ble, BatteryState.Charging);
				HidMapper->SetTarget(IHidDevice::TargetEnum::Ble);
				BleStart = millis();
				TS::Task::delay(0);
				State = StateEnum::BleConnected;
			}
			else if (BleDev.IsAdvertising()
				&& (millis() - BleStart) < RetroBle::BleConfig::ADVERTISE_NO_ACTIVITY_TIMEOUT_MILLIS)
			{
				// Keep the lights updated.
				BMS->GetBatteryState(BatteryState);
				Lights->SetDrawMode(IIndicator::StateEnum::Searching, BatteryState.Charging);
				TS::Task::delay(RetroBle::BleConfig::BATTERY_UPDATE_PERIOD_MILLIS);
			}
			else // Automatic advertising time out.
			{
				State = StateEnum::Sleep;
				TS::Task::delay(0);
			}
			break;
		case StateEnum::BleConnected:
			if (UsbDev.IsConnected()) // Disconnect from BLE if USB is connected.
			{
				BleDev.Stop();
				HidMapper->SetTarget(IHidDevice::TargetEnum::Usb);
				TS::Task::delay(0);
				State = StateEnum::UsbConnected;
			}
			else if (BleDev.IsConnected())
			{
				if (HidMapper->IsPowerDownRequested()) // Long press to shutdown.
				{
					BleDev.Stop();
					Lights->SetDrawMode(IIndicator::StateEnum::Off, BatteryState.Charging);
					HidMapper->SetTarget(IHidDevice::TargetEnum::None);
					State = StateEnum::PowerDown;
				}
				else if ((millis() - BleStart) > RetroBle::BleConfig::CONNECTED_NO_ACTIVITY_TIMEOUT_MILLIS
					&& HidMapper->GetElapsedMillisSinceLastActivity() >= RetroBle::BleConfig::CONNECTED_NO_ACTIVITY_TIMEOUT_MILLIS)
				{
					State = StateEnum::Sleep;
					TS::Task::delay(0);
				}
				else // No activity time-out or long press to shutdown.
				{
					BMS->GetBatteryState(BatteryState);
					Lights->SetDrawMode(IIndicator::StateEnum::Ble, BatteryState.Charging);
					TS::Task::delay(RetroBle::BleConfig::BATTERY_UPDATE_PERIOD_MILLIS);

					BleDev.NotifyBattery(BatteryManager::ChargeLevelPercent(BatteryState));
				}
			}
			else // BLE disconnected.
			{
				HidMapper->SetTarget(IHidDevice::TargetEnum::None);
				State = StateEnum::Sleep;
				TS::Task::delay(0);
			}
			break;
		case StateEnum::UsbConnected:
			if (UsbDev.IsConnected()) // No time-out when connected through USB.
			{
				BMS->GetBatteryState(BatteryState);
				Lights->SetDrawMode(IIndicator::StateEnum::Usb, BatteryState.Charging);
				TS::Task::delay(RetroBle::BleConfig::BATTERY_UPDATE_PERIOD_MILLIS);
			}
			else // USB disconnected, restart state.
			{
				HidMapper->SetTarget(IHidDevice::TargetEnum::None);
				State = StateEnum::Sleep;
				TS::Task::delay(0);
			}
			break;
		case StateEnum::PowerDown:
			if (!HidMapper->IsPowerDownRequested())
			{
				State = StateEnum::Sleep;
				TS::Task::delay(0);
			}
			else
			{
				TS::Task::delay(RetroBle::BleConfig::BATTERY_UPDATE_PERIOD_MILLIS);
			}
			break;
		case StateEnum::Sleep:
		default:
			// Double check BLE is disabled.
			BleDev.Stop();

			// Last battery state update before sleep.
			BMS->GetBatteryState(BatteryState);

			// Clear update target.
			HidMapper->SetTarget(IHidDevice::TargetEnum::None);

			// Update lights.
			Lights->SetDrawMode(IIndicator::StateEnum::Off, BatteryState.Charging);

			///////// Sleep... ////////
			if (HidMapper->WakeOnInterrupt()
				&& BMS->WakeOnInterrupt())
			{
				ExecuteLowPowerMode();
			}
			// Just woke up, or failed to got to sleep.
			// Restore wake/input pin configuration.
			HidMapper->OnWakeUp();
			BMS->OnWakeUp();
			///////////////////////////

			// Restart on wakeup.
			State = StateEnum::Waking;
			TS::Task::delay(0);
			break;
		}

		return true;
	}

public:
	virtual void OnBleStateChange() final
	{
		TS::Task::delay(0);
	}

	virtual void OnUsbStateChange() final
	{
		TS::Task::delay(0);
	}

	virtual void OnUsbBackReport(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) final
	{
		//TODO: Forward to listener.
	}

private:
	void ExecuteLowPowerMode()
	{
#if defined(ARDUINO_Seeed_XIAO_nRF52840_Sense) || defined(ARDUINO_Seeed_XIAO_nRF52840)
		// Power down nRF52: puts the whole nRF52 to deep sleep (no Bluetooth).  If no sense pins are setup (or other hardware interrupts), the nrf52 will not wake up.
		sd_power_system_off();
#else
		delay(1);
#endif
	}
};
#endif
#endif