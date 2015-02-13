#ifndef SERIALGENERIC_H_INCLUDED
#define SERIALGENERIC_H_INCLUDED


namespace SerialComm
{
	class SerialGeneric
	{
	protected:
		//Keeps track of the port name
		std::wstring m_wsPortName;

		//Suppresses error messages
		bool m_bErrorSuppress;

	public:
		//Initialize Serial communication with the given COM port
		SerialGeneric(const std::wstring wsPortName, bool bErrorSuppress);

		//Initialize Serial communication with the given COM port
		SerialGeneric(const std::wstring wsPortName);

		//Close the connection
		virtual ~SerialGeneric();

		//Find out the port name
		void GetPortName(std::wstring &wsName);

		/****************************
		**                         **
		**   Function Prototypes   **
		**                         **
		****************************/


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
	};
}


#endif