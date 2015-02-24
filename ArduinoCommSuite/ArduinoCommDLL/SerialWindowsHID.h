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
		std::shared_ptr<RawWinHID> m_upCurrentHID;
	public:
		SerialWindowsHID(bool bErrorSuppress);
		SerialWindowsHID();
		virtual ~SerialWindowsHID();

		virtual int ReadData(char *buffer, unsigned int nbChar);

		virtual bool WaitReadData(char *buffer, unsigned int nbChar, unsigned long long ullMaxWait);
		virtual bool WriteData(const char *buffer, unsigned int nbChar);
		virtual bool WriteData(std::string sData);

		virtual bool IsConnected();
		virtual bool FlushBuffer();
		virtual int CharsInQueue();

		virtual std::vector<std::shared_ptr<RawHID>> findAllHID();
		virtual bool PickConnection(std::shared_ptr<RawHID> &upRawHID);
	};

	struct RawWinHID : public RawHID
	{
		RawWinHID();
		virtual ~RawWinHID();
		HANDLE Handle;
	};
}

#endif
#endif
