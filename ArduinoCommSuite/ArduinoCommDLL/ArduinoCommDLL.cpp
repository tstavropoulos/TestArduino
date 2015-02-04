// ArduinoCommDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ArduinoCommDLL.h"
#include <stdexcept>

#define inittest() \
	if (!m_bInitialized) \
	{\
		throw "ArduinoCommDLL used before initialization"; \
	}

namespace SerialComm
{
	bool ArduinoComm::m_bInitialized = false;
	
	bool ArduinoComm::Init()
	{
		m_bInitialized = true;
	}

	bool ArduinoComm::Connect(int iPortNum)
	{
		inittest();
		
	}

	bool ArduinoComm::SendChar(const char ccMessage)
	{
		inittest();

	}

	bool ArduinoComm::SendString(const char *cszMessage)
	{
		inittest();

	}

	bool ArduinoComm::SendString(const std::string csMessage)
	{
		inittest();
	}

	bool ArduinoComm::Disconnect()
	{
		inittest();

	}

}
