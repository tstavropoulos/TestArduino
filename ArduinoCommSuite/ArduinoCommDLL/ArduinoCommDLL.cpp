// ArduinoCommDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ArduinoCommDLL.h"
#include "CoreFunctions.h"

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

	bool ArduinoComm::SetProperties(bool bRefl, bool bPaired, bool bMaster)
	{
		inittest();
		m_sArduino.setReflexivity(bRefl);

		if (bPaired)
		{
			m_sArduino.setPairedness(bPaired, bMaster);
		}

		return true;
	}

	bool ArduinoComm::Connect(int iPortNum)
	{
		inittest();
		return m_sArduino.connectPort(iPortNum);
	}

	void ArduinoComm::FindArduinos(int *piArduinos, int iMaxPortNum)
	{
		inittest();
		std::vector<int> vValidPorts = m_sArduino.tryAllPorts(iMaxPortNum);
		int index = 0;
		for (int i : vValidPorts)
		{
			piArduinos[index++] = i;
		}

		return;
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

	bool ArduinoComm::SendChars(const char *cszMessage, int iLength)
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

	char ArduinoComm::ReadChar()
	{
		inittest();
		return m_sArduino.ReadChar();
	}

	void ArduinoComm::ReadChars(char *szMessage, int iNumChars)
	{
		inittest();
		m_sArduino.ReadData(szMessage, iNumChars);
	}

	void ArduinoComm::ReadString(std::string &sMessage)
	{
		inittest();
		m_sArduino.ReadData(sMessage);
	}

	char ArduinoComm::WaitForChar(int msTimeout)
	{
		inittest();
		millisecond msProperTimeOut = msTimeout;
		return m_sArduino.WaitReadChar(msProperTimeOut);
	}

	int ArduinoComm::WaitForChars(char *szMessage, int iCharNum, int msTimeout)
	{
		inittest();
		millisecond msProperTimeOut = msTimeout;
		return m_sArduino.WaitReadData(szMessage, iCharNum, msProperTimeOut);
	}

	int ArduinoComm::WaitForString(std::string &sMessage, int iCharNum, int msTimeout)
	{
		inittest();
		millisecond msProperTimeOut = msTimeout;
		return m_sArduino.WaitReadData(sMessage, iCharNum, msProperTimeOut);
	}

}
