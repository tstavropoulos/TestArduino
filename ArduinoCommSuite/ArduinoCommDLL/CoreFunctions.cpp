#include "stdafx.h"

#include "CoreFunctions.h"

namespace SerialComm
{
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
