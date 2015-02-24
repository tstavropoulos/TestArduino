#ifndef SERIALGENERIC_H_INCLUDED
#define SERIALGENERIC_H_INCLUDED


typedef void rawhid_t;

namespace SerialComm
{
	class SerialGeneric
	{
	protected:
		//Suppresses error messages
		bool m_bErrorSuppress;
	public:
		SerialGeneric();
		SerialGeneric(bool bErrorSuppress);

		//Close the connection
		virtual ~SerialGeneric();

		/****************************
		**                         **
		**   Function Prototypes   **
		**                         **
		****************************/

		//Initiates the connection to the previously set 
		virtual bool Connect() = 0;

		//Read data in a buffer, if nbChar is greater than the
		//maximum number of bytes available, it will return only the
		//bytes available. The function return -1 when nothing could
		//be read, the number of bytes actually read.
		virtual int ReadData(char *buffer, unsigned int nbChar) = 0;

		//Waits up to the specified max wait time to fully read the 
		//specified number of characters.  Returns indicating success.
		virtual bool WaitReadData(char *buffer, unsigned int nbChar, unsigned long long ullMaxWait) = 0;

		//Writes data from a buffer through the Serial connection
		//return true on success.
		virtual bool WriteData(const char *buffer, unsigned int nbChar) = 0;

		//Writes the whole character string.
		virtual bool WriteData(std::string sData) = 0;

		//Check if we are actually connected
		virtual bool IsConnected() = 0;

		//Flush any contents of the buffer
		virtual bool FlushBuffer() = 0;

		//Return the number of characters in the queue
		virtual int CharsInQueue() = 0;

		//Get a pretty-printing of the connection name
		virtual void GetConnectionName(std::string &sName);
	};

	struct RawHID
	{
		RawHID();
		virtual ~RawHID();
		int m_iPID;
		int m_iVID;
	};

	class SerialGenericHID : public SerialGeneric
	{
	protected:
		int m_iTargetPID;
		int m_iTargetVID;
		int m_iUsagePage;
		int m_iUsage;
	public:
		SerialGenericHID();
		SerialGenericHID(bool bErrorSuppress);

		void SetTargetHID(int iTargetVID, int iTargetPID);
		void SetTargetUsage(int iUsagePage, int iUsage);
		virtual ~SerialGenericHID();

		virtual std::vector<std::shared_ptr<RawHID>> findAllHID() = 0;
		virtual bool PickConnection(std::shared_ptr<RawHID> &upRawHID) = 0;
	};

	class SerialGenericCOM : public SerialGeneric
	{
	protected:
		//Keeps track of the port name
		std::string m_sPortName;
	public:
		SerialGenericCOM();
		SerialGenericCOM(bool bErrorSuppress);

		//Set path to COM port
		void SetPortName(const std::string sPortName);
		//Get path to COM port
		void GetPortName(std::string &sPortName);

		virtual void GetConnectionName(std::string &sName);

		virtual ~SerialGenericCOM();

	};
}


#endif