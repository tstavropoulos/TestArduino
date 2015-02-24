#include "stdafx.h"

#ifdef _WINDOWS

#include "SerialWindowsHID.h"
#include "SerialGeneric.h"
#include "Logging.h"

using namespace SerialComm;

SerialWindowsHID::SerialWindowsHID(bool bErrorSuppress)
	:SerialGenericHID(bErrorSuppress)
{

}

SerialWindowsHID::SerialWindowsHID()
	:SerialWindowsHID(false)
{

}

SerialWindowsHID::~SerialWindowsHID()
{

}


int SerialWindowsHID::ReadData(char *buffer, unsigned int nbChar)
{
	OVERLAPPED ov;
	DWORD dwCharsRead = 0;
	DWORD dwResult = 0;
	bool bSuccess;
	int iCharsRead;

	if (!m_upCurrentHID)
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

	bSuccess = ReadFile(m_upCurrentHID->Handle, buffer, nbChar, &dwCharsRead, &ov);
	if (bSuccess)
	{
		iCharsRead = dwCharsRead;
	}
	else
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			dwResult = WaitForSingleObject(ov.hEvent, 1);
			if (dwResult == WAIT_OBJECT_0)
			{
				if (GetOverlappedResult(m_upCurrentHID->Handle, &ov, &dwCharsRead, FALSE))
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
				CancelIo(m_upCurrentHID->Handle);
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

bool SerialWindowsHID::WaitReadData(char *buffer, unsigned int nbChar, unsigned long long ullMaxWait)
{

}

bool SerialWindowsHID::WriteData(const char *buffer, unsigned int nbChar)
{

}

bool SerialWindowsHID::WriteData(std::string sData)
{

}

bool SerialWindowsHID::IsConnected()
{

}

bool SerialWindowsHID::FlushBuffer()
{

}

int SerialWindowsHID::CharsInQueue()
{

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
	bool ret;

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
		std::unique_ptr<SP_DEVICE_INTERFACE_DETAIL_DATA> details;

		iface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		ret = SetupDiEnumDeviceInterfaces(info, nullptr, &guid, iIndex, &iface);

		//end of list
		if (!ret)
		{
			SetupDiDestroyDeviceInfoList(info);
			return vFoundHIDs;
		}

		SetupDiGetInterfaceDeviceDetail(info, &iface, nullptr, 0, &required_size, nullptr);
		details = std::make_unique<SP_DEVICE_INTERFACE_DETAIL_DATA>();
		details->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		ret = SetupDiGetDeviceInterfaceDetail(info, &iface, details.get(), required_size, nullptr, nullptr);
		if (!ret)
		{
			continue;
		}

		h = CreateFile(details->DevicePath,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);

		if (h == INVALID_HANDLE_VALUE)
		{
			continue;
		}

		attrib.Size = sizeof(HIDD_ATTRIBUTES);
		ret = HidD_GetAttributes(h, &attrib);
		
		if (!ret)
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
}

bool SerialWindowsHID::PickConnection(std::shared_ptr<RawHID> &upRawHID)
{
	if (m_upCurrentHID)
	{
		m_upCurrentHID.reset();
	}

	m_upCurrentHID = std::dynamic_pointer_cast<RawWinHID>(upRawHID);
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