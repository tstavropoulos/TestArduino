// ArduinoCommSpeedTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#ifdef _WINDOWS
#include "../ArduinoCommDLL/ArduinoCommDLL.h"
#endif // _WINDOWS

#ifdef _UNIX
#include "../ArduinoCommDLL/ArduinoCommSO.h"
#define _tmain main
#define _TCHAR char
#endif // _UNIX

#include "../ArduinoCommDLL/CoreFunctions.h"


#define HIDTEST
#define RUNNUM 100

const char cPingChar = 'G';
std::vector<SerialComm::microsecond> vRunTimes;

void analyzeOutput(std::vector<SerialComm::microsecond> &vRunTimes);

#ifdef _WINDOWS
BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
	case CTRL_C_EVENT:
	case CTRL_SHUTDOWN_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
		SerialComm::ArduinoComm::Disconnect();
		return FALSE;
		break;
	default:
		break;
	}
	return FALSE;
}
#endif

int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _WINDOWS
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
#endif

	SerialComm::ArduinoComm::Init();

#ifndef HIDTEST
	int piArduinos[10] = {};
	SerialComm::ArduinoComm::FindArduinos(piArduinos, 10);
	if (piArduinos[0] != 0)
	{
		std::cout << "Arduinos found on ports: ";
	}
	else
	{
		std::cout << "No Arduinos found." << std::endl << "Quitting" << std::endl;
		std::string sInput;
		std::cin >> sInput;
		return 0;
	}

	for (int i : piArduinos)
	{
		if (i)
		{
			std::cout << i << "  ";
		}
	}

	int iPort;
	std::cout << std::endl << "Connect to Port: ";
	std::cin >> iPort;

#endif

	SerialComm::ArduinoComm::SetProperties(true);

#ifdef HIDTEST
	if (!SerialComm::ArduinoComm::ConnectToFirstHID())
#else
	if (!SerialComm::ArduinoComm::Connect(iPort))
#endif
	{
#ifdef HIDTEST
		std::cout << "Failure to connect to HID." << std::endl << "Quitting" << std::endl;
#else
		std::cout << "Failure to connect to Port " << iPort << "." << std::endl << "Quitting" << std::endl;
#endif
		std::string sInput;
		std::cin >> sInput;
		return 0;
	}

	char readResult = '\0';


	for (int run = 0; run < RUNNUM; run++)
	{
		bool bFinished = false;
		SerialComm::microsecond start = SerialComm::microsecondsNow();

		SerialComm::ArduinoComm::SendChar(cPingChar);
		while (!bFinished)
		{
			readResult = SerialComm::ArduinoComm::ReadChar();
			if (readResult != '\0')
			{
				if (readResult == cPingChar)
				{
					bFinished = true;
				}
				else
				{
					std::cout << readResult << std::endl;
				}
			}
		}

		SerialComm::microsecond elapsed = SerialComm::microsecondsNow() - start;

		if (SerialComm::ArduinoComm::GetCharAvailable())
		{
			while (SerialComm::ArduinoComm::GetCharAvailable())
			{
				std::cout << SerialComm::ArduinoComm::ReadChar();
			}
			std::cout << std::endl;
		}

		vRunTimes.push_back(elapsed);

		std::stringstream ss;
		ss << "Run Number: " << run << std::endl;
		std::cout << ss.str();
	}

	SerialComm::ArduinoComm::Disconnect();

	analyzeOutput(vRunTimes);

	std::string input;
	std::cin >> input;

	return 0;
}



void analyzeOutput(std::vector<SerialComm::microsecond> &vRunTimes)
{
	double dTimeSum = 0.0;
	double dTimeSqSum = 0.0;
	long iNum = 0;

	SerialComm::microsecond usMax = 0;

	for (SerialComm::microsecond time : vRunTimes)
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

	std::cout << "Trial of " << iNum << " messages finised." << std::endl;
	std::cout << "  Average turnaround time: " << dAverage << "us" << std::endl;
	std::cout << "  Turnaround Standard Deviation: " << dStandardDeviation << "us" << std::endl;
	std::cout << "  Longest Latency: " << usMax << "us" << std::endl;
}