#include "stdafx.h"

#ifdef _UNIX

//Stuff for headers
//End stuff for headers

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
	mFileDescriptor = -1;

	mFileDescriptor = ::open(wsPortName.str(), O_RDWR | O_NOCTTY );

	if ( mFileDescriptor == -1 )
	{
		if ( !m_bErrorSuppress )
		{
			std::wstringstream wss;
			wss << L"Failed to open Serial Port.  Port: " << wsPortName;
			PrintDebugError(wss.str());
		}
	}
	else
	{
		InitializeSerialPort();
	}
}

SerialUnix::~SerialUnix()
{
	if ( IsConnected() )
	{
		::close(mFileDescriptor);
		mFileDescriptor = -1;
	}
}

void SerialUnix::InitializeSerialPort()
{
	int flags = fcntl(mFileDescriptor, F_GETFL, 0);

	//Set to Non-blocking mode for configuration
	if (fcntl(mFileDescriptor, F_SETFL, flags | O_NONBLOCK ) == -1)
	{
		if ( !m_bErrorSuppress )
		{
			std::wstringstream wss;
			wss << L"Failed to set Serial Port to Non-Blocking mode.  Port: " << m_wsPortName;
			PrintDebugError(wss.str());
		}

		return;
	}

	//Flush Serial Buffer
	if (tcflush(mFileDescriptor, TCIOFLUSH) == -1)
	{
		if ( !m_bErrorSuppress )
		{
			std::wstringstream wss;
			wss << L"Failed to flush serial buffer.  Port: " << m_wsPortName;
			PrintDebugError(wss.str());
		}

		return;
	}

	//Get Term Settings
	struct termios term_setting;
	if (tcgetattr(mFileDescriptor, &term_setting) == -1)
	{
		if ( !m_bErrorSuppress )
		{
			std::wstringstream wss;
			wss << L"Failed to get term settings.  Port: " << m_wsPortName;
			PrintDebugError(wss.str());
		}

		return;
	}

	//Set Baud Rate - 115200 Baud
	cfsetispeed ( &term_setting, B115200 );
	cfsetospeed ( &term_setting, B115200 );

	//Set Char Size - 8 bits
	term_setting.c_iflag &= ~ISTRIP;
	term_setting.c_cflag &= ~CSIZE;
	term_setting.c_cflag |= CS8;

	//Set Stop Bits - One Stop Bit
	term_setting.c_cflag &= ~CSTOPB ;

	//Set Parity - No Parity
	term_setting.c_cflag &= ~PARENB ;

	//Set Flow Control - None
	term_setting.c_iflag &= ~(IXON|IXOFF);
	term_setting.c_cflag &= ~CRTSCTS;


	if (tcsetattr(mFileDescriptor, TCSANOW, &term_setting) == -1)
	{
		if ( !m_bErrorSuppress )
		{
			std::wstringstream wss;
			wss << L"Failed to set term settings.  Port: " << m_wsPortName;
			PrintDebugError(wss.str());
		}

		return;
	}

	//Return to blocking mode
	flags = fcntl(mFileDescriptor, F_GETFL, 0);
	if (fcntl(mFileDescriptor, F_SETFL, flags & ~O_NONBLOCK ) == -1)
	{
		if ( !m_bErrorSuppress )
		{
			std::wstringstream wss;
			wss << L"Failed to set Serial Port to Blocking mode.  Port: " << m_wsPortName;
			PrintDebugError(wss.str());
		}

		return;
	}

	return;
}

int SerialUnix::ReadData(char *buffer, unsigned int nbChar)
{
	int iCharsWritten = 0;
	while ((mqBufferedChars.size() > 0) && (iCharsWritten < nbChar))
	{
		buffer[iCharsWritten++] = mqBufferedChars.front();
		mqBufferedChars.pop();
	}

	if (iCharsWritten < nbChar)
	{
		iCharsWritten += ::read(mFileDescriptor, &buffer + iCharsWritten, nbChar - iCharsWritten);
	}

	return iCharsWritten;
}

bool SerialUnix::WaitReadData(char *buffer, unsigned int nbChar, unsigned long long ullMaxWait)
{

	unsigned long long ullTargetWaitTime = millisecondsNow() + ullMaxWait;

	while (ullTargetWaitTime > millisecondsNow())
	{
		char tmpBuffer[100];
		int iCharsRead = ReadData(tmpBuffer, 100);
		for (int iChar = 0; iChar < iCharsRead; iChar++)
		{
			mqBufferedChars.push(tmpBuffer[iChar]);
		}

		if (mqBufferedChars.size() >= nbChar)
		{
			int iCharsWritten = 0;
			buffer[iCharsWritten++] = mqBufferedChars.front();
			mqBufferedChars.pop();

			return true;
		}
	}

	memset(&buffer[0], 0, sizeof(buffer[0])*nbChar);

	//If nothing has been read, or that an error was detected return -1
	return false;
}

bool SerialUnix::WriteData(const char *buffer, unsigned int nbChar)
{
	return ( ::write(mFileDescriptor, buffer, nbChar) > 0 );
}

bool SerialUnix::WriteData(std::string sData)
{
	return ( ::write(mFileDescriptor, sData.c_str(), DWORD(sData.size())) > 0 );
}

bool SerialUnix::IsConnected()
{
	return ( mFileDescriptor != -1 );
}

bool SerialUnix::FlushBuffer()
{
	while (mqBufferedChars.size() > 0)
	{
		mqBufferedChars.pop();
	}

	if (tcflush(mFileDescriptor, TCIOFLUSH) == -1)
	{
		if ( !m_bErrorSuppress )
		{
			std::wstringstream wss;
			wss << L"Failed to flush serial buffer.  Port: " << m_wsPortName;
			PrintDebugError(wss.str());
		}

		return false;
	}

	return true;
}

int SerialUnix::CharsInQueue()
{
	char tmpBuffer[100];
	int iCharsRead = ReadData(tmpBuffer, 100);
	while (iCharsRead > 0)
	{
		for (int iChar = 0; iChar < iCharsRead; iChar++)
		{
			mqBufferedChars.push(tmpBuffer[iChar]);
		}
		iCharsRead = ReadData(tmpBuffer, 100);
	}

	return mqBufferedChars.size();
}


#endif
