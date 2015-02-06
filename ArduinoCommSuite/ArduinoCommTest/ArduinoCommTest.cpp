// ArduinoCommTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../ArduinoCommDLL/ArduinoCommDLL.h"

#include <iostream>
#include <string>

int _tmain(int argc, _TCHAR* argv[])
{
	SerialComm::ArduinoComm::Init();
	/*
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
	*/
	int iPort;
	std::cout << std::endl << "Connect to Port: ";
	std::cin >> iPort;

	SerialComm::ArduinoComm::SetProperties(true);

	if (!SerialComm::ArduinoComm::Connect(iPort))
	{
		std::cout << "Failure to connect to Port " << iPort << "." << std::endl << "Quitting" << std::endl;
		std::string sInput;
		std::cin >> sInput;
		return 0;
	}

	SerialComm::ArduinoComm::SendChar('A');

	char cReflChar = SerialComm::ArduinoComm::WaitForChar(5000);

	std::cout << "Received Char: " << cReflChar << std::endl;
	std::cout << "Results: " << ((cReflChar == 'A') ? "Success!" : "Failure!") << std::endl;

	SerialComm::ArduinoComm::Disconnect();
	return 0;
}

