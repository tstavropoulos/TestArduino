/*************************************
**  Serial Unix HID
**
**  A good chunk of this implementation will be initially plagarized off of the
**  Teensy HID listener code located here:
**
**  http://www.pjrc.com/teensy/hid_listen.html
**
********************************/

#ifdef _UNIX
#ifndef SERIALUNIXHID_H_INCLUDED
#define SERIALUNIXHID_H_INCLUDED

namespace SerialComm
{
	struct RawUnixHID : public RawHID
	{
		RawUnixHID();
		virtual ~RawUnixHID();
	};

	//Class meant to handle Serial communication in Windows
	class SerialUnixHID : public SerialGenericHID
	{
	protected:
		//Pointer to the currently selected HID
		std::shared_ptr<RawUnixHID> m_spCurrentHID;

	public:
		SerialUnixHID(bool bErrorSuppress);
		SerialUnixHID();
		virtual ~SerialUnixHID();

		virtual bool Connect();

		virtual int ReadData(char *buffer, unsigned int nbChar);

		virtual bool WaitReadData(char *buffer, unsigned int nbChar, unsigned long long ullMaxWait);
		virtual bool WriteData(const char *buffer, unsigned int nbChar);
		virtual bool WriteData(const std::string &sData);

		virtual bool IsConnected();
		virtual bool FlushBuffer();
		virtual int CharsInQueue();

		virtual std::vector<std::shared_ptr<RawHID>> findAllHID();
		virtual bool PickConnection(std::shared_ptr<RawHID> &upRawHID);

		virtual void GetConnectionName(std::string &sName);

		virtual bool disconnect();
	};
}

#endif
#endif
