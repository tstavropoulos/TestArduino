#ifndef LOGGING_H_
#define LOGGING_H_

namespace SerialComm
{
	void PrintDebugTest(const std::wstring sMessage);
	void PrintDebugError(const std::wstring sMessage);
	void PrintDebugTest(const std::string sMessage);
	void PrintDebugError(const std::string sMessage);
};

#endif