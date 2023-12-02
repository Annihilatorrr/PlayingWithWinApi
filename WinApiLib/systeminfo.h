#pragma once

#include "comhelper.h"
#include "dllexport.h"
namespace WA
{
	class DLLEXPORT SystemInfo
	{
		unsigned int getCpuCount() const
		{
			SYSTEM_INFO systemInfo;
			GetSystemInfo(&systemInfo);
			return systemInfo.dwNumberOfProcessors;
		}
	};
};

