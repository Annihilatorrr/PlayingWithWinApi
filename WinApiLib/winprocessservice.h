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

#include "iwinprocessservice.h"
#include "processinfo.h"

namespace WA {
	class DLLEXPORT WinProcessService: public IWinProcessService
	{
		std::map<unsigned int, ProcessInfo> getProcessTreeByCom(IWbemServices* pServices);
		std::map<DWORD, ProcessInfo> getProcessTreeBySnapshot(bool withMemoryInfo) const;

		ExtendedInfo getExtendedProcessInfo(DWORD processID) const;
		std::map<unsigned int, PerfRawData> getProcessUsageInfo(IWbemServices* pServices);

	public:
		
		std::map<DWORD, ProcessInfo> getProcessTreeBySnapshot() const;
		std::map<unsigned int, ProcessInfo> getProcessTreeByCom();
		void kill(unsigned int processId);

	};
}
