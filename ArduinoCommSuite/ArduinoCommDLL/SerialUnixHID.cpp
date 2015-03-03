#include "stdafx.h"

#ifdef _UNIX

#include "SerialGeneric.h"
#include "SerialUnixHID.h"
#include "CoreFunctions.h"
#include "Logging.h"

using namespace SerialComm;

SerialUnixHID::SerialUnixHID(bool bErrorSuppress)
	: SerialGenericHID(bErrorSuppress)
{

}

SerialUnixHID::SerialUnixHID()
	: SerialUnixHID(false)
{

}

SerialUnixHID::~SerialUnixHID()
{
	disconnect();
}

bool SerialUnixHID::disconnect()
{

	return true;
}

bool SerialUnixHID::Connect()
{
	return false;
}


int SerialUnixHID::ReadData(char *cBuffer, unsigned int uiNumChar)
{
	cBuffer[0] = '\0';

	return 0;
}

bool SerialUnixHID::WaitReadData(char *cBuffer, unsigned int uiNumChar, unsigned long long ullMaxWait)
{
	cBuffer[0] = '\0';
	return false;
}

bool SerialUnixHID::WriteData(const char *cBuffer, unsigned int uiNumChar)
{
	return false;
}

bool SerialUnixHID::WriteData(const std::string &sData)
{
	return false;
}

bool SerialUnixHID::IsConnected()
{
	return false;
}

bool SerialUnixHID::FlushBuffer()
{
	return true;
}

int SerialUnixHID::CharsInQueue()
{
	return -1;
}

std::vector<std::shared_ptr<RawHID>> SerialUnixHID::findAllHID()
{
	std::vector<std::shared_ptr<RawHID>> vFoundHIDs;
	
	return vFoundHIDs;
}

bool SerialUnixHID::PickConnection(std::shared_ptr<RawHID> &upRawHID)
{

	//Clear current connection, if it's active
	if (m_spCurrentHID)
	{
		m_spCurrentHID.reset();
	}

	m_spCurrentHID = std::dynamic_pointer_cast<RawUnixHID>(upRawHID);

	return (!!m_spCurrentHID);
}

void SerialUnixHID::GetConnectionName(std::string &sName)
{
	std::stringstream ss;
	ss << "HID Device:\t";

	if (m_spCurrentHID)
	{
		ss << "VID(" << m_spCurrentHID->m_iVID << ")\tPID(" << m_spCurrentHID->m_iPID << ")";
	}
	else
	{
		ss << "None Set";
	}

	sName = ss.str();
}

RawUnixHID::RawUnixHID()
	:RawHID()
{

}

RawUnixHID::~RawUnixHID()
{

}

#endif