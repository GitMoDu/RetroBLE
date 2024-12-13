// RetroBleDevice.h

#ifndef _RETRO_BLE_DEVICE_h
#define _RETRO_BLE_DEVICE_h

#include <stdint.h>

namespace RetroBle
{
	namespace Device
	{
		/// <summary>
		/// Same "manufacturer" for all devices.
		/// </summary>
		static constexpr char Manufacturer[] = "RetroBLE";

		/// <summary>
		/// TinyUSB VID.
		/// </summary>
		static constexpr uint16_t VendorId = 2886;

		/// <summary>
		/// Each Base product group can have up 100 variants.
		/// </summary>
		static constexpr uint8_t BaseCount = 100;

		/// <summary>
		/// Base of ProductId indexes.
		/// Each product base can have up to 100 variant ProductIds.
		/// Roughly ordered by release date.
		/// </summary>
		enum class ProductBase : uint16_t
		{
			/// <summary>
			/// Tiny USB Gamepad as base PID.
			/// </summary>
			BaseStart = 8044,

			/// <summary>
			/// Atari 2600 and compatibles.
			/// </summary>
			Atari = (uint16_t)BaseStart + 1,

			/// <summary>
			/// Nintendo Entertainment System (Famicom).
			/// </summary>
			Nes = (uint16_t)Atari + GroupCount,

			/// <summary>
			/// Sega Master System.
			/// </summary>
			MasterSystem = (uint16_t)Nes + GroupCount,

			/// <summary>
			/// Sega MegaDrive (Genesis).
			/// </summary>
			MegaDrive = (uint16_t)MasterSystem + GroupCount,

			/// <summary>
			/// Super Nintendo Entertainment System (Super Famicom).
			/// </summary>
			Snes = (uint16_t)MegaDrive + GroupCount,

			/// <summary>
			/// Sega Saturn.
			/// </summary>
			Saturn = (uint16_t)Snes + GroupCount,

			/// <summary>
			/// Sony Playstation 1,2,3,4,5, etc...
			/// </summary>
			Playstation = (uint16_t)Saturn + GroupCount,

			/// <summary>
			/// Nintendo 64.
			/// </summary>
			Nintendo64 = (uint16_t)Playstation + GroupCount,

			/// <summary>
			/// Sega Dreamcast.
			/// </summary>
			Dreamcast = (uint16_t)Nintendo64 + GroupCount,

			/// <summary>
			/// Nintendo GameCube.
			/// </summary>
			GameCube = (uint16_t)Dreamcast + GroupCount,

			/// <summary>
			/// Microsoft XBox Og, 360, X, Series, etc...
			/// </summary>
			XBox = (uint16_t)GameCube + GroupCount,

			/// <summary>
			/// Nintendo Wii.
			/// </summary>
			Wii = (uint16_t)XBox + GroupCount,

			/// <summary>
			/// Nintendo Wii-U.
			/// </summary>
			WiiU = (uint16_t)Wii + GroupCount,

			/// <summary>
			/// Nintendo Switch.
			/// </summary>
			Switch = (uint16_t)WiiU + GroupCount,

			/// <summary>
			/// Custom controllers not tied to a console platform.
			/// </summary>
			Custom = INT16_MAX
		};

		/// <summary>
		/// Product Id table for RetroBLE.
		/// </summary>
		enum class ProductIds : uint16_t
		{
			/// <summary>
			/// OG controller with 1 button.
			/// </summary>
			Atari = (uint16_t)ProductBase::Atari,

			/// <summary>
			/// Classic Famicom 2 button controller.
			/// </summary>
			Nes = (uint16_t)ProductBase::Nes,

			/// <summary>
			/// Super Famicom controller with L&R.
			/// </summary>
			Snes = (uint16_t)ProductBase::Snes,

			/// <summary>
			/// Master System controller.
			/// </summary>
			MasterSystem = (uint16_t)ProductBase::MasterSystem,

			/// <summary>
			/// Mega Drive controller (most common in EU).
			/// </summary>
			MegaDrive3Button = (uint16_t)ProductBase::MegaDrive,

			/// <summary>
			/// Genesis controller (most common in US).
			/// </summary>
			MegaDrive6Button = (uint16_t)ProductBase::MegaDrive + 1,

			/// <summary>
			/// Saturn original controller.
			/// </summary>
			Saturn = (uint16_t)ProductBase::Saturn,

			/// <summary>
			/// Special Analog version of the Saturn controller.
			/// </summary>
			SaturnAnalog = (uint16_t)ProductBase::Saturn + 1,

			/// <summary>
			/// Original Playstation 1 controller without analog sticks.
			/// </summary>
			Playstation1 = (uint16_t)ProductBase::Playstation,

			/// <summary>
			/// 2nd version of the Playstation 1 controller with analog sticks.
			/// </summary>
			Playstation1Analog = (uint16_t)ProductBase::Playstation + 1,

			/// <summary>
			/// Playstation 2 controller.
			/// </summary>
			Playstation2 = (uint16_t)ProductBase::Playstation + 2,

			/// <summary>
			/// Playstation 3 controller.
			/// </summary>
			Playstation3 = (uint16_t)ProductBase::Playstation + 3,

			/// <summary>
			/// Playstation 4 controller.
			/// </summary>
			Playstation4 = (uint16_t)ProductBase::Playstation + 4,

			/// <summary>
			/// Playstation 5 controller.
			/// </summary>
			Playstation5 = (uint16_t)ProductBase::Playstation + 5,

			/// <summary>
			/// Nintendo 64 "Trident" controller.
			/// </summary>
			Nintendo64 = (uint16_t)ProductBase::Nintendo64,

			/// <summary>
			/// Dreamcast controller.
			/// </summary>
			Dreamcast = (uint16_t)ProductBase::Dreamcast,

			/// <summary>
			/// GameCube controller.
			/// </summary>
			GameCube = (uint16_t)ProductBase::GameCube,

			/// <summary>
			/// DK Congas for GameCube.
			/// </summary>
			GameCubeDkCongas = (uint16_t)ProductBase::GameCube + 1,
		};
	}
}
#endif