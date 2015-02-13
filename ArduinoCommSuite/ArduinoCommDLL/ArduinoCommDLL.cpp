// ArduinoCommDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#ifndef MATLAB

#include "ArduinoCommDLL.h"
#include "CoreFunctions.h"
#include "ArduinoClass.h"
#include "Logging.h"

const char macroString[] = "ArduinoCommDLL uninitialized: ";

#define inittest(stage,returnValue) \
	if (!m_bInitialized) \
	{\
		std::string sMacroErrorMessage;\
		sMacroErrorMessage = std::string(macroString,sizeof(macroString)+sizeof(stage)+1);\
		sMacroErrorMessage += stage; \
		PrintDebugError(sMacroErrorMessage); \
		return returnValue; \
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
		inittest("SetProperties",false);
		m_sArduino.setReflexivity(bRefl);

		if (bPaired)
		{
			m_sArduino.setPairedness(bPaired, bMaster);
		}

		return true;
	}

	bool ArduinoComm::IsConnected()
	{
		inittest("IsConnected",false);
		return m_sArduino.IsConnected();
	}

	bool ArduinoComm::Connect(int iPortNum)
	{
		inittest("Connect",false);
		return m_sArduino.connectPort(iPortNum);
	}

	void ArduinoComm::FindArduinos(int *piArduinos, int iMaxPortNum)
	{
		inittest("FindArduinos",);
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
		inittest("SendChar",false);
		bool bConnected = m_sArduino.IsConnected();
		if (bConnected)
		{
			m_sArduino.WriteChars(&ccMessage, 1);
		}

		return bConnected;
	}

	bool ArduinoComm::SendChars(const char *cszMessage, int iLength)
	{
		inittest("SendChars",false);
		bool bConnected = m_sArduino.IsConnected();
		if (bConnected)
		{
			m_sArduino.WriteChars(cszMessage, iLength);
		}

		return bConnected;
	}

	int ArduinoComm::GetCharAvailable()
	{
		inittest("GetCharAvailable",-1);
		if (m_sArduino.IsConnected())
		{
			return m_sArduino.QueuedCharacters();
		}

		return -1;
	}

	bool ArduinoComm::Disconnect()
	{
		inittest("Disconnect",false);
		return m_sArduino.disconnect();
	}

	char ArduinoComm::ReadChar()
	{
		inittest("ReadChar",'\0');
		return m_sArduino.ReadChar();
	}

	int ArduinoComm::ReadChars(char *szMessage, int iNumChars)
	{
		szMessage[0] = '\0';
		inittest("ReadChars",-1);
		return m_sArduino.ReadData(szMessage, iNumChars);
	}

	char ArduinoComm::WaitForChar(int msTimeout)
	{
		inittest("WaitForChar",'\0');
		millisecond msProperTimeOut = msTimeout;
		return m_sArduino.WaitReadChar(msProperTimeOut);
	}

	bool ArduinoComm::WaitForChars(char *szMessage, int iCharNum, int msTimeout)
	{
		szMessage[0] = '\0';
		inittest("WaitForChars",false);
		millisecond msProperTimeOut = msTimeout;
		return m_sArduino.WaitReadData(szMessage, iCharNum, msProperTimeOut);
	}


	bool ArduinoComm::SendString(const std::string csMessage)
	{
		inittest("SendString", false);
		bool bConnected = m_sArduino.IsConnected();
		if (bConnected)
		{
			m_sArduino.WriteString(csMessage);
		}

		return bConnected;
	}

	int ArduinoComm::ReadString(std::string &sMessage)
	{
		sMessage = "";
		inittest("ReadString",-1);
		return m_sArduino.ReadData(sMessage);
	}

	bool ArduinoComm::WaitForString(std::string &sMessage, int iCharNum, int msTimeout)
	{
		sMessage = "";
		inittest("WaitForString",false);
		millisecond msProperTimeOut = msTimeout;
		return m_sArduino.WaitReadData(sMessage, iCharNum, msProperTimeOut);
	}
}

#endif
