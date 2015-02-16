#include "stdafx.h"

#include "SerialGeneric.h"

using namespace SerialComm;

SerialGeneric::SerialGeneric(const std::string sPortName, bool bErrorSuppress)
	: m_sPortName(sPortName), m_bErrorSuppress(bErrorSuppress)
{
}

SerialGeneric::SerialGeneric(const std::string sPortName)
	: SerialGeneric(sPortName, false)
{
}

SerialGeneric::~SerialGeneric()
{
}

void SerialGeneric::GetPortName(std::string &sName)
{
	sName = m_sPortName;
}
