#ifndef _I_SLEEP_h
#define _I_SLEEP_h

namespace BatteryManager
{
	/// <summary>
	/// Class interface for sleep/wake capable peripheral.
	/// </summary>
	class ISleep
	{
	public:
		/// <summary>
		/// Sleep peripheral and setup wake event.
		/// </summary>
		/// <returns>False if sleep isn't possible right now.</returns>
		virtual bool WakeOnInterrupt() = 0;

		/// <summary>
		/// Device has just woken up from sleep, restore peripheral.
		/// </summary>
		virtual void OnWakeUp() = 0;
	};
}
#endif