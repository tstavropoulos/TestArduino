// ArduinoCommTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../ArduinoCommDLL/ArduinoCommDLL.h"

#include <iostream>


int _tmain(int argc, _TCHAR* argv[])
{
	SerialComm::ArduinoComm::Init();
	SerialComm::ArduinoComm::Disconnect();
	return 0;
}

