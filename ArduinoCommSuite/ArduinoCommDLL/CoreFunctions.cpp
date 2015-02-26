#include "stdafx.h"

#include "CoreFunctions.h"

#define MAXTIMERS 100

namespace SerialComm
{
	microsecond microTimers[MAXTIMERS] = { 0 };
	millisecond milliTimers[MAXTIMERS] = { 0 };


	void microTimerStart(unsigned int index)
	{
		if (index < MAXTIMERS)
		{
			microTimers[index] = microsecondsNow();
		}
	}

	microsecond microTimerTotal(unsigned int index)
	{
		if (index < MAXTIMERS)
		{
			return microsecondsNow() - microTimers[index];
		}

		return 0;
	}

	void microTimerClear(unsigned int index)
	{
		if (index < MAXTIMERS)
		{
			microTimers[index] = 0;
		}
	}

	void milliTimerStart(unsigned int index)
	{
		if (index < MAXTIMERS)
		{
			milliTimers[index] = millisecondsNow();
		}
	}

	millisecond milliTimerTotal(unsigned int index)
	{
		if (index < MAXTIMERS)
		{
			return millisecondsNow() - milliTimers[index];
		}

		return 0;
	}

	void milliTimerClear(unsigned int index)
	{
		if (index < MAXTIMERS)
		{
			milliTimers[index] = 0;
		}
	}

#ifdef _WINDOWS
	millisecond millisecondsNow() {
		static LARGE_INTEGER s_frequency;
		static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
		if (s_use_qpc) {
			LARGE_INTEGER now;
			QueryPerformanceCounter(&now);
			return (1000LL * now.QuadPart) / s_frequency.QuadPart;
		}
		else {
			return GetTickCount64();
		}
	}

	microsecond microsecondsNow() {
		static LARGE_INTEGER s_frequency;
		static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
		if (s_use_qpc) {
			LARGE_INTEGER now;
			QueryPerformanceCounter(&now);
			return (1000000LL * now.QuadPart) / s_frequency.QuadPart;
		}
		else {
			return GetTickCount64();
		}
	}

#endif // _WINDOWS

#ifdef _UNIX
	millisecond millisecondsNow() {
        return millisecond( (microsecondsNow() + 500)/1000 );
	}

	microsecond microsecondsNow() {
        timeval tv;
        gettimeofday(&tv,NULL);
        return (1000000*tv.tv_sec + tv.tv_usec);
	}
#endif
}
