#ifndef COREFUNCTIONS_H_
#define COREFUNCTIONS_H_

namespace SerialComm
{
	typedef unsigned long long millisecond;
	typedef unsigned long long microsecond;

	millisecond millisecondsNow();
	microsecond microsecondsNow();
}
#endif