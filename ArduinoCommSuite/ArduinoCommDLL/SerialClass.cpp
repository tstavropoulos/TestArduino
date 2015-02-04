#include "SerialClass.h"
#include "CoreFunctions.h"

Serial::Serial(LPCWSTR portName) : Serial(portName, false)
{

}

Serial::Serial(LPCWSTR portName, bool bErrorSuppress) : m_bErrorSuppress(bErrorSuppress)
{
	//We're not yet connected
	this->connected = false;

	//Try to connect to the given port throuh CreateFile
	this->hSerial = CreateFile(portName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	//Check if the connection was successfull
	if (this->hSerial == INVALID_HANDLE_VALUE)
	{
		//If not success full display an Error
		if (GetLastError() == ERROR_FILE_NOT_FOUND){

			//Print Error if neccessary
			if (!m_bErrorSuppress)
			{
				printf("ERROR: Handle was not attached. Reason: %s not available.\n", portName);
			}
		}
		else if ( !m_bErrorSuppress )
		{
			printf("ERROR!!!");
		}
	}
	else
	{
		//If connected we try to set the comm parameters
		DCB dcbSerialParams = { 0 };

		//Try to get the current
		if (!GetCommState(this->hSerial, &dcbSerialParams))
		{
			//If impossible, show an error
			if (!m_bErrorSuppress)
			{
				printf("failed to get current serial parameters!");
			}
		}
		else
		{
			//Define serial connection parameters for the arduino board
			dcbSerialParams.BaudRate = CBR_115200;
			dcbSerialParams.ByteSize = 8;
			dcbSerialParams.StopBits = ONESTOPBIT;
			dcbSerialParams.Parity = NOPARITY;

			//Set the parameters and check for their proper application
			if (!SetCommState(hSerial, &dcbSerialParams))
			{
				if (!m_bErrorSuppress)
				{
					printf("ALERT: Could not set Serial Port parameters");
				}
			}
			else
			{
				//If everything went fine we're connected
				this->connected = true;
				FlushBuffer();
				//We do not need to sleep here, as our first action will be to wait
				// for communication from the Arduino.
			}
		}
	}

	wcscpy_s(m_wszPortName, portName);
}

Serial::~Serial()
{
	//Check if we are connected before trying to disconnect
	if (this->connected)
	{
		//We're no longer connected
		this->connected = false;
		//Close the serial handler
		CloseHandle(this->hSerial);
	}
}

int Serial::ReadData(char *buffer, unsigned int nbChar)
{
	//Number of bytes we'll have read
	DWORD bytesRead;
	//Number of bytes we'll really ask to read
	unsigned int toRead;

	//Use the ClearCommError function to get status info on the Serial port
	ClearCommError(this->hSerial, &this->errors, &this->status);

	//Check if there is something to read
	if (this->status.cbInQue>0)
	{
		//If there is we check if there is enough data to read the required number
		//of characters, if not we'll read only the available characters to prevent
		//locking of the application.
		if (this->status.cbInQue>nbChar)
		{
			toRead = nbChar;
		}
		else
		{
			toRead = this->status.cbInQue;
		}

		//Try to read the require number of chars, and return the number of read bytes on success
		if (ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL) && bytesRead != 0)
		{
			return bytesRead;
		}

	}

	//If nothing has been read, or that an error was detected return -1
	return -1;
}

int Serial::CharsInQueue()
{
	//Number of bytes we'll have read
	DWORD bytesRead;
	//Number of bytes we'll really ask to read
	unsigned int toRead;

	//Use the ClearCommError function to get status info on the Serial port
	ClearCommError(this->hSerial, &this->errors, &this->status);

	return this->status.cbInQue;
}

bool Serial::FlushBuffer()
{
	return (bool)PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
}

bool Serial::WaitReadData(char *buffer, unsigned int nbChar, unsigned long long ullMaxWait)
{
	//Number of bytes we'll have read
	DWORD bytesRead;
	//The target wait time.  After this, if nbChar number of characters aren't available, abort.
	unsigned long long ullTargetWaitTime = millisecondsNow() + ullMaxWait;

	while (ullTargetWaitTime > millisecondsNow())
	{
		//Use the ClearCommError function to get status info on the Serial port
		ClearCommError(this->hSerial, &this->errors, &this->status);

		//Check if there is something to read
		if (this->status.cbInQue >= nbChar)
		{
			//Try to read the require number of chars, and return status on success
			if (ReadFile(this->hSerial, buffer, nbChar, &bytesRead, NULL) && bytesRead != 0)
			{
				return true;
			}
		}
	}

	memset(&buffer[0], 0, sizeof(buffer[0])*nbChar);

	//If nothing has been read, or that an error was detected return -1
	return false;
}


bool Serial::WriteData(const char *buffer, unsigned int nbChar)
{
	DWORD bytesSend;

	//Try to write the buffer on the Serial port
	if (!WriteFile(this->hSerial, (void *)buffer, nbChar, &bytesSend, 0))
	{
		//In case it don't work get comm error and return false
		ClearCommError(this->hSerial, &this->errors, &this->status);

		return false;
	}
	else
	{
		return true;
	}
}

bool Serial::WriteData(std::string sData)
{
	DWORD bytesSend;

	//Try to write the buffer on the Serial port
	if (!WriteFile(this->hSerial, (void *)(sData.c_str()), sData.size(), &bytesSend, 0))
	{
		//In case it don't work get comm error and return false
		ClearCommError(this->hSerial, &this->errors, &this->status);

		return false;
	}
	else
	{
		return true;
	}
}


bool Serial::IsConnected()
{
	//Simply return the connection status
	return this->connected;
}

void Serial::GetPortName(std::wstring &sName)
{
	sName = (m_wszPortName);
}
