// TestArduino.cpp : Defines the entry point for the console application.
//


#include "CoreFunctions.h"
#include "ArduinoClass.h"
#include <tchar.h>
#include <string>
#include <iostream>
#include <thread>
#include <vector>

#define RUNNUM 10000

const char cPingChar[] = "G";

std::vector<microsecond> vRunTimes;

int _tmain(int argc, _TCHAR* argv[])
{
	printf("Welcome to the serial test app!\n\n");

	char incomingData[256] = "";			// don't forget to pre-allocate memory
	int readResult = 0;
	memset(&incomingData[0], 0, sizeof(incomingData));
	bool bPrintedOther = false;
	bool bAbort = false;

	Arduino arduino;

	arduino.setReflexivity(true);

	std::vector<int> vPorts = arduino.tryAllPorts(10);

	if (vPorts.size() == 0)
	{
		std::cout << "No Arduinos discovered between COM0 and COM10." << std::endl;
		std::cout << "Quitting." << std::endl;

		int i;
		std::cin >> i;

		return 1;
	}

	std::cout << "Arduino Search Results" << std::endl;
	std::cout << "Arduinos found on ports: ";
	for (int port : vPorts)
	{
		std::cout << port << " ";
	}
	std::cout << std::endl << std::endl;

	int portNum;
	std::cout << "Enter port number for Arduino: ";
	std::cin >> portNum;
	std::cout << std::endl;

	bool bConnected = arduino.connectPort(portNum);

	if (bConnected)
	{
		for (int run = 0; ((run < RUNNUM) && (arduino.IsConnected())); run++)
		{
			memset(&incomingData[0], 0, sizeof(incomingData));
			bool bFinished = false;
			microsecond start = microsecondsNow();

			arduino.WriteChars(cPingChar, 1);
			while (!bFinished)
			{
				readResult = arduino.ReadData(incomingData, sizeof(incomingData)/sizeof(char));
				if (readResult != -1)
				{
					if (strchr(incomingData, cPingChar[0]))
					{
						bFinished = true;
					}
					else
					{
						bPrintedOther = true;
						std::cout << incomingData << std::endl;
						memset(&incomingData[0], 0, sizeof(incomingData));
					}
				}
			}

			microsecond elapsed = microsecondsNow() - start;

			if (readResult != 1)
			{
				for (unsigned int i = 0; i < strlen(incomingData); i++)
				{
					if (incomingData[i] != cPingChar[0])
					{
						std::cout << incomingData[i];
					}
				}

				if (strlen(incomingData) > 1)
				{
					std::cout << std::endl;
				}
			}

			vRunTimes.push_back(elapsed);

			/*
			char buffer[256] = "";
			memset(&buffer[0], 0, sizeof(buffer));


			sprintf_s(buffer, "%llums", elapsed);

			if (bPrintedOther)
			{
				bPrintedOther = false;
				printf("\n");
			}
			int decplace = strlen(buffer) - 5;
			for (unsigned int i = 0; i < strlen(buffer); i++)
			{
				if (i == decplace)
				{
					printf(".");
				}
				printf("%c", buffer[i]);
			}
			printf("\n");
			*/
		}
	}

	if (bConnected)
	{
		analyzeOutput(vRunTimes);
	}

	std::string input;
	std::cin >> input;

	return 0;
}

