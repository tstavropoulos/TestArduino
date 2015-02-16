/*************************************
**  Serial Unix
**
**  A good chunk of this implementation was initially plagarized off of the
**  excellent libserial software, available here:
**
**  http://libserial.sourceforge.net/
**
********************************/

#ifdef _UNIX
#ifndef SERIALUNIX_H_INCLUDED
#define SERIALUNIX_H_INCLUDED

namespace SerialComm
{
	//Class meant to handle Serial communication in Unix
	class SerialUnix : public SerialGeneric
	{
	private:
		int mFileDescriptor;
		void InitializeSerialPort();
		std::queue<char> mqBufferedChars;
	public:
		SerialUnix(const std::string sPortName, bool bErrorSuppress);
		SerialUnix(const std::string sPortName);
		virtual ~SerialUnix();

		virtual int ReadData(char *buffer, unsigned int nbChar);

		virtual bool WaitReadData(char *buffer, unsigned int nbChar, unsigned long long ullMaxWait);
		virtual bool WriteData(const char *buffer, unsigned int nbChar);
		virtual bool WriteData(std::string sData);

		virtual bool IsConnected();
		virtual bool FlushBuffer();
		virtual int CharsInQueue();
	};
}

#endif
#endif
