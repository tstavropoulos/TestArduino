#include "stdafx.h"

#ifdef _WINDOWS

#include "SerialGeneric.h"
#include "SerialWindowsHID.h"
#include "CoreFunctions.h"
#include "Logging.h"

using namespace SerialComm;

SerialWindowsHID::SerialWindowsHID(bool bErrorSuppress)
	: SerialGenericHID(bErrorSuppress), m_upDeviceThread(nullptr), m_bStopRead(false), m_bConnected(false)
{

}

SerialWindowsHID::SerialWindowsHID()
	: SerialWindowsHID(false)
{

}

SerialWindowsHID::~SerialWindowsHID()
{
	disconnect();
}

bool SerialWindowsHID::disconnect()
{
	//Gracefully clean up device thread
	if (m_upDeviceThread)
	{
		m_bStopRead = true;
		m_cvStopRead.notify_all();

		m_upDeviceThread->join();
		m_upDeviceThread.reset();
	}

	return true;
}

bool SerialWindowsHID::Connect()
{
	if (!m_spCurrentHID)
	{
		if (!m_bErrorSuppress)
		{
			PrintDebugError(L"WinHID Connect: HID not selected");
		}

		return false;
	}

	if (m_upDeviceThread)
	{
		PrintDebugError(L"WinHID Connect: Read Thread active?!?");

		return false;
	}

	m_upDeviceThread = std::make_unique<std::thread>(&SerialWindowsHID::ReadThreadMethod, this);
	m_bConnected = true;

	return true;
}

void SerialWindowsHID::ReadThreadMethod()
{
	char buffer[65];
	int trueBufferChars = sizeof(buffer) - 1;
	std::unique_lock<std::mutex> StopLock(m_mtxStopRead);
	while (!m_bStopRead && m_cvStopRead.wait_for(StopLock, std::chrono::microseconds(500)) == std::cv_status::timeout)
	{
		//Read pending data
		int readResult = DirectReadData(buffer, sizeof(buffer));
		if (readResult > 0)
		{
			//PrintDebugTest(buffer);
			int i = 0;
			std::lock_guard<std::recursive_mutex> ReadLock(m_mtxReadBuffer);
			while (buffer[i] != '\0' && i < trueBufferChars)
			{
				m_qcReadBuffer.push(buffer[i++]);
			}
		}

		if (readResult > 0)
		{
			m_cvReadBufferUpdated.notify_one();
		}

		//Write pending data
		{
			std::lock_guard<std::mutex> WriteLock(m_mtxWriteBuffer);
			while (!m_qcWriteBuffer.empty())
			{
				int i = 0;
				while (i < trueBufferChars && !m_qcWriteBuffer.empty())
				{
					buffer[i++] = m_qcWriteBuffer.front();
					m_qcWriteBuffer.pop();
				}

				while (i < sizeof(buffer))
				{
					buffer[i++] = '\0';
				}

				DirectWriteData(buffer, sizeof(buffer));
			}
		}
	}
}

int SerialWindowsHID::DirectReadData(char *cBuffer, unsigned int uiNumChar)
{
	OVERLAPPED ov;
	DWORD dwCharsRead = 0;
	DWORD dwResult = 0;
	int iCharsRead;

	if (!m_spCurrentHID)
	{
		if (!m_bErrorSuppress)
		{
			PrintDebugError(L"Cannot Read: Not connected.");
		}
		return -1;
	}

	ov.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (ov.hEvent == NULL)
	{
		if (!m_bErrorSuppress)
		{
			PrintDebugError(L"Read Failure: Create Overlapped failed.");
		}
		return -1;
	}

	//std::lock_guard<std::mutex> lock(m_mtxHIDAccess);
	if (ReadFile(m_spCurrentHID->Handle, cBuffer, uiNumChar, &dwCharsRead, &ov) == TRUE)
	{
		iCharsRead = dwCharsRead;
	}
	else
	{
		auto tmp = GetLastError();
		if (tmp == ERROR_IO_PENDING)
		{
			dwResult = WaitForSingleObject(ov.hEvent, 2000);
			if (dwResult == WAIT_OBJECT_0)
			{
				if (GetOverlappedResult(m_spCurrentHID->Handle, &ov, &dwCharsRead, FALSE))
				{
					iCharsRead = dwCharsRead;
				}
				else
				{
					if (!m_bErrorSuppress)
					{
						PrintDebugError(L"Read Failure: Read blocked then failed.");
					}

					iCharsRead = -1;
				}
			}
			else
			{
				CancelIo(m_spCurrentHID->Handle);
				if (!m_bErrorSuppress)
				{
					PrintDebugTest(L"Read Failure: Read blocked then timeout.");
				}

				iCharsRead = 0;
			}
		}
		else
		{
			if (!m_bErrorSuppress)
			{
				PrintDebugTest(L"Read Failure: Read failed.");
			}

			iCharsRead = -1;
		}
	}
	CloseHandle(ov.hEvent);
	return iCharsRead;
}

int SerialWindowsHID::DirectWriteData(char *cBuffer, unsigned int uiNumChar)
{
	OVERLAPPED ov;
	DWORD dwCharsRead = 0;
	DWORD dwResult = 0;
	bool bSuccess;
	int iCharsWritten;

	if (!m_spCurrentHID)
	{
		if (!m_bErrorSuppress)
		{
			PrintDebugError(L"Cannot Read: Not connected.");
		}
		return -1;
	}

	ov.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (ov.hEvent == NULL)
	{
		if (!m_bErrorSuppress)
		{
			PrintDebugError(L"Read Failure: Create Overlapped failed.");
		}
		return -1;
	}

	bSuccess = (WriteFile(m_spCurrentHID->Handle, cBuffer, uiNumChar, &dwCharsRead, &ov) == TRUE);
	if (bSuccess)
	{
		iCharsWritten = dwCharsRead;
	}
	else
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			if (GetOverlappedResult(m_spCurrentHID->Handle, &ov, &dwCharsRead, TRUE))
			{
				iCharsWritten = dwCharsRead;
			}
			else
			{
				if (!m_bErrorSuppress)
				{
					PrintDebugError(L"Read Failure: Read blocked then failed.");
				}

				iCharsWritten = -1;
			}
		}
		else
		{
			if (!m_bErrorSuppress)
			{
				PrintDebugTest(L"Read Failure: Read failed.");
			}

			iCharsWritten = -1;
		}
	}
	CloseHandle(ov.hEvent);
	return iCharsWritten;
}

int SerialWindowsHID::ReadData(char *cBuffer, unsigned int uiNumChar)
{
	std::lock_guard<std::recursive_mutex> ReadLock(m_mtxReadBuffer);
	unsigned int index = 0;
	while (index < uiNumChar && !m_qcReadBuffer.empty())
	{
		cBuffer[index] = m_qcReadBuffer.front();
		m_qcReadBuffer.pop();
		index++;
	}
	cBuffer[index] = '\0';

	return index;
}

bool SerialWindowsHID::WaitReadData(char *cBuffer, unsigned int uiNumChar, unsigned long long ullMaxWait)
{
	std::unique_lock<std::recursive_mutex> ReadLock(m_mtxReadBuffer);

	auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(ullMaxWait);

	do
	{
		if (m_qcReadBuffer.size() >= uiNumChar)
		{
			ReadData(cBuffer, uiNumChar);
			return true;
		}
	} while (m_cvReadBufferUpdated.wait_until(ReadLock, timeout) != std::cv_status::timeout);

	return false;
}

bool SerialWindowsHID::WriteData(const char *cBuffer, unsigned int uiNumChar)
{
	std::unique_lock<std::mutex> WriteLock(m_mtxWriteBuffer);
	for (unsigned int i = 0; i < uiNumChar; i++)
	{
		if (cBuffer[i] != '\0')
		{
			m_qcWriteBuffer.push(cBuffer[i]);
		}
	}

	return true;
}

bool SerialWindowsHID::WriteData(const std::string &sData)
{
	std::unique_lock<std::mutex> WriteLock(m_mtxWriteBuffer);
	for (unsigned int i = 0; i < sData.length(); i++)
	{
		m_qcWriteBuffer.push(sData.c_str()[i]);
	}
	return true;
}

bool SerialWindowsHID::IsConnected()
{
	return m_bConnected;
}

bool SerialWindowsHID::FlushBuffer()
{
	{
		std::unique_lock<std::mutex> WriteLock(m_mtxWriteBuffer);
		while (!m_qcWriteBuffer.empty())
		{
			m_qcWriteBuffer.pop();
		}
	}

	{
		std::unique_lock<std::recursive_mutex> ReadLock(m_mtxReadBuffer);
		while (!m_qcReadBuffer.empty())
		{
			m_qcReadBuffer.pop();
		}
	}

	return true;
}

int SerialWindowsHID::CharsInQueue()
{
	std::unique_lock<std::recursive_mutex> ReadLock(m_mtxReadBuffer);
	return m_qcReadBuffer.size();
}

std::vector<std::shared_ptr<RawHID>> SerialWindowsHID::findAllHID()
{
	GUID guid;
	HDEVINFO info;
	DWORD required_size;
	SP_DEVICE_INTERFACE_DATA iface;
	HIDD_ATTRIBUTES attrib;
	PHIDP_PREPARSED_DATA hid_data;
	HIDP_CAPS capabilities;
	HANDLE h;
	SP_DEVICE_INTERFACE_DETAIL_DATA *details;

	std::vector<std::shared_ptr<RawHID>> vFoundHIDs;

	HidD_GetHidGuid(&guid);
	info = SetupDiGetClassDevs(&guid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (info == INVALID_HANDLE_VALUE)
	{
		if (!m_bErrorSuppress)
		{
			PrintDebugError(L"Unable to initiate HID lookup.");
		}
		return vFoundHIDs;
	}

	for (int iIndex = 0;; iIndex++)
	{
		iface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		//end of list
		if (SetupDiEnumDeviceInterfaces(info, nullptr, &guid, iIndex, &iface) != TRUE)
		{
			SetupDiDestroyDeviceInfoList(info);
			return vFoundHIDs;
		}

		SetupDiGetInterfaceDeviceDetail(info, &iface, nullptr, 0, &required_size, nullptr);
		details = (SP_DEVICE_INTERFACE_DETAIL_DATA *)malloc(required_size);
		details->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		if (SetupDiGetDeviceInterfaceDetail(info, &iface, details, required_size, nullptr, nullptr) != TRUE)
		{
			free(details);
			continue;
		}

		h = CreateFile(details->DevicePath,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
		
		free(details);

		if (h == INVALID_HANDLE_VALUE)
		{
			continue;
		}

		attrib.Size = sizeof(HIDD_ATTRIBUTES);
		
		if (HidD_GetAttributes(h, &attrib) != TRUE)
		{
			CloseHandle(h);
			continue;
		}

		if (!m_bErrorSuppress)
		{
			std::wstringstream wss;
			wss << L"HID/win32: USB Device:" << std::endl;
			wss << L"HID/win32:  vid =\t" << attrib.VendorID << std::endl;
			wss << L"HID/win32:  pid =\t" << attrib.ProductID << std::endl;
			PrintDebugTest(wss.str());
		}

		if (m_iTargetVID > 0 && m_iTargetVID != (int)attrib.VendorID)
		{
			CloseHandle(h);
			continue;
		}

		if (m_iTargetPID > 0 && m_iTargetPID != (int)attrib.ProductID)
		{
			CloseHandle(h);
			continue;
		}

		if ( !HidD_GetPreparsedData(h, &hid_data) )
		{
			if (!m_bErrorSuppress)
			{
				PrintDebugError(L"HID/win32: HidD_GetPreparsedData failed");
			}

			CloseHandle(h);
			continue;
		}

		if ( !HidP_GetCaps(hid_data, &capabilities) )
		{
			if (!m_bErrorSuppress)
			{
				PrintDebugError("HID/win32: HidP_GetCaps failed");
			}

			HidD_FreePreparsedData(hid_data);
			CloseHandle(h);
			continue;
		}

		if (!m_bErrorSuppress)
		{
			std::wstringstream wss;
			wss << L"HID/win32:  usage_page =\t" << capabilities.UsagePage << std::endl;
			wss << L"HID/win32:  usage =\t" << capabilities.Usage << std::endl;
			PrintDebugTest(wss.str());
		}

		if ( m_iUsagePage > 0 && m_iUsagePage != (int)(capabilities.UsagePage) )
		{
			HidD_FreePreparsedData(hid_data);
			CloseHandle(h);
			continue;
		}

		if ( m_iUsage > 0 && m_iUsage != (int)(capabilities.Usage) )
		{
			HidD_FreePreparsedData(hid_data);
			CloseHandle(h);
			continue;
		}

		HidD_FreePreparsedData(hid_data);

		std::shared_ptr<RawWinHID> newHID = std::make_shared<RawWinHID>();

		newHID->m_iPID = (int)attrib.ProductID;
		newHID->m_iVID = (int)attrib.VendorID;
		newHID->Handle = h;

		vFoundHIDs.push_back(std::move(newHID));
	}

	return vFoundHIDs;
}

bool SerialWindowsHID::PickConnection(std::shared_ptr<RawHID> &upRawHID)
{
	//Clear current read thread, if it's active
	if (m_upDeviceThread)
	{
		std::unique_lock<std::mutex> StopLock(m_mtxStopRead);
		m_bStopRead = true;
		m_cvStopRead.notify_all();

		m_upDeviceThread->join();
		m_upDeviceThread.reset();
	}

	//Clear current connection, if it's active
	if (m_spCurrentHID)
	{
		m_spCurrentHID.reset();
	}

	m_bConnected = false;

	m_spCurrentHID = std::dynamic_pointer_cast<RawWinHID>(upRawHID);

	return (!!m_spCurrentHID);
}

void SerialWindowsHID::GetConnectionName(std::string &sName)
{
	std::stringstream ss;
	ss << "HID Device:\t";

	if (m_spCurrentHID)
	{
		ss << "VID(" << m_spCurrentHID->m_iVID << ")\tPID(" << m_spCurrentHID->m_iPID << ")";
	}
	else
	{
		ss << "None Set";
	}

	sName = ss.str();
}

RawWinHID::RawWinHID()
	:RawHID(), Handle(nullptr)
{

}

RawWinHID::~RawWinHID()
{
	if (Handle)
	{
		CloseHandle(Handle);
		Handle = nullptr;
	}
}

#endif