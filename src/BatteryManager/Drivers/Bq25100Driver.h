// Bq25100Driver.h

#ifndef _BQ_25100_DRIVER_h
#define _BQ_25100_DRIVER_h

#define _TASK_OO_CALLBACKS
#include <TaskSchedulerDeclarations.h>

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

	static constexpr uint16_t VMin = 3350;
	static constexpr uint16_t VMax = 3700;

	static constexpr uint16_t AdcReference = 3300;
	static constexpr uint16_t AdcMax = 1023;

	static constexpr uint32_t AdcSettleMicros = 50;
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

	virtual void Start() final
	{
		TS::Task::enable();
	}

	virtual void Stop() final
	{
		Setup();
		DisableProbe();
		TS::Task::disable();
	}

	void Setup()
	{
		// Setup read pins.
		pinMode(Pin::VInPin, INPUT);
		pinMode(Pin::ChargingPin, INPUT);

		// Disable reading of the BAT voltage.
		pinMode(Pin::EnablePin, OUTPUT);
		digitalWrite(Pin::EnablePin, HIGH);

		// Set to slow charge current.
		pinMode(Pin::ChargeCurrentPin, INPUT);

		// Clear history.
		Average.Clear();

		// Clear state.
		Charging = false;
	}

	virtual void GetBatteryState(BatteryManager::BatteryStateStruct& batteryState) final
	{
		batteryState.ChargeLevel = GetBatteryChargeLevel(Average.GetAverage());
		batteryState.Charging = Charging;
	}

	virtual bool Callback() final
	{
		// Only enable level check if NOT charging.
		if (!digitalRead(Pin::ChargingPin))
		{
			if (!Charging)
			{
				Charging = true;
			}

			Average.Step(GetBatteryVoltage());
		}
		else
		{
			if (Charging)
			{
				Charging = false;
				Average.Clear();
			}
		}

		return true;
	}

private:
	const uint8_t GetBatteryChargeLevel(const uint16_t batteryVoltage) const
	{
		uint16_t clipped = 0;
		if (batteryVoltage >= Calibration::VMax)
		{
			clipped = Calibration::VMax;
		}
		else if (batteryVoltage < Calibration::VMin)
		{
			clipped = Calibration::VMin;
		}
		else
		{
			clipped = batteryVoltage;
		}

		return ((uint32_t)(clipped - Calibration::VMin) * BatteryManager::ChargeLevelMax)
			/ (Calibration::VMax - Calibration::VMin);
	}

#if defined(ARDUINO_Seeed_XIAO_nRF52840_Sense) || defined(ARDUINO_Seeed_XIAO_nRF52840)
	/// <summary>
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
		adc = analogRead(Pin::VInPin);
		DisableProbe();

		// Set the ADC back to the default settings
		analogReference(AR_DEFAULT);
		analogReadResolution(10);

		return GetVoltageFromAdc(adc);
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
		adc = analogRead(Pin::VInPin);
		DisableProbe();

		return GetVoltageFromAdc(adc);
	}
#endif

	static constexpr uint16_t GetVoltageFromAdc(const uint16_t adcValue)
	{
		return ((uint32_t)adcValue * Calibration::R2 * Calibration::AdcReference) /
			(((uint32_t)Calibration::R1 + Calibration::R2) * Calibration::AdcMax);
	}

	void DisableProbe()
	{
		pinMode(Pin::EnablePin, INPUT);
	}

	void EnableProbe()
	{
		pinMode(Pin::EnablePin, OUTPUT);
		digitalWrite(Pin::EnablePin, LOW);
	}
};
#endif