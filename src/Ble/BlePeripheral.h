// BlePeripheral.h

#ifndef _BLE_PERIPHERAL_h
#define _BLE_PERIPHERAL_h

#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <bluefruit.h>

#include "BleConfig.h"

#include "../Framework/RetroBleDevice.h"


/// <summary>
/// Retro BLE device Bluetooth manager: discovery + battery + user services.
/// Exposes the current connection and advertising state.
/// Provides callbacks for connection and advertising events.
/// BLE Battery state update.
/// 
/// TODO: Callbacks for events dispatched by central.
/// 
/// No auto-restart on disconnect.
/// Fixed 30s fast advertising.
/// Fixed 60s advertising timeout.
/// </summary>
class BlePeripheral
{
private:
	static constexpr uint32_t ADVERTISE_FAST_TIMEOUT_MILLIS = 30000;

private:
	BLEDis BleDiscovery{};
	BLEBas BleBattery{};

private:
	IBleListener* Listener = nullptr;
	uint16_t Handle = 0;

public:
	BlePeripheral()
	{}

	void Setup(BLEService& deviceService,
		void (*onConnect)(const uint16_t conn_hdl),
		void (*onDisconnect)(const uint16_t conn_hdl, const uint8_t reason),
		void (*onAdvertiseStop)(),
		void (*onBleEvent)(ble_evt_t* bleEvent),
		const RetroBle::BleConfig::Appearance appearance,
		const char* name,
		const char* version)
	{
		// Setup BLE and advertising service.
		SetupBle(onConnect, onDisconnect, onBleEvent, name, version, RetroBle::BleConfig::TxPower);

		// Start all services.
		BleDiscovery.begin();
		BleBattery.begin();
		deviceService.begin();

		SetupAdvertisement(deviceService, onAdvertiseStop, (uint16_t)appearance);
	}

	void SetBleListener(IBleListener* listener)
	{
		Listener = listener;
	}

	const bool IsConnected()
	{
		return Bluefruit.connected();
	}

	const bool IsAdvertising()
	{
		return Bluefruit.Advertising.isRunning();
	}

	void Start()
	{
		Bluefruit.Advertising.start(0); // 0 = Don't stop advertising after n seconds
	}

	void Stop()
	{
		Bluefruit.Advertising.stop();

		if (IsConnected())
		{
			Bluefruit.disconnect(Handle);
		}
	}

	/// <summary>
	/// Notify BLE Battery service of update charge level.
	/// </summary>
	/// <param name="chargePercentage"></param>
	void NotifyBattery(const uint8_t chargePercentage)
	{
		if (IsConnected())
		{
			BleBattery.notify(chargePercentage);
		}
	}

	const bool GetCentralName(char name[32])
	{
		if (IsConnected())
		{
			// Get the reference to current connection
			BLEConnection* connection = Bluefruit.Connection(Handle);

			if (connection != nullptr)
			{
				connection->getPeerName(name, 32);
				return true;
			}
		}

		return false;
	}

public:
	void OnConnectInterrupt(uint16_t conn_hdl)
	{
		Handle = conn_hdl;
		if (Listener != nullptr)
		{
			Listener->OnBleStateChange();
		}
	}

	void OnDisconnectInterrupt(uint16_t conn_hdl, uint8_t reason)
	{
		Handle = 0;
		if (Listener != nullptr)
		{
			Listener->OnBleStateChange();
		}
	}

	void OnBleEventInterrupt(ble_evt_t* bleEvent)
	{
		//TODO: Forward event to relevant listener.
	}

private:
	void SetupBle(
		void (*onConnect)(const uint16_t conn_hdl),
		void (*onDisconnect)(const uint16_t conn_hdl, const uint8_t reason),
		void (*onBleEvent)(ble_evt_t* bleEvent),
		const char* name,
		const char* version,
		const int8_t txPower)
	{
		// Init Bluefruit
		Bluefruit.begin();
		Bluefruit.setTxPower(txPower);
		Bluefruit.setName(name);

		// Disable automatic LED handling.
		Bluefruit.autoConnLed(false);
		Bluefruit.setConnLedInterval(UINT32_MAX);

		// Set event callback listener.
		Bluefruit.setEventCallback(onBleEvent);

		// Configure and Start Device Information Service
		BleDiscovery.setManufacturer(RetroBle::Device::Manufacturer);
		BleDiscovery.setModel(name);
		BleDiscovery.setSoftwareRev(version);

		// Set connection interval (i.e. poll rate).
		Bluefruit.Periph.setConnInterval(RetroBle::BleConfig::ConnectionInterval::Min, RetroBle::BleConfig::ConnectionInterval::Max);

		// Set connection callback listeners.
		Bluefruit.Periph.setConnectCallback(onConnect);
		Bluefruit.Periph.setDisconnectCallback(onDisconnect);

		Bluefruit.ScanResponse.addName();
	}

	void SetupAdvertisement(BLEService& deviceService, void (*onAdvertiseStop)(), const uint16_t appearance)
	{
		// Advertising packet
		Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
		Bluefruit.Advertising.addTxPower();
		Bluefruit.Advertising.addAppearance(appearance);

		Bluefruit.Advertising.setStopCallback(onAdvertiseStop);

		// Add BLE services.
		Bluefruit.Advertising.addService(deviceService);
		Bluefruit.Advertising.addService(BleBattery);

		// There is enough room for the dev name in the advertising packet
		Bluefruit.Advertising.addName();

		// Configure advertising, no auto-restart on disconnect.
		Bluefruit.Advertising.restartOnDisconnect(false);
		Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms

		// Number of seconds in fast mode.
		Bluefruit.Advertising.setFastTimeout(ADVERTISE_FAST_TIMEOUT_MILLIS / 1000);
	}
};

#endif

