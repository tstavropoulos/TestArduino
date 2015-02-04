// ArduinoCommDLL.h : Defines the exported functions for the DLL application.
//
#ifndef ARDUINOCOMMDLL_H_
#define ARDUINOCOMMDLL_H_

#ifdef ARDUINOCOMMDLL_EXPORTS
#define ARDUINOCOMMDLL_API __declspec(dllexport)
#else
#define ARDUINOCOMMDLL_API __declspec(dllimport)
#endif

#include <string>

namespace SerialComm
{
	class ArduinoComm
	{
	public:
		static ARDUINOCOMMDLL_API bool Init();

		static ARDUINOCOMMDLL_API bool Connect(int iPortNum);
		static ARDUINOCOMMDLL_API bool SendChar(const char ccMessage);
		static ARDUINOCOMMDLL_API bool SendString(const char *cszMessage);
		static ARDUINOCOMMDLL_API bool SendString(const std::string csMessage);

		static ARDUINOCOMMDLL_API bool Disconnect();
	protected:
		static bool m_bInitialized;
		static 
	};

}

#endif