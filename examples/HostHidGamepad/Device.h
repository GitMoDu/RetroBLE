#ifndef _DEVICE_h
#define _DEVICE_h

#if defined(ARDUINO_ARCH_NRF52)
#include <RetroBle.h>
#include <VirtualPad.h>

namespace Device
{
#if defined(DEBUG)
	namespace Debug
	{
		static constexpr uint32_t SERIAL_BAUD_RATE = 115200;
	}
#endif
	static constexpr char Name[] = "VirtualPadHost";

	namespace Version
	{
		static constexpr uint16_t Code = 0;
		static constexpr char Name[] = "0";
	}

	namespace UartInterface
	{
		static constexpr auto& UartInstance = Serial1;

		enum class Pin : uint8_t
		{
			Rx = PIN_SERIAL1_RX,
			Tx = PIN_SERIAL1_TX
		};

	}

	namespace BLE
	{
	}

	namespace Unused
	{
		static constexpr uint8_t Pins[] = { };
	}

	namespace VirtualPadUartInterface
	{
		static constexpr uint32_t ConfigurationCode =
			VirtualPad::Configuration::GetConfigurationCode(
				VirtualPad::Configuration::GetFeatureFlags<VirtualPad::Configuration::FeaturesEnum::DPad,
				VirtualPad::Configuration::FeaturesEnum::Joy1,
				VirtualPad::Configuration::FeaturesEnum::Joy2,
				VirtualPad::Configuration::FeaturesEnum::Start, VirtualPad::Configuration::FeaturesEnum::Select,
				VirtualPad::Configuration::FeaturesEnum::Home, VirtualPad::Configuration::FeaturesEnum::Share,
				VirtualPad::Configuration::FeaturesEnum::A, VirtualPad::Configuration::FeaturesEnum::B,
				VirtualPad::Configuration::FeaturesEnum::X, VirtualPad::Configuration::FeaturesEnum::Y,
				VirtualPad::Configuration::FeaturesEnum::L1, VirtualPad::Configuration::FeaturesEnum::R1,
				VirtualPad::Configuration::FeaturesEnum::L2, VirtualPad::Configuration::FeaturesEnum::R2,
				VirtualPad::Configuration::FeaturesEnum::L3, VirtualPad::Configuration::FeaturesEnum::R3>(),
				VirtualPad::Configuration::NoProperties,
				VirtualPad::Configuration::NavigationEnum::AB);
	}
};

#else
#error Device is not supported for this project.
#endif

#endif

