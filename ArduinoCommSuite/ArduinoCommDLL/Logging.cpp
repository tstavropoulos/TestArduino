#include "stdafx.h"

#include "Logging.h"
#include <mutex>

//Support adding other builds
#ifdef _WINDOWS
namespace SerialComm
{
	std::mutex mtxPrint;

	void PrintDebugTest(const std::string sMessage)
	{
		std::wstring wsMessage(sMessage.begin(), sMessage.end());

		std::lock_guard<std::mutex> lock(mtxPrint);
		OutputDebugString(wsMessage.c_str());
	}

	void PrintDebugTest(const std::wstring sMessage)
	{
		std::lock_guard<std::mutex> lock(mtxPrint);
		OutputDebugString(sMessage.c_str());
	}

	void PrintDebugError(const std::string sMessage)
	{
		std::wstring wsMessage(sMessage.begin(), sMessage.end());

		std::lock_guard<std::mutex> lock(mtxPrint);
		OutputDebugString(L"Error!");
		OutputDebugString(wsMessage.c_str());
	}

	void PrintDebugError(const std::wstring sMessage)
	{
		std::lock_guard<std::mutex> lock(mtxPrint);
		OutputDebugString(L"Error!");
		OutputDebugString(sMessage.c_str());
	}
};
#endif

