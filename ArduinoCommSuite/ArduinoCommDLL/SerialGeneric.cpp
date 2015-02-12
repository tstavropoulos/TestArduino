#include "stdafx.h"

#include "SerialGeneric.h"

using namespace SerialComm;

SerialGeneric::SerialGeneric(const std::wstring wsPortName, bool bErrorSuppress)
	: m_wsPortName(wsPortName), m_bErrorSuppress(bErrorSuppress)
{
}

SerialGeneric::SerialGeneric(const std::wstring wsPortName)
	: SerialGeneric(wsPortName, false)
{
}

SerialGeneric::~SerialGeneric()
{
}

void SerialGeneric::GetPortName(std::wstring &wsName)
{
	wsName = m_wsPortName;
}
