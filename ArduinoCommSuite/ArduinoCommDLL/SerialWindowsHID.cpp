#include "stdafx.h"

#ifdef _WINDOWS

#include "SerialWindowsHID.h"
#include "SerialGeneric.h"

using namespace SerialComm;

SerialWindowsHID::SerialWindowsHID(const std::string sPortName, bool bErrorSuppress)
	:SerialGenericHID(sPortName,bErrorSuppress)
{

}

SerialWindowsHID::SerialWindowsHID(const std::string sPortName)
	:SerialWindowsHID(sPortName,false)
{

}

SerialWindowsHID::~SerialWindowsHID()
{

}


int SerialWindowsHID::ReadData(char *buffer, unsigned int nbChar)
{

}

bool SerialWindowsHID::WaitReadData(char *buffer, unsigned int nbChar, unsigned long long ullMaxWait)
{

}

bool SerialWindowsHID::WriteData(const char *buffer, unsigned int nbChar)
{

}

bool SerialWindowsHID::WriteData(std::string sData)
{

}

bool SerialWindowsHID::IsConnected()
{

}

bool SerialWindowsHID::FlushBuffer()
{

}

int SerialWindowsHID::CharsInQueue()
{

}

std::vector<rawhid_t*> SerialWindowsHID::findAllHID()
{
	GUID guid;
	HDEVINFO info;
	DWORD index = 0;
	DWORD required_size;
	SP_DEVICE_INTERFACE_DATA iface;
	SP_DEVICE_INTERFACE_DETAIL_DATA *details;
	HIDD_ATTRIBUTES attrib;
	PHIDP_PREPARSED_DATA hid_data;
	struct rawhid_struct *hid;
	HANDLE h;
	bool ret;
}

#endif