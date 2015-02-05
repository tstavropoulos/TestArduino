// ArduinoCommDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ArduinoCommDLL.h"

#define inittest() \
	if (!m_bInitialized) \
	{\
		throw "ArduinoCommDLL used before initialization"; \
	}

namespace SerialComm
{
	Arduino ArduinoComm::m_sArduino;
	bool ArduinoComm::m_bInitialized = false;
	
	bool ArduinoComm::Init()
	{
		m_bInitialized = true;
		return m_bInitialized;
	}

	bool ArduinoComm::Connect(int iPortNum)
	{
		inittest();
		return m_sArduino.connectPort(iPortNum);
	}

	bool ArduinoComm::SendChar(const char ccMessage)
	{
		inittest();
		bool bConnected = m_sArduino.IsConnected();
		if (bConnected)
		{
			m_sArduino.WriteChars(&ccMessage, 1);
		}

		return bConnected;
	}

	bool ArduinoComm::SendString(const char *cszMessage, int iLength)
	{
		inittest();
		bool bConnected = m_sArduino.IsConnected();
		if (bConnected)
		{
			m_sArduino.WriteChars(cszMessage, iLength);
		}

		return bConnected;
	}

	bool ArduinoComm::SendString(const std::string csMessage)
	{
		inittest();
		bool bConnected = m_sArduino.IsConnected();
		if (bConnected)
		{
			m_sArduino.WriteString(csMessage);
		}

		return bConnected;
	}

	int ArduinoComm::GetCharAvailable()
	{
		inittest();
		if (m_sArduino.IsConnected())
		{
			return m_sArduino.QueuedCharacters();
		}

		return -1;
	}

	bool ArduinoComm::Disconnect()
	{
		inittest();
		return m_sArduino.disconnect();
	}

}
