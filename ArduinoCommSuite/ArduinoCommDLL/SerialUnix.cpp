#include "stdafx.h"

#ifdef _UNIX

#include "SerialGeneric.h"
#include "SerialUnix.h"
#include "CoreFunctions.h"
#include "Logging.h"

using namespace SerialComm;

#define GETATTR(sSetting) \
	if (tcgetattr(mFileDescriptor, &term_setting) == -1) \
	{ \
		if (!m_bErrorSuppress) \
		{ \
			std::wstringstream wss; \
			wss << L"Failed to get term settings for " << sSetting << L".  Port: " << m_sPortName.c_str(); \
			PrintDebugError(wss.str()); \
		} \
		  \
		return; \
	}

#define SETATTR(sSetting) \
	if (tcsetattr(mFileDescriptor, TCSANOW, &term_setting) == -1) \
	{ \
		if (!m_bErrorSuppress) \
		{ \
			std::wstringstream wss; \
			wss << L"Failed to set term settings for " << sSetting << ".  Port: " << m_sPortName.c_str(); \
			PrintDebugError(wss.str()); \
		} \
		  \
		return; \
	}


SerialUnix::SerialUnix()
	: SerialUnix(false)
{

}

SerialUnix::SerialUnix(bool bErrorSuppress)
	: SerialGenericCOM(bErrorSuppress), mFileDescriptor(-1)
{

}

SerialUnix::~SerialUnix()
{
	if ( IsConnected() )
	{
		::close(mFileDescriptor);
		mFileDescriptor = -1;
	}
}

bool SerialUnix::Connect()
{
	mFileDescriptor = ::open(sPortName.c_str(), O_RDWR | O_NOCTTY);

	if (mFileDescriptor == -1)
	{
		if (!m_bErrorSuppress)
		{
			std::wstringstream wss;
			wss << L"Failed to open Serial Port.  Port: " << sPortName.c_str();
			PrintDebugError(wss.str());

			return false;
		}
	}

	if (!InitializeSerialPort())
	{
		//Possibly need to close file descriptor here?
		return false;
	}

	return true;
}

bool SerialUnix::InitializeSerialPort()
{
	int flags = fcntl(mFileDescriptor, F_GETFL, 0);

	//Set to Non-blocking mode for configuration
	if (fcntl(mFileDescriptor, F_SETFL, flags | O_NONBLOCK ) == -1)
	{
		if ( !m_bErrorSuppress )
		{
			std::wstringstream wss;
			wss << L"Failed to set Serial Port to Non-Blocking mode.  Port: " << m_sPortName.c_str();
			PrintDebugError(wss.str());
		}

		return false;
	}

	//Flush Serial Buffer
	if (tcflush(mFileDescriptor, TCIOFLUSH) == -1)
	{
		if ( !m_bErrorSuppress )
		{
			std::wstringstream wss;
			wss << L"Failed to flush serial buffer.  Port: " << m_sPortName.c_str();
			PrintDebugError(wss.str());
		}

		return false;
	}

	//Get Term Settings
	struct termios term_setting;

	//Set Baud Rate - 115200 Baud
	GETATTR("Baud Rate");
	cfsetispeed ( &term_setting, B115200 );
	cfsetospeed ( &term_setting, B115200 );
	SETATTR("Baud Rate");

	//Set Char Size - 8 bits
	GETATTR("Char Size");
	term_setting.c_iflag &= ~ISTRIP;
	term_setting.c_cflag &= ~CSIZE;
	term_setting.c_cflag |= CS8;
	SETATTR("Char Size");

	//Set Stop Bits - One Stop Bit
	GETATTR("Stop Bit");
	term_setting.c_cflag &= ~CSTOPB;
	SETATTR("Stop Bit");

	//Set Parity - No Parity
	GETATTR("Parity");
	term_setting.c_cflag &= ~PARENB;
	SETATTR("Parity");

	//Set Flow Control - None
	GETATTR("Flow Control");
	term_setting.c_iflag &= ~(IXON|IXOFF);
	term_setting.c_cflag &= ~CRTSCTS;
	SETATTR("Flow Control");

	//Return to blocking mode
	flags = fcntl(mFileDescriptor, F_GETFL, 0);
	if (fcntl(mFileDescriptor, F_SETFL, flags & ~O_NONBLOCK ) == -1)
	{
		if ( !m_bErrorSuppress )
		{
			std::wstringstream wss;
			wss << L"Failed to set Serial Port to Blocking mode.  Port: " << m_sPortName.c_str();
			PrintDebugError(wss.str());
		}

		return false;
	}

	return true;
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
	return ( ::write(mFileDescriptor, sData.c_str(), sData.size()) > 0 );
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
			wss << L"Failed to flush serial buffer.  Port: " << m_sPortName.c_str();
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

const char *SerialUnix::GetPortTemplate()
{
	return "/dev/ttyUSB";
}


#endif
