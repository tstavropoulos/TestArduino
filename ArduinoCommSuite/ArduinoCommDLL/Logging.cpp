#include "stdafx.h"

#include "Logging.h"
#include <mutex>

#ifdef _WINDOWS
#endif

#ifdef _UNIX
#include <iostream>
#endif

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

	void PrintDebugTest(const std::wstring wsMessage)
	{
		std::lock_guard<std::mutex> lock(mtxPrint);
		OutputDebugString(wsMessage.c_str());
	}

	void PrintDebugError(const std::string sMessage)
	{
		std::wstring wsMessage(sMessage.begin(), sMessage.end());

		std::lock_guard<std::mutex> lock(mtxPrint);
		OutputDebugString(L"Error!");
		OutputDebugString(wsMessage.c_str());
	}

	void PrintDebugError(const std::wstring wsMessage)
	{
		std::lock_guard<std::mutex> lock(mtxPrint);
		OutputDebugString(L"Error!");
		OutputDebugString(wsMessage.c_str());
	}
};
#endif

#ifdef _UNIX
namespace SerialComm
{
	std::mutex mtxPrint;

	void PrintDebugTest(const std::string sMessage)
	{
		std::lock_guard<std::mutex> lock(mtxPrint);
		std::cout << sMessage << std::endl;
	}

	void PrintDebugTest(const std::wstring wsMessage)
	{
		std::lock_guard<std::mutex> lock(mtxPrint);
		std::wcout << wsMessage << std::endl;
	}

	void PrintDebugError(const std::string sMessage)
	{
		std::lock_guard<std::mutex> lock(mtxPrint);
		std::cout << "Error!" << std::endl << sMessage << std::endl;
	}

	void PrintDebugError(const std::wstring wsMessage)
	{
		std::lock_guard<std::mutex> lock(mtxPrint);
		std::wcout << L"Error!" << std::endl << wsMessage << std::endl;
	}
};
#endif

