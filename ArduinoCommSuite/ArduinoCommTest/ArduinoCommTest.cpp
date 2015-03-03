// ArduinoCommTest.cpp : Defines the entry point for the console application.
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

	SerialComm::millisecond msReadyTime = SerialComm::millisecondsNow() + 1000;

	std::cout << std::endl;

	if (SerialComm::ArduinoComm::GetCharAvailable())
	{
		while (SerialComm::millisecondsNow() < msReadyTime)
		{
			while (SerialComm::ArduinoComm::GetCharAvailable())
			{
				std::cout << SerialComm::ArduinoComm::ReadChar();
			}
		}
	}
	std::cout << std::endl;

	SerialComm::ArduinoComm::SendChar('Q');

	char cReflChar = SerialComm::ArduinoComm::WaitForChar(5000);

	std::cout << "Received Char: " << cReflChar << std::endl;
	std::cout << "Results: " << ((cReflChar == 'Q') ? "Success!" : "Failure!") << std::endl;
	std::cout << "More Chars: ";
	std::cout << std::endl;

	SerialComm::ArduinoComm::Disconnect();

	std::string sInput;
	std::cin >> sInput;
	return 0;
}

