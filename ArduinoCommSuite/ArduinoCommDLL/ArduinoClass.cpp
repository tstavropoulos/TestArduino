#include "stdafx.h"

#include "CoreFunctions.h"
#include "ArduinoClass.h"
#include "SerialGeneric.h"
#include "SerialWindows.h"
#include "SerialUnix.h"
#include "SerialWindowsHID.h"
#include "Logging.h"

using namespace SerialComm;

#ifdef _WINDOWS
typedef SerialWindows SerialCOM;
typedef SerialWindowsHID SerialHID;
#endif

#ifdef _UNIX
typedef SerialUnix SerialCOM;
typedef SerialUnixHID SerialHID;
#endif



namespace SerialComm
{
	const char csInitPhrase[] = "ARDUINO";
	const char csVersionPhrase[] = "C";
	const char csReflexivePhrase[] = "R";
	const char csPairedPhrase[] = "P";
	const char csMasterPhrase[] = "M";
	const char csSlavePhrase[] = "S";
	const char csFinishedPhrase[] = "F";
	const char csAckPhrase[] = "A";
	const char csRestartPhrase[] = "~";
}

#define SENDPARAM(csSendPhrase,csParamName) \
	pSerial->WriteData(csSendPhrase); \
	pSerial->WaitReadData(szBuffer,1,4000); \
	if (strcmp(szBuffer,csAckPhrase) != 0 ) \
			{ \
		bOut = false; \
		if ( bPrintErrors ) \
				{ \
			std::string sPortName; \
			pSerial->GetConnectionName(sPortName); \
			\
			std::wstringstream wss; \
			wss << L"Unrecognized param message for " << csParamName << L" with " << sPortName.c_str() << std::endl; \
			wss << L" Sent param message: \"" << csSendPhrase << L"\"" << std::endl; \
			wss << L" Expected ack message: \"" << csAckPhrase << L"\"" << std::endl; \
			wss << L" Received ack message: \"" << szBuffer << L"\""; \
			PrintDebugError(wss.str()); \
		} \
	}


Arduino::Arduino()
	:	m_bDebug(false),
		m_bReflexive(false),
		m_bPaired(false),
		m_bMaster(true),
		m_bPrintAll(true),
		m_eState(ARDUINO_STATE::UNCONNECTED)
{
}

Arduino::Arduino(bool bDebug)
	:	m_bDebug(bDebug),
		m_bReflexive(false),
		m_bPaired(false),
		m_bMaster(true),
		m_bPrintAll(true),
		m_eState(ARDUINO_STATE::UNCONNECTED)
{
}

Arduino::~Arduino()
{
	disconnect();
}

bool Arduino::tryCOMPort(int iPortNum)
{
	std::stringstream ss;
	ss << "\\\\.\\COM" << iPortNum;

	SerialCOM testSerial(true);
	testSerial.SetPortName(ss.str());
	testSerial.Connect();
	if (confirmVersion(&testSerial, false) &&
		readSignature(&testSerial, m_bDebug))
	{
		return true;
	}

	return false;
}

std::vector<int> Arduino::findAllComPorts(int iPortMax)
{
	std::vector<int> vSuccessList;
	std::vector<std::thread> vThreads;
	std::mutex mtxVector;

	for (int i = 0; i <= iPortMax; i++)
	{
		vThreads.push_back(std::thread([&,i](){
			if (this->tryCOMPort(i))
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


bool Arduino::findAllHID()
{
	std::unique_ptr<SerialHID> upSerialHID = std::make_unique<SerialHID>(false);
	upSerialHID->SetTargetUsage(0xFFAB, 0x0200);

	m_vAllHIDs = upSerialHID->findAllHID();

	return !m_vAllHIDs.empty();
}

bool Arduino::connectHID(int iHIDnum)
{

	std::unique_ptr<SerialHID> pTmp;
#ifdef _WINDOWS
	pTmp = std::make_unique<SerialHID>(true);
#else
	pTmp = std::unique_ptr<SerialHID>(new SerialHID(true));
#endif // _WINDOWS

	pTmp->PickConnection(m_vAllHIDs[iHIDnum]);

	m_pSerial = std::unique_ptr<SerialGeneric>(std::move(pTmp));

	return genericConnect();
}

bool Arduino::genericConnect()
{
	if (!m_pSerial->Connect() ||
		!confirmVersion(m_pSerial.get(), true) ||
		!readSignature(m_pSerial.get(), true) ||
		!sendParameters(m_pSerial.get(), true))
	{
		disconnect();
		return false;
	}
	return true;
}

bool Arduino::connectCOMPort(int iPortNum)
{

	std::unique_ptr<SerialCOM> pTmp;
#ifdef _WINDOWS
	pTmp = std::make_unique<SerialCOM>();
#else
	pTmp = std::unique_ptr<SerialCOM>(new SerialCOM());
#endif // _WINDOWS

	std::stringstream ss;
	ss << SerialCOM::GetPortTemplate() << iPortNum;
	pTmp->SetPortName(ss.str());

	if (m_pSerial)
	{
		m_pSerial.reset();
	}

	m_pSerial = std::unique_ptr<SerialGeneric>(std::move(pTmp));

	return genericConnect();
}

bool Arduino::readSignature(SerialGeneric *pSerial, bool bPrintErrors)
{
	if (pSerial->IsConnected())
	{
		char incomingData[256] = "";

		pSerial->WaitReadData(incomingData, 7, 1000);

		if (strcmp(incomingData, csInitPhrase) == 0)
		{
			m_eState = ARDUINO_STATE::SEND_PARAMETERS;
			return true;
		}
		else if (bPrintErrors)
		{
			std::string sPortName;
			pSerial->GetConnectionName(sPortName);

			std::wstringstream wss;
			wss << L"Unsuccessful communication attempt with " << sPortName.c_str() << std::endl;
			wss << L" Expected initialization message: \"" << csInitPhrase << L"\"" << std::endl;
			wss << L" Received initialization message: \"" << incomingData << L"\"" << std::endl << std::endl;
			PrintDebugError(wss.str());
		}
	}
	else if (bPrintErrors)
	{
		std::string sPortName;
		pSerial->GetConnectionName(sPortName);

		std::wstringstream wss;
		wss << L"Unsuccessful connection with " << sPortName.c_str() << std::endl << std::endl;
		PrintDebugError(wss.str());
	}

	return false;
}

bool Arduino::confirmVersion(SerialGeneric *pSerial, bool bPrintErrors)
{
	if (pSerial->IsConnected())
	{
		char szBuffer[2] = "";
		pSerial->WaitReadData(szBuffer, 1, 6000);
		if (strcmp(szBuffer, csVersionPhrase) == 0)
		{
			pSerial->WriteData(csVersionPhrase);
			m_eState = ARDUINO_STATE::CONFIRM_VERSION;
			return true;
		}
		else if (bPrintErrors)
		{
			std::string sPortName;
			pSerial->GetConnectionName(sPortName);

			std::wstringstream wss;
			wss << L"Incompatible version with " << sPortName.c_str() << std::endl;
			wss << L" Expected version message: \"" << csVersionPhrase << L"\"" << std::endl;
			wss << L" Received version message: \"" << szBuffer << L"\"" << std::endl << std::endl;
		}
	}

	return false;
}


bool Arduino::disconnect()
{
	if (m_pSerial)
	{
		m_pSerial->disconnect();
		m_pSerial.reset();
	}
	m_eState = ARDUINO_STATE::UNCONNECTED;

	return true;
}


bool Arduino::sendParameters(SerialGeneric *pSerial, bool bPrintErrors)
{
	bool bOut = true;

	char szBuffer[256] = "";

	pSerial->FlushBuffer();

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

void Arduino::WriteString(const std::string &sString)
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

	buffer[0] = '\0';
	return false;
}

char Arduino::WaitReadChar(unsigned long long ullMaxWait)
{
	if (m_pSerial && m_pSerial->IsConnected() && m_eState == ARDUINO_STATE::CONNECTED)
	{
		char buffer[2];
		m_pSerial->WaitReadData(buffer, 1, ullMaxWait);
		return buffer[0];
	}

	return '\0';
}

bool Arduino::WaitReadData(std::string &sBuffer, unsigned int nbChar, unsigned long long ullMaxWait)
{
	if (m_pSerial && m_pSerial->IsConnected() && m_eState == ARDUINO_STATE::CONNECTED)
	{
		char buffer[1024];
		bool bStatus = m_pSerial->WaitReadData(buffer, 1024, ullMaxWait);
		sBuffer = buffer;
		return bStatus;
	}

	sBuffer = "";
	return false;
}

int Arduino::ReadData(char *buffer, unsigned int nbChar)
{
	if (m_pSerial && m_pSerial->IsConnected() && m_eState == ARDUINO_STATE::CONNECTED)
	{
		return m_pSerial->ReadData(buffer, nbChar);
	}

	buffer[0] = '\0';
	return -1;
}

int Arduino::ReadData(std::string &sBuffer)
{
	if (m_pSerial && m_pSerial->IsConnected() && m_eState == ARDUINO_STATE::CONNECTED)
	{
		char buffer[1024];
		int iRead = m_pSerial->ReadData(buffer, 1024);
		sBuffer = buffer;
		return iRead;
	}

	sBuffer = "";
	return -1;
}

char Arduino::ReadChar()
{
	if (m_pSerial && m_pSerial->IsConnected() && m_eState == ARDUINO_STATE::CONNECTED)
	{
		char buffer[2];
		m_pSerial->ReadData(buffer, 1);
		return buffer[0];
	}

	return '\0';
}

int Arduino::QueuedCharacters()
{
	if (m_pSerial && m_pSerial->IsConnected() && m_eState == ARDUINO_STATE::CONNECTED)
	{
		return m_pSerial->CharsInQueue();
	}

	return -1;
}
