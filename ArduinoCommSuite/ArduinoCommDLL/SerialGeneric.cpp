#include "stdafx.h"

#include "SerialGeneric.h"

using namespace SerialComm;

SerialGeneric::SerialGeneric()
	: SerialGeneric(false)
{
}

SerialGeneric::SerialGeneric(bool bErrorSuppress)
	: m_bErrorSuppress(bErrorSuppress)
{
}

SerialGeneric::~SerialGeneric()
{
}

SerialGenericCOM::SerialGenericCOM(bool bErrorSuppress)
	: SerialGeneric(bErrorSuppress), m_sPortName("")
{
}

SerialGenericCOM::SerialGenericCOM()
	: SerialGenericCOM(false)
{
}

SerialGenericCOM::~SerialGenericCOM()
{
}

void SerialGenericCOM::GetConnectionName(std::string &sName)
{
	GetPortName(sName);
}

void SerialGenericCOM::GetPortName(std::string &sName)
{
	sName = m_sPortName;
}

void SerialGenericCOM::SetPortName(const std::string sName)
{
	m_sPortName = sName;
}


SerialGenericHID::SerialGenericHID(bool bErrorSuppress)
	: SerialGeneric(bErrorSuppress), m_iTargetPID(0), m_iTargetVID(0), m_iUsagePage(0), m_iUsage(0)
{
}

SerialGenericHID::SerialGenericHID()
	: SerialGenericHID(false)
{
}

SerialGenericHID::~SerialGenericHID()
{
}

void SerialGenericHID::SetTargetHID(int iTargetVID, int iTargetPID)
{
	m_iTargetVID = iTargetVID;
	m_iTargetPID = iTargetPID;
}

void SerialGenericHID::SetTargetUsage(int iUsagePage, int iUsage)
{
	m_iUsagePage = iUsagePage;
	m_iUsage = iUsage;
}

RawHID::RawHID()
	: m_iPID(0), m_iVID(0)
{

}

RawHID::~RawHID()
{

}
