// BleCentral.h

#ifndef _BLE_CENTRAL_h
#define _BLE_CENTRAL_h

#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <bluefruit.h>

#include "BleConfig.h"

#include "../Framework/RetroBleDevice.h"

class IHidListener
{
public:
	virtual void OnReportNotify(uint8_t* data, const uint16_t size, const uint16_t uuid) {}
	virtual void OnStateChange(const bool connected) {}
};

class BleCentral
{
private:
	uint16_t Handle = 0;

private:
	BLEClientCharacteristic CharaReport;
	BLEClientCharacteristic CharaReportMap;

	BLEClientService XBoxService;

private:
	IHidListener* HidListener;

public:
	BleCentral(IHidListener* hidListener)
		: CharaReport(UUID16_CHR_REPORT)
		, CharaReportMap(UUID16_CHR_REPORT_MAP)
		, XBoxService(UUID16_SVC_HUMAN_INTERFACE_DEVICE)
		, HidListener(hidListener)
	{}

	void Setup(void (*onConnect)(const uint16_t conn_hdl),
		void (*onDisconnect)(const uint16_t conn_hdl, const uint8_t reason),
		void (*onScanCallback)(ble_gap_evt_adv_report_t* report),
		void (*onConnectionSecured)(uint16_t conn_handle),
		void (*onReportNotify)(BLEClientCharacteristic* chr, uint8_t* data, uint16_t len))
	{
		CharaReport.setNotifyCallback(onReportNotify);

		XBoxService.begin();

		CharaReport.begin();
		CharaReportMap.begin();

		SetupBle(onConnect, onDisconnect, onScanCallback, onConnectionSecured);
	}

	void OnConnectionSecured(uint16_t conn_hdl)
	{
		BLEConnection* conn = Bluefruit.Connection(conn_hdl);

		if (!conn->secured())
		{
			// It is possible that connection is still not secured by this time.
			// This happens (central only) when we try to encrypt connection using
			// stored bond keys but peer reject it (probably it remove its stored key).
			// Therefore we will request an pairing again --> callback again when
			// encrypted
			conn->requestPairing();
		}
		else
		{
			if (!CharaReportMap.discover())
			{
				// Measurement chr is mandatory, if it is not found (valid), then disconnect
#if defined(DEBUG)
				Serial.println("characteristic report map not found !!!");
#endif
				Bluefruit.disconnect(conn_hdl);
				return;
			}

			if (!CharaReport.discover())
			{
#if defined(DEBUG)
				// Measurement chr is mandatory, if it is not found (valid), then disconnect
				Serial.println("characteristic report not found !!!");
#endif
				Bluefruit.disconnect(conn_hdl);
				return;
			}

			if (CharaReport.enableNotify())
			{
				if (HidListener != nullptr)
				{
					HidListener->OnStateChange(true);
				}
#if defined(DEBUG)
				Serial.println(F("Controller connected"));
#endif
			}
#if defined(DEBUG)
			else
			{
				Serial.println("Couldn't enable notify for report. Increase DEBUG LEVEL for troubleshooting");
			}
#endif
		}
	}

	void OnConnect(uint16_t conn_hdl)
	{
#if defined(DEBUG)
		Serial.println(F("Connecting"));
#endif
		Handle = conn_hdl;

		BLEConnection* conn = Bluefruit.Connection(conn_hdl);

		if (XBoxService.discover(conn_hdl))
		{
			conn->requestPairing();
		}
	}

	void OnDisconnect(uint16_t conn_hdl, uint8_t reason)
	{
#if defined(DEBUG)
		Serial.println(F("Controller disconnected"));
#endif
		Handle = 0;
		if (HidListener != nullptr)
		{
			HidListener->OnStateChange(false);
		}
	}

	void OnScanCallback(ble_gap_evt_adv_report_t* report)
	{
		Bluefruit.Central.connect(report);
		// For Softdevice v6: after received a report, scanner will be paused
		// We need to call Scanner resume() to continue scanning
		Bluefruit.Scanner.resume();
	}

	void OnReportNotify(BLEClientCharacteristic* chr, uint8_t* data, const uint16_t len)
	{
		if (chr != nullptr
			&& data != nullptr
			&& HidListener != nullptr)
		{
			HidListener->OnReportNotify(data, len, chr->uuid._uuid.uuid);
		}
	}

private:
	void SetupBle(void (*onConnect)(const uint16_t conn_hdl),
		void (*onDisconnect)(const uint16_t conn_hdl, const uint8_t reason),
		void (*onScanCallback)(ble_gap_evt_adv_report_t* report),
		void (*onConnectionSecured)(uint16_t conn_handle))
	{
		// Initialize Bluefruit with maximum connections as Peripheral = 0, Central =
		// 1 SRAM usage required by SoftDevice will increase dramatically with number
		// of connections
		Bluefruit.begin(0, 1);
		Bluefruit.setTxPower(4);  // Check bluefruit.h for supported values

		/* Set the device name */
		Bluefruit.setName(RetroBle::Device::Manufacturer);

		// Disable automatic LED handling.
		Bluefruit.autoConnLed(true);
		Bluefruit.setConnLedInterval(UINT32_MAX);

		Bluefruit.Central.setConnectCallback(onConnect);
		Bluefruit.Central.setDisconnectCallback(onDisconnect);
		Bluefruit.Security.setSecuredCallback(onConnectionSecured);
		Bluefruit.Scanner.setRxCallback(onScanCallback);
		Bluefruit.Scanner.restartOnDisconnect(true);
		Bluefruit.Scanner.filterUuid(XBoxService.uuid);
		Bluefruit.Scanner.filterRssi(-80);
		Bluefruit.Scanner.setInterval(160, 80);  // in units of 0.625 ms
		Bluefruit.Scanner.useActiveScan(true);   // Request scan response data
		Bluefruit.Scanner.start(0);  // 0 = Don't stop scanning after n seconds
	}
};
#endif