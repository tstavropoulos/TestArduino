#ifndef ARDUINO_CLASS_H_
#define ARDUINO_CLASS_H_

#include "SerialGeneric.h"

namespace SerialComm
{
	enum ARDUINO_STATE
	{
		UNCONNECTED = 0,
		CONFIRM_VERSION,
		SEND_PARAMETERS,
		CONNECTED
	};


	class Arduino
	{
	public:
		Arduino();
		Arduino(bool bDebug);
		~Arduino();

		void setReflexivity(bool bReflexive);
		void setPairedness(bool bPaired, bool bMaster);
		void setPrintAll(bool bPrintAll);

		// Tries connecting to the specified port
		bool tryCOMPort(int iPortNum);
		// Establishes a connection to the specified COM Port
		bool connectCOMPort(int iPortNum);
		// Doesn't actually try all ports.  Just searches from COM0 to COM${iPortMax} 
		std::vector<int> findAllComPorts(int iPortMax);

		std::vector<RawHID> findAllHID();

		bool disconnect();

		bool IsConnected();

		void WriteString(std::string sString);
		void WriteChars(const char *szMessage, int iLength);

		int ReadData(char *buffer, unsigned int nbChar);
		int ReadData(std::string &sBuffer);
		char ReadChar();

		bool WaitReadData(char *buffer, unsigned int nbChar, unsigned long long ullMaxWait);
		char WaitReadChar(unsigned long long ullMaxWait);
		bool WaitReadData(std::string &sBuffer, unsigned int nbChar, unsigned long long ullMaxWait);

		int QueuedCharacters();

	protected:
		std::unique_ptr<SerialGeneric> m_pSerial;
		bool m_bDebug;
		bool m_bReflexive;
		bool m_bPaired;
		bool m_bMaster;
		bool m_bPrintAll;
		ARDUINO_STATE m_eState;

		bool connect(SerialGeneric *pSerial, bool bPrintErrors);
		bool confirmVersion(SerialGeneric *pSerial, bool bPrintErrors);
		bool sendParameters(SerialGeneric *pSerial, bool bPrintErrors);
	};
}

#endif