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
	struct RawWinHID : public RawHID
	{
		RawWinHID();
		virtual ~RawWinHID();
		HANDLE Handle;
	};

	//Class meant to handle Serial communication in Windows
	class SerialWindowsHID : public SerialGenericHID
	{
	protected:
		//Pointer to the currently selected HID
		std::shared_ptr<RawWinHID> m_spCurrentHID;

		//Boolean holding current connection status
		bool m_bConnected;

		//
		//  Threaded reading varaibles
		//

		//Unique Pointer to the thread responsible for reading input
		std::unique_ptr<std::thread> m_upDeviceThread;

		//Buffer holding characters that have been recieved from device
		std::queue<char> m_qcReadBuffer;

		//Condition Variable to Notify threads waiting on communicaiton
		std::condition_variable_any m_cvReadBufferUpdated;

		//Mutex to make accessing Read Buffer threadsafe
		std::recursive_mutex m_mtxReadBuffer;

		//Buffer holding characters that are waiting to be sent to device
		std::queue<char> m_qcWriteBuffer;

		std::vector<char> m_vcWriteFailBuffer;

		//Mutex to make accessing Write Buffer threadsafe
		std::mutex m_mtxWriteBuffer;

		//Condition Variable to Notify Read Thread of intention to end connection
		std::condition_variable m_cvStopRead;

		//Mutex to lock for the Condition Variable
		std::mutex m_mtxStopRead;

		//Boolean to capture intent to stop reading
		bool m_bStopRead;

		int DirectWriteData(char *cBuffer, unsigned int uiNumChar);
		int DirectReadData(char *cBuffer, unsigned int uiNumChar);
		void ReadThreadMethod();

	public:
		SerialWindowsHID(bool bErrorSuppress);
		SerialWindowsHID();
		virtual ~SerialWindowsHID();

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
