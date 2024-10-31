// Bq25100Driver.h

#ifndef _BQ_25100_DRIVER_h
#define _BQ_25100_DRIVER_h

#if defined(_TASK_OO_CALLBACKS)
#include <TSchedulerDeclarations.hpp>

#include <Arduino.h>

#include "../IBatteryManager.h"
#include "../BatteryState.h"

#include "../HistoryAverage.h"

/// <summary>
/// Example calibration definition for Bq25100Driver.
/// </summary>
struct Bq25100Calibration
{
	static constexpr uint16_t R1 = 1000;
	static constexpr uint16_t R2 = 510;

	static constexpr uint16_t VMin = 3600;
	static constexpr uint16_t VMax = 4000;
	static constexpr uint16_t VMaxCharging = 4200;

	static constexpr uint32_t AdcSettleMicros = 15;
};

/// <summary>
/// Battery Manager driver implementation for Bq25100 BMS.
/// Implements IBatteryManager.
/// </summary>
/// <typeparam name="Pin">Driver pin definition.</typeparam>
/// <typeparam name="Calibration">Driver calibration definition.</typeparam>
/// <typeparam name="UpdatePeriod">Manager read step update period in milliseconds.</typeparam>
/// <typeparam name="HistorySize">History size of rolling average.</typeparam>
template<uint32_t UpdatePeriod, uint8_t HistorySize,
	typename Pin,
	typename Calibration = Bq25100Calibration>
class Bq25100Driver : public virtual BatteryManager::IBatteryManager, private TS::Task
{
private:
	HistoryAverage<HistorySize> Average{};

private:
	bool Charging = false;

public:
	Bq25100Driver(TS::Scheduler& scheduler)
		: BatteryManager::IBatteryManager()
		, TS::Task(UpdatePeriod, TASK_FOREVER, &scheduler, false)
	{}

	void Start()
	{
		TS::Task::enable();
	}

	void Stop()
	{
		DisableProbe();
		TS::Task::disable();
	}

	void Setup()
	{
		// Setup read pins.
		pinMode((uint8_t)Pin::VIn, INPUT);
		pinMode((uint8_t)Pin::Charging, INPUT);

		// Disable reading of the BAT voltage.
		pinMode((uint8_t)Pin::Enable, OUTPUT);
		digitalWrite((uint8_t)Pin::Enable, HIGH);

		// Set 100mA charge current.
		//pinMode((uint8_t)Pin::ChargeCurrent, INPUT);
		pinMode((uint8_t)Pin::ChargeCurrent, OUTPUT);
		digitalWrite((uint8_t)Pin::ChargeCurrent, LOW);

		// Clear history.
		Average.Clear(GetBatteryVoltage());

		// Clear state.
		Charging = !digitalRead((uint8_t)Pin::Charging);
	}

	/// <summary>
	/// Wake on battery charging state change.
	/// </summary>
	/// <returns>False if sleep isn't possible right now.</returns>
	virtual const bool WakeOnInterrupt() final
	{
		if (Charging == !digitalRead((uint8_t)Pin::Charging))
		{
#if defined(ARDUINO_Seeed_XIAO_nRF52840_Sense) || defined(ARDUINO_Seeed_XIAO_nRF52840)
			if (Charging)
			{
				pinMode((uint8_t)Pin::Charging, INPUT_SENSE_HIGH);// Wakes up when externally connected to 3.3V.
			}
			else
			{
				pinMode((uint8_t)Pin::Charging, INPUT_SENSE_LOW);	// Wakes up when externally connected to ground.
			}

#else
			pinMode((uint8_t)Pin::Charging, INPUT);
#endif
			return true;
		}

		return false;
	}

	virtual void OnWakeUp() final
	{
		pinMode((uint8_t)Pin::Charging, INPUT);
		Task::delay(0);
	}

	virtual void GetBatteryState(BatteryManager::BatteryStateStruct& batteryState) final
	{
		Charging = !digitalRead((uint8_t)Pin::Charging);
		batteryState.ChargeLevel = GetBatteryChargeLevel(Average.GetAverage(), Charging);
		batteryState.Charging = Charging;
	}

	virtual bool Callback() final
	{
		Charging = !digitalRead((uint8_t)Pin::Charging);

		// Make sure probe is disabled on wake if charging.
		if (Charging)
		{
			DisableProbe();
		}

		const uint16_t voltage = GetBatteryVoltage();

		Average.Step(voltage);

		return true;
	}

private:
	const uint8_t GetBatteryChargeLevel(const uint16_t batteryVoltage, const bool charging) const
	{
		uint16_t clipped = 0;
		if (batteryVoltage < Calibration::VMin)
		{
			clipped = Calibration::VMin;
		}
		else
		{
			clipped = batteryVoltage;
		}

		// When charging, show level until expected full charge.
		if (charging)
		{
			if (batteryVoltage >= Calibration::VMaxCharging)
			{
				// Keep at 99% until charging stops.
				return BatteryManager::ChargeLevelMax - 1;
			}
			else
			{
				// Scale 99% until VMaxCharging.
				return (((uint32_t)(clipped - Calibration::VMin)) * (BatteryManager::ChargeLevelMax - 1))
					/ (Calibration::VMaxCharging - Calibration::VMin);
			}
		}
		else // When discharging, show expected battery level.
		{
			if (batteryVoltage >= Calibration::VMax)
			{
				clipped = Calibration::VMax;
			}

			return (((uint32_t)(clipped - Calibration::VMin)) * BatteryManager::ChargeLevelMax)
				/ (Calibration::VMax - Calibration::VMin);
		}
	}

public:
#if defined(ARDUINO_Seeed_XIAO_nRF52840_Sense) || defined(ARDUINO_Seeed_XIAO_nRF52840)
	/// <summary>
	/// 
	/// ADC voltage read for Seeed Xiao nRF52840.
	/// https://wiki.seeedstudio.com/XIAO_BLE#q3-what-are-the-considerations-when-using-xiao-nrf52840-sense-for-battery-charging
	/// https://wiki.seeedstudio.com/XIAO_BLE#battery-charging-current
	/// When P0.14 (D14)turns off the ADC function at a high level of 3.3V, P0.31 will be at the input voltage limit of 3.6V.There is a risk of burning out the P0.31 pin.
	/// Currently for this issue, we recommend that users do not turn off the ADC function of P0.14 (D14) or set P0.14 (D14)to high during battery charging.
	/// </summary>
	/// <returns>Current battery voltage in mV.</returns>
	const uint16_t GetBatteryVoltage()
	{
		uint16_t adc = 0;

		// Set the analog reference to AdcReference = 3.0V (default = 3.6V)
		analogReference(AR_INTERNAL_3_0);

		// Set the resolution to 12-bit (0..4095) (AdcMax)
		analogReadResolution(12); // Can be 8, 10, 12 or 14

		EnableProbe();
		delayMicroseconds(Calibration::AdcSettleMicros); // Let the ADC settle just a bit.
		adc = analogRead((uint8_t)Pin::VIn);
		DisableProbe();

		// Set the ADC back to the default settings
		analogReference(AR_DEFAULT);
		analogReadResolution(10);

		static constexpr uint16_t AdcReference = 3000;
		static constexpr uint16_t AdcMax = 4095;

		return GetVInFromVOut(GetVoltageFromAdc<AdcReference, AdcMax>(adc));
	}
#else
	/// <summary>
	/// Generic Arduino ADC voltage read.
	/// </summary>
	/// <returns>Current battery voltage in mV.</returns>
	const uint16_t GetBatteryVoltage()
	{
		uint16_t adc = 0;

		EnableProbe();
		delayMicroseconds(Calibration::AdcSettleMicros); // Let the ADC settle just a bit.
		adc = analogRead((uint8_t)Pin::VIn);
		DisableProbe();

		return GetVInFromVOut(GetVoltageFromAdc<>(adc));
	}
#endif

	template<uint16_t AdcReference = 3300, uint16_t AdcMax = 1024>
	static const uint16_t GetVoltageFromAdc(const uint16_t adcValue)
	{
		return ((uint32_t)adcValue * AdcReference) / AdcMax;
	}

	static const uint16_t GetVInFromVOut(const uint16_t adcValue)
	{
		return ((uint32_t)adcValue * (Calibration::R2 + Calibration::R1)) / Calibration::R2;
	}

	void DisableProbe()
	{
		//pinMode((uint8_t)Pin::Enable, INPUT);
		pinMode((uint8_t)Pin::Enable, OUTPUT);
		digitalWrite((uint8_t)Pin::Enable, HIGH);
	}

	void EnableProbe()
	{
		pinMode((uint8_t)Pin::Enable, OUTPUT);
		digitalWrite((uint8_t)Pin::Enable, LOW);
	}
};
#endif
#endif