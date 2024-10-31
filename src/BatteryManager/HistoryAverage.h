// HistoryAverage.h

#ifndef _HISTORY_AVERAGE_h
#define _HISTORY_AVERAGE_h

#include <stdint.h>

/// <summary>
/// Uint16 rolling average stepper and calculator, with templated history size.
/// </summary>
/// <typeparam name="HistorySize">Total sample count in rolling history [0 ; UINT8_MAX]</typeparam>
template<uint8_t HistorySize>
class HistoryAverage
{
private:
	uint16_t History[HistorySize]{};
	uint8_t Index = 0;

public:
	void Clear(const uint16_t resetValue = 0)
	{
		for (size_t i = 0; i < HistorySize; i++)
		{
			History[i] = resetValue;
		}
		Index = 0;
	}

	void Step(const uint16_t value)
	{
		History[Index] = value;
		Index++;
		if (Index >= HistorySize)
		{
			Index = 0;
		}
	}

	const uint16_t GetAverage() const
	{
		uint32_t sum = 0;
		for (size_t i = 0; i < HistorySize; i++)
		{
			sum += History[i];
		}

		return sum / HistorySize;
	}
};
#endif