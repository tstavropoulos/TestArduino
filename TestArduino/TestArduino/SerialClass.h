#ifndef SERIALCLASS_H_INCLUDED
#define SERIALCLASS_H_INCLUDED

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

class Serial
{
private:
	//Serial comm handler
	HANDLE hSerial;
	//Connection status
	bool connected;
	//Get various information about the connection
	COMSTAT status;
	//Keep track of last error
	DWORD errors;
	//Keeps track of the port name
	wchar_t m_wszPortName[256];
	bool m_bErrorSuppress;

public:
	//Initialize Serial communication with the given COM port
	Serial(LPCWSTR portName, bool bErrorSuppress);
	//Initialize Serial communication with the given COM port
	Serial(LPCWSTR portName);
	//Close the connection
	~Serial();
	//Read data in a buffer, if nbChar is greater than the
	//maximum number of bytes available, it will return only the
	//bytes available. The function return -1 when nothing could
	//be read, the number of bytes actually read.
	int ReadData(char *buffer, unsigned int nbChar);
	//Waits up to the specified max wait time to fully read the 
	//specified number of characters.  Returns indicating success.
	bool WaitReadData(char *buffer, unsigned int nbChar, unsigned long long ullMaxWait);
	//Writes data from a buffer through the Serial connection
	//return true on success.
	bool WriteData(const char *buffer, unsigned int nbChar);
	//Writes the whole character string.
	bool WriteData(std::string sData);
	//Check if we are actually connected
	bool IsConnected();
	//Flush any contents of the buffer
	bool FlushBuffer();
	//Find out the port name
	void GetPortName(std::wstring &sName);
};

#endif // SERIALCLASS_H_INCLUDED