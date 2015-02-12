#ifdef _UNIX
#ifndef SERIALUNIX_H_INCLUDED
#define SERIALUNIX_H_INCLUDED

namespace SerialComm
{
	//Class meant to handle Serial communication in Unix
	class SerialUnix : public SerialGeneric
	{
	private:
		//Private Stuff
	public:
		SerialUnix(const std::wstring wsPortName, bool bErrorSuppress);
		SerialUnix(const std::wstring wsPortName);
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