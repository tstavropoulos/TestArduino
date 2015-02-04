#include "CoreFunctions.h"
#include <windows.h>
#include <math.h>
#include <iostream>

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


void analyzeOutput(std::vector<microsecond> &vRunTimes)
{
	double dTimeSum = 0.0;
	double dTimeSqSum = 0.0;
	long iNum = 0;

	microsecond usMax = 0;

	for (microsecond time : vRunTimes)
	{
		dTimeSum += time;
		dTimeSqSum += (time * time);
		iNum++;

		if (time > usMax)
		{
			usMax = time;
		}
	}

	double dAverage = dTimeSum / iNum;

	double dStandardDeviation = sqrt((dTimeSqSum / iNum) - dAverage*dAverage);

	{
		std::lock_guard<std::mutex> lock(mtxPrint);

		std::cout << "Trial of " << iNum << " messages finised." << std::endl;
		std::cout << "  Average turnaround time: " << dAverage << "us" << std::endl;
		std::cout << "  Turnaround Standard Deviation: " << dStandardDeviation << "us" << std::endl;
		std::cout << "  Longest Latency: " << usMax << "us" << std::endl;
	}

}