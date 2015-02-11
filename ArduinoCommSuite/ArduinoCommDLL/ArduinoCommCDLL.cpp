// ArduinoCommCDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#ifndef NOTMATLAB

#include "ArduinoCommCDLL.h"
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
		SerialComm::PrintDebugError(sMacroErrorMessage); \
		return returnValue; \
	}

static SerialComm::Arduino m_sArduino;
static bool m_bInitialized = false;

bool Init()
{
	m_bInitialized = true;
	return m_bInitialized;
}

bool SetRefl(bool bRefl)
{
	inittest("SetRefl",false);
	m_sArduino.setReflexivity(bRefl);

	return true;
}

bool SetProperties(bool bRefl, bool bPaired, bool bMaster)
{
	inittest("SetProperties",false);
	m_sArduino.setReflexivity(bRefl);

	if (bPaired)
	{
		m_sArduino.setPairedness(bPaired, bMaster);
	}

	return true;
}

bool IsConnected()
{
	inittest("IsConnected",false);
	return m_sArduino.IsConnected();
}

bool Connect(int iPortNum)
{
	inittest("Connect",false);
	return m_sArduino.connectPort(iPortNum);
}

void FindArduinos(int *piArduinos, int iMaxPortNum)
{
	inittest("FindArduinos",);
	std::vector<int> vValidPorts = m_sArduino.tryAllPorts(iMaxPortNum);
	int index = 0;
	for (int i : vValidPorts)
	{
		piArduinos[index++] = i;

		std::stringstream ss;
		ss << "Found a Valid Arduino on port: " << i;
		SerialComm::PrintDebugTest(ss.str());
	}

	return;
}

bool SendChar(const char ccMessage)
{
	inittest("SendChar",false);
	bool bConnected = m_sArduino.IsConnected();
	if (bConnected)
	{
		m_sArduino.WriteChars(&ccMessage, 1);
	}

	return bConnected;
}

bool SendChars(const char *cszMessage, int iLength)
{
	inittest("SendChars",false);
	bool bConnected = m_sArduino.IsConnected();
	if (bConnected)
	{
		m_sArduino.WriteChars(cszMessage, iLength);
	}

	return bConnected;
}

int GetCharAvailable()
{
	inittest("GetCharAvailable",-1);
	if (m_sArduino.IsConnected())
	{
		return m_sArduino.QueuedCharacters();
	}

	return -1;
}

bool Disconnect()
{
	inittest("Disconnect",false);
	return m_sArduino.disconnect();
}

char ReadChar()
{
	inittest("ReadChar",'\0');
	return m_sArduino.ReadChar();
}

int ReadChars(char *szMessage, int iNumChars)
{
	inittest("ReadChars",-1);
	return m_sArduino.ReadData(szMessage, iNumChars);
}

char WaitForChar(int msTimeout)
{
	inittest("WaitForChar",'\0');
	SerialComm::millisecond msProperTimeOut = msTimeout;
	return m_sArduino.WaitReadChar(msProperTimeOut);
}

bool WaitForChars(char *szMessage, int iCharNum, int msTimeout)
{
	inittest("WaitForChars",false);
	SerialComm::millisecond msProperTimeOut = msTimeout;
	return m_sArduino.WaitReadData(szMessage, iCharNum, msProperTimeOut);
}

#endif