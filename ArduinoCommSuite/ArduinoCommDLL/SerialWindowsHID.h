/*************************************
**  Serial Windows HID
**
**  A good chunk of this implementation was initially plagarized off of the
**  Teensy HID listener code located here:
**
**  http://www.pjrc.com/teensy/hid_listen.html
**
********************************/

#ifdef _WINDOWS
#ifndef SERIALWINDOWSHID_H_INCLUDED
#define SERIALWINDOWSHID_H_INCLUDED

namespace SerialComm
{
	//Class meant to handle Serial communication in Windows
	class SerialWindowsHID : public SerialGenericHID
	{
	private:

	public:
		SerialWindowsHID(const std::string sPortName, bool bErrorSuppress);
		SerialWindowsHID(const std::string sPortName);
		virtual ~SerialWindowsHID();

		virtual int ReadData(char *buffer, unsigned int nbChar);

		virtual bool WaitReadData(char *buffer, unsigned int nbChar, unsigned long long ullMaxWait);
		virtual bool WriteData(const char *buffer, unsigned int nbChar);
		virtual bool WriteData(std::string sData);

		virtual bool IsConnected();
		virtual bool FlushBuffer();
		virtual int CharsInQueue();

		virtual std::vector<rawhid_t*> findAllHID();
	};
}

#endif
#endif
