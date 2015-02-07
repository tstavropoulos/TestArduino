#ifndef COREFUNCTIONS_H_
#define COREFUNCTIONS_H_

#include <windows.h>
#include <cstdint>
#include <mutex>
#include <vector>

#ifndef MATLAB
namespace SerialComm
{
#endif
	typedef uint64_t millisecond;
	typedef uint64_t microsecond;

	millisecond millisecondsNow();
	microsecond microsecondsNow();

	extern std::mutex mtxPrint;
#ifndef MATLAB
}
#endif
#endif