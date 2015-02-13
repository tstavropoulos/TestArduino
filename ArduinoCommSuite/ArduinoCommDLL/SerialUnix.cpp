#include "stdafx.h"

#ifdef _UNIX

#include "SerialGeneric.h"
#include "SerialUnix.h"
#include "CoreFunctions.h"
#include "Logging.h"

using namespace SerialComm;

SerialUnix::SerialUnix(const std::wstring wsPortName)
	: SerialUnix(wsPortName, false)
{
	
}

SerialUnix::SerialUnix(const std::wstring wsPortName, bool bErrorSuppress)
	: SerialGeneric(wsPortName, bErrorSuppress)
{

}

SerialUnix::~SerialUnix()
{

}

int SerialUnix::ReadData(char *buffer, unsigned int nbChar)
{

}

bool SerialUnix::WaitReadData(char *buffer, unsigned int nbChar, unsigned long long ullMaxWait)
{

}

bool SerialUnix::WriteData(const char *buffer, unsigned int nbChar)
{

}

bool SerialUnix::WriteData(std::string sData)
{

}

bool SerialUnix::IsConnected()
{

}

bool SerialUnix::FlushBuffer()
{

}

int SerialUnix::CharsInQueue()
{

}


#endif