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


#endif