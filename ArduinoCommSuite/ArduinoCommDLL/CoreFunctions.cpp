
#include "stdafx.h"
#include "CoreFunctions.h"
#include <iostream>

#ifndef MATLAB
namespace SerialComm
{
#endif
	std::mutex mtxPrint;

	millisecond millisecondsNow() {
		static LARGE_INTEGER s_frequency;
		static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
		if (s_use_qpc) {
			LARGE_INTEGER now;
			QueryPerformanceCounter(&now);
			return (1000LL * now.QuadPart) / s_frequency.QuadPart;
		}
		else {
			return GetTickCount();
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
			return GetTickCount();
		}
	}
#ifndef MATLAB
}
#endif