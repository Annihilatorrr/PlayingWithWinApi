#include "erroroutput.h"

#include <locale>

#include "framework.h"

std::wstring WA::ErrorOutput::getLastError()
{
	LPWSTR message = nullptr;
	DWORD  error = 0;
	error = GetLastError();
	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM, // use windows internal message table
		0,       // 0 since source is internal message table
		std::codecvt_base::error,    // this is the error code 
		// Could just as well have been an error code from generic
		// Windows errors from GetLastError()
		0,       // auto-determine language to use
		(LPWSTR)&message,
		0,       // min size for buffer
		0);

	return std::wstring{message};
}
