#include "CoreFunctions.h"
#include "ArduinoClass.h"
#include "SerialClass.h"

#include <iostream>
#include <thread>
#include <algorithm>

const char csInitPhrase[] = "ARDUINO";
const char csVersionPhrase[] = "C";
const char csReflexivePhrase[] = "R";
const char csPairedPhrase[] = "P";
const char csMasterPhrase[] = "M";
const char csSlavePhrase[] = "S";
const char csFinishedPhrase[] = "F";
const char csAckPhrase[] = "A";
const char csRestartPhrase[] = "~";



#define SENDPARAM(csSendPhrase,csParamName) \
	rSerial.WriteData(csSendPhrase); \
	rSerial.WaitReadData(szBuffer,1,4000); \
	if (strcmp(szBuffer,csAckPhrase) != 0 ) \
		{ \
		bOut = false; \
		if ( bPrintErrors ) \
		{ \
			std::wstring wsPortName; \
			rSerial.GetPortName(wsPortName); \
			\
			std::lock_guard<std::mutex> lock(mtxPrint); \
			std::wcout << "Unrecognized param message for " << csParamName << " with " << wsPortName << std::endl; \
			std::cout << " Sent param message: \"" << csSendPhrase << "\"" << std::endl; \
			std::cout << " Expected ack message: \"" << csAckPhrase << "\"" << std::endl; \
			std::cout << " Received ack message: \"" << szBuffer << "\"" << std::endl << std::endl; \
		} \
	} 


Arduino::Arduino() :	m_bDebug(false),
						m_bReflexive(false),
						m_bPaired(false),
						m_bMaster(true),
						m_bPrintAll(true),
						m_eState(ARDUINO_STATE::UNCONNECTED)
{
}

Arduino::Arduino(bool bDebug) : m_bDebug(bDebug),
								m_bReflexive(false),
								m_bPaired(false),
								m_bMaster(true),
								m_bPrintAll(true),
								m_eState(ARDUINO_STATE::UNCONNECTED)
{
}

Arduino::~Arduino()
{
	if (m_pSerial)
	{
		m_pSerial.release();
		m_pSerial = nullptr;
	}
}

bool Arduino::tryPort(int iPortNum)
{
	wchar_t wszPortName[26] = {};
	wsprintf(wszPortName, L"\\\\.\\COM%u", iPortNum);

	Serial testSerial(wszPortName, true);
	if (confirmVersion(testSerial, false))
	{
		if (connect(testSerial, m_bDebug))
		{
			testSerial.WriteData(csRestartPhrase);
			return true;
		}
	}

	return false;
}

std::vector<int> Arduino::tryAllPorts(int iPortMax)
{
	std::vector<int> vSuccessList;
	std::vector<std::thread> vThreads;
	std::mutex mtxVector;

	for (int i = 0; i <= iPortMax; i++)
	{
		vThreads.push_back(std::thread([&,i](){
			if (this->tryPort(i))
			{
				std::lock_guard<std::mutex> lock(mtxVector);
				vSuccessList.push_back(i);
			}
		}));
	}

	for (std::thread &thread : vThreads)
	{
		thread.join();
	}

	std::sort(std::begin(vSuccessList), std::end(vSuccessList));

	return vSuccessList;
}


bool Arduino::connectPort(int iPortNum)
{
	wchar_t wszPortName[26] = {};
	wsprintf(wszPortName, L"\\\\.\\COM%u", iPortNum);

	m_pSerial = std::make_unique<Serial>(wszPortName);

	if (!confirmVersion(*(m_pSerial), true) || 
		!connect(*(m_pSerial), true) ||
		!sendParameters(*(m_pSerial), true))
	{
		disconnect();
		return false;
	}

	return true;
}

bool Arduino::connect(Serial &rSerial, bool bPrintErrors)
{
	if (rSerial.IsConnected())
	{
		char incomingData[256] = "";

		rSerial.WaitReadData(incomingData, 7, 1000);

		if (strcmp(incomingData, csInitPhrase) == 0)
		{
			m_eState = ARDUINO_STATE::SEND_PARAMETERS;
			return true;
		}
		else if (bPrintErrors)
		{
			std::wstring wsPortName;
			rSerial.GetPortName(wsPortName);

			std::lock_guard<std::mutex> lock(mtxPrint);
			std::wcout << "Unsuccessful communication attempt with " << wsPortName << std::endl;
			std::cout << " Expected initialization message: \"" << csInitPhrase << "\"" << std::endl;
			std::cout << " Received initialization message: \"" << incomingData << "\"" << std::endl << std::endl;
		}
	}
	else if (bPrintErrors)
	{
		std::wstring wsPortName;
		rSerial.GetPortName(wsPortName);

		std::lock_guard<std::mutex> lock(mtxPrint);
		std::wcout << "Unsuccessful connection with " << wsPortName << std::endl << std::endl;
	}

	return false;
}

bool Arduino::confirmVersion(Serial &rSerial, bool bPrintErrors)
{
	if (rSerial.IsConnected())
	{
		char szBuffer[2] = "";
		rSerial.WaitReadData(szBuffer, 1, 6000);
		if (strcmp(szBuffer, csVersionPhrase) == 0)
		{
			rSerial.WriteData(csVersionPhrase);
			m_eState = ARDUINO_STATE::CONFIRM_VERSION;
			return true;
		}
		else if (bPrintErrors)
		{
			std::wstring wsPortName;
			rSerial.GetPortName(wsPortName);

			std::lock_guard<std::mutex> lock(mtxPrint);
			std::wcout << "Incompatible version with " << wsPortName << std::endl;
			std::cout << " Expected version message: \"" << csVersionPhrase << "\"" << std::endl;
			std::cout << " Received version message: \"" << szBuffer << "\"" << std::endl << std::endl;
		}
	}

	return false;
}

bool Arduino::disconnect()
{
	if (m_pSerial && m_pSerial->IsConnected())
	{
		m_pSerial->WriteData(csRestartPhrase);
		m_pSerial.release();
		m_pSerial = nullptr;
	}
	m_eState = ARDUINO_STATE::UNCONNECTED;

	return true;
}


bool Arduino::sendParameters(Serial &rSerial, bool bPrintErrors)
{
	bool bOut = true;

	char szBuffer[256] = "";

	rSerial.FlushBuffer();

	if (m_bReflexive && bOut)
	{
		SENDPARAM(csReflexivePhrase, "Reflexivity");
	}

	if (m_bPaired && bOut)
	{
		SENDPARAM(csPairedPhrase, "Paired");

		if (bOut)
		{
			if (m_bMaster)
			{
				SENDPARAM(csMasterPhrase, "Master");
			}
			else
			{
				SENDPARAM(csSlavePhrase, "Slave");
			}
		}
	}

	if (bOut)
	{
		SENDPARAM(csFinishedPhrase, "Finished");
	}

	if (bOut)
	{
		m_eState = ARDUINO_STATE::CONNECTED;
	}

	return bOut;
}

void Arduino::setReflexivity(bool bReflexive)
{
	m_bReflexive = bReflexive;
}

void Arduino::setPairedness(bool bPaired, bool bMaster)
{
	m_bPaired = bPaired;
	m_bMaster = bMaster;
}

bool Arduino::IsConnected()
{
	if (m_pSerial)
	{
		return m_pSerial->IsConnected();
	}

	return false;
}

void Arduino::WriteString(std::string sString)
{
	if (m_pSerial && m_pSerial->IsConnected() && m_eState == ARDUINO_STATE::CONNECTED)
	{
		m_pSerial->WriteData(sString);
	}
}

void Arduino::WriteChars(const char *szMessage, int iLength)
{
	if (m_pSerial && m_pSerial->IsConnected() && m_eState == ARDUINO_STATE::CONNECTED)
	{
		m_pSerial->WriteData(szMessage, iLength);
	}
}

bool Arduino::WaitReadData(char *buffer, unsigned int nbChar, unsigned long long ullMaxWait)
{
	if (m_pSerial && m_pSerial->IsConnected() && m_eState == ARDUINO_STATE::CONNECTED)
	{
		return m_pSerial->WaitReadData(buffer, nbChar, ullMaxWait);
	}

	return false;
}

int Arduino::ReadData(char *buffer, unsigned int nbChar)
{
	if (m_pSerial && m_pSerial->IsConnected() && m_eState == ARDUINO_STATE::CONNECTED)
	{
		return m_pSerial->ReadData(buffer, nbChar);
	}

	return -1;
}