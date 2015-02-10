// ArduinoCommDLL.h : Defines the exported functions for the DLL application.
//
#ifndef ARDUINOCOMMDLL_H_
#define ARDUINOCOMMDLL_H_

#ifndef MATLAB

#ifdef ARDUINOCOMMDLL_EXPORTS
#define ARDUINOCOMMDLL_API __declspec(dllexport)
#else
#define ARDUINOCOMMDLL_API __declspec(dllimport)
#endif


#include <string>

namespace SerialComm
{
	class Arduino;

	class ArduinoComm
	{
	public:
		/*  Init() initializes the ArduinoCommDLL.  Returns success.
		*/
		static ARDUINOCOMMDLL_API bool Init();
		/*  SetProperties(Relfectivity,Pairedness,Masterness) is used to set the properties the Arduino will initialize with.
		If Reflexive, it will copy Serial messages back to their source.
		If Paired, it will communicate to a second Arduino using I2C
		If Master (and paired) it will be the Master in the I2C connection.
		If Not Master, it will be the Slave in the I2C connection.
		*/
		static ARDUINOCOMMDLL_API bool SetProperties(bool bRefl, bool bPaired = false, bool bMaster = false);


		/*  IsConnected() returns whether the DLL is currently connected to an Arduino.
		*/
		static ARDUINOCOMMDLL_API bool IsConnected();
		/*  Connect(PortNum) connects to the indicated Com Port number.  
		Also responsible for communicating properties and initiating Arduino loop.
		Returns success.
		*/
		static ARDUINOCOMMDLL_API bool Connect(int iPortNum);
		/*  FindArduinos(Arduinos[], MaxPortNum) writes an array of discovered Arduino ports to the supplied buffer.
		Buffer must be at least MaxPortNum in length.
		Searches from Com 1 up to the Com Port MaxPortNum.
		*/
		static ARDUINOCOMMDLL_API void FindArduinos(int *piArduinos, int iMaxPortNum);


		/*  SendChar(char) sends a single character to the currently connected Arduino.
		Returns success.
		*/
		static ARDUINOCOMMDLL_API bool SendChar(const char ccMessage);
		/*  SendChars(chars[],length) sends a full character string to the Arduino, of indicated length.
		Returns success.
		*/
		static ARDUINOCOMMDLL_API bool SendChars(const char *cszMessage, int iLength);


		/*  GetCharAvailable() returns how many characters are currently in the Queue to be read.
		Returns -1 if not connected.
		*/
		static ARDUINOCOMMDLL_API int GetCharAvailable();


		/*  ReadChar() returns the next character in the buffer if one is available.
		Returns a null character if the buffer is empty.
		Returns a null character if not connected.
		*/
		static ARDUINOCOMMDLL_API char ReadChar();
		/*  ReadChars(Message[],Length) writes buffered messages to the supplied character buffer, up to indicated length.
		Returns the number of characters read.
		Writes a null character to Message[0] if the buffer is empty or the Arduino is not connected.
		Returns -1 if not connected.
		*/
		static ARDUINOCOMMDLL_API int ReadChars(char *szMessage, int iNumChars);


		/*	WaitForChar(timeout) waits up to the indicated number of milliseconds for a character to arrive in the buffer.
		Returns as soon as message arrives, or immediately if the buffer is not empty.
		Returns a null character if the wait times out.
		Returns a null character if not connected.
		*/
		static ARDUINOCOMMDLL_API char WaitForChar(int msTimeout);
		/*	WaitForChar(Message[],Length,timeout) waits up to the indicated number of milliseconds for the indicated number of character to be in the buffer.
		Returns as soon as sufficient characters are buffered, or immediately if the condition is met.
		If sufficient characters arrive, they are written to the supplied buffer.
		Writes a null character to the buffer if the wait times out or Arduino is not connected.
		Returns success.
		*/
		static ARDUINOCOMMDLL_API bool WaitForChars(char *szMessage, int iCharNum, int msTimeout);

		/*  SendString(string) sends the full std::string to the Arduino.
		Returns success.
		*/
		static ARDUINOCOMMDLL_API bool SendString(const std::string csMessage);
		/*  ReadChars(StringMessage) writes buffered messages to the supplied std::string.
		Returns the number of characters read.
		Writes an empty string if the buffer is empty or the Arduino is not connected.
		Returns -1 if not connected.
		*/
		static ARDUINOCOMMDLL_API int ReadString(std::string &sMessage);
		/*	WaitForChar(StringMessage,Length,timeout) waits up to the indicated number of milliseconds for the indicated number of character to be in the buffer.
		Returns as soon as sufficient characters are buffered, or immediately if the condition is met.
		If sufficient characters arrive, they are written to the string.
		Writes an empty string if the wait times out or Arduino is not connected.
		Returns success.
		*/
		static ARDUINOCOMMDLL_API bool WaitForString(std::string &sMessage, int iCharNum, int msTimeout);


		/*  Disconnect() disconnects from the Arduino and destroys the serial information.
		*/
		static ARDUINOCOMMDLL_API bool Disconnect();
	protected:
		/*  m_bInitialized tracks whether or not the DLL has been initialized.
		*/
		static bool m_bInitialized;

		/*  Our static reference to the Arduino object.
		*/
		static Arduino m_sArduino;
	};

}

#endif

#endif
