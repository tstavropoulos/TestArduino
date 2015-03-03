#ifndef COREFUNCTIONS_H_
#define COREFUNCTIONS_H_

namespace SerialComm
{
	typedef unsigned long long millisecond;
	typedef unsigned long long microsecond;

	millisecond millisecondsNow();
	microsecond microsecondsNow();

	void microTimerStart(unsigned int index);
	microsecond microTimerTotal(unsigned int index);
	void microTimerClear(unsigned int index);

	void milliTimerStart(unsigned int index);
	millisecond milliTimerTotal(unsigned int index);
	void milliTimerClear(unsigned int index);
}
#endif