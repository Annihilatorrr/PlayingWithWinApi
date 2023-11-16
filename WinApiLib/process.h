#pragma once
#include <iostream>
#include <list>
#include <map>
#include <ostream>

#include "dllexport.h"
#include "framework.h"
#include <Wtsapi32.h>
#include <sddl.h>
#include <psapi.h>
#include <Tlhelp32.h>
#include <vector>
#include <wbemcli.h>

#include "erroroutput.h"
#include "mulfactor.h"
#include "processinfo.h"

namespace WA {
	class DLLEXPORT Process
	{
		std::map<unsigned int, ProcessInfo> getProcessTreeByCom(IWbemServices* pServices);
		std::map<DWORD, ProcessInfo> getProcessTreeBySnapshot(bool withMemoryInfo) const;

		ExtendedInfo getExtendedProcessInfo(DWORD processID) const;

	public:
		
		std::map<DWORD, ProcessInfo> getProcessTreeBySnapshot();
		std::map<unsigned int, ProcessInfo> getProcessTreeByCom();

	};
}
