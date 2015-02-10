// ArduinoCommCDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#ifndef NOTMATLAB

#include "ArduinoCommCDLL.h"
#include "CoreFunctions.h"
#include "ArduinoClass.h"

#define TMPDEBUG

#define inittest() \
	if (!m_bInitialized) \
	{\
		OutputDebugString(L"ArduinoCommDLL used before initialization"); \
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
	inittest();
	m_sArduino.setReflexivity(bRefl);

	return true;
}

bool SetProperties(bool bRefl, bool bPaired, bool bMaster)
{
	inittest();
	m_sArduino.setReflexivity(bRefl);

	if (bPaired)
	{
		m_sArduino.setPairedness(bPaired, bMaster);
	}

	return true;
}

bool IsConnected()
{
	inittest();
	return m_sArduino.IsConnected();
}

bool Connect(int iPortNum)
{
	inittest();
	return m_sArduino.connectPort(iPortNum);
}

void FindArduinos(int *piArduinos, int iMaxPortNum)
{
	inittest();
	std::vector<int> vValidPorts = m_sArduino.tryAllPorts(iMaxPortNum);
	int index = 0;
	for (int i : vValidPorts)
	{
		piArduinos[index++] = i;
#ifdef TMPDEBUG
		OutputDebugString(L"Found a Valid Arduino!");
#endif
	}

	return;
}

bool SendChar(const char ccMessage)
{
	inittest();
	bool bConnected = m_sArduino.IsConnected();
	if (bConnected)
	{
		m_sArduino.WriteChars(&ccMessage, 1);
	}

	return bConnected;
}

bool SendChars(const char *cszMessage, int iLength)
{
	inittest();
	bool bConnected = m_sArduino.IsConnected();
	if (bConnected)
	{
		m_sArduino.WriteChars(cszMessage, iLength);
	}

	return bConnected;
}

int GetCharAvailable()
{
	inittest();
	if (m_sArduino.IsConnected())
	{
		return m_sArduino.QueuedCharacters();
	}

	return -1;
}

bool Disconnect()
{
	inittest();
	return m_sArduino.disconnect();
}

char ReadChar()
{
	inittest();
	return m_sArduino.ReadChar();
}

int ReadChars(char *szMessage, int iNumChars)
{
	inittest();
	return m_sArduino.ReadData(szMessage, iNumChars);
}

char WaitForChar(int msTimeout)
{
	inittest();
	SerialComm::millisecond msProperTimeOut = msTimeout;
	return m_sArduino.WaitReadChar(msProperTimeOut);
}

bool WaitForChars(char *szMessage, int iCharNum, int msTimeout)
{
	inittest();
	SerialComm::millisecond msProperTimeOut = msTimeout;
	return m_sArduino.WaitReadData(szMessage, iCharNum, msProperTimeOut);
}

#endif