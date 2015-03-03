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
	class SerialUnix : public SerialGenericCOM
	{
	private:
		int mFileDescriptor;
		bool InitializeSerialPort();
		std::queue<char> mqBufferedChars;
	public:
		SerialUnix(bool bErrorSuppress);
		SerialUnix();

		virtual ~SerialUnix();

		virtual bool Connect();

		virtual int ReadData(char *buffer, unsigned int nbChar);

		virtual bool WaitReadData(char *buffer, unsigned int nbChar, unsigned long long ullMaxWait);
		virtual bool WriteData(const char *buffer, unsigned int nbChar);
		virtual bool WriteData(const std::string &sData);

		virtual bool IsConnected();
		virtual bool FlushBuffer();
		virtual int CharsInQueue();

		static const char *GetPortTemplate();
	};
}

#endif
#endif
