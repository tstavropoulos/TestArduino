#include "stdafx.h"

#include "SerialGeneric.h"

using namespace SerialComm;

SerialGeneric::SerialGeneric(const std::string sPortName)
	: SerialGeneric(sPortName, false)
{
}

SerialGeneric::SerialGeneric(const std::string sPortName, bool bErrorSuppress)
	: m_sPortName(sPortName), m_bErrorSuppress(bErrorSuppress)
{
}

SerialGeneric::~SerialGeneric()
{
}

void SerialGeneric::GetPortName(std::string &sName)
{
	sName = m_sPortName;
}

SerialGenericCOM::SerialGenericCOM(const std::string sPortName, bool bErrorSuppress)
	: SerialGeneric(sPortName, bErrorSuppress)
{
}

SerialGenericCOM::SerialGenericCOM(const std::string sPortName)
	: SerialGenericCOM(sPortName, false)
{
}

SerialGenericCOM::~SerialGenericCOM()
{
}

SerialGenericHID::SerialGenericHID(const std::string sPortName, bool bErrorSuppress)
	: SerialGeneric(sPortName, bErrorSuppress)
{
}

SerialGenericHID::SerialGenericHID(const std::string sPortName)
	: SerialGenericHID(sPortName, false)
{
}

SerialGenericHID::~SerialGenericHID()
{
}