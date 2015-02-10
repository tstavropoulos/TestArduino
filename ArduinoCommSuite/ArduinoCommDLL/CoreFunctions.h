#ifndef COREFUNCTIONS_H_
#define COREFUNCTIONS_H_

#include <cstdint>
#include <mutex>
#include <vector>

namespace SerialComm
{
	typedef unsigned long long millisecond;
	typedef unsigned long long microsecond;

	millisecond millisecondsNow();
	microsecond microsecondsNow();

	extern std::mutex mtxPrint;
}
#endif