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

#include "erroroutput.h"
#include "mulfactor.h"
#include "processinfo.h"

namespace WA {
	class DLLEXPORT Process
	{
		DWORD buildProcessTreeViaSnapshot(std::map<DWORD, ProcessInfo>& processMap);
		std::map<unsigned int, ProcessInfo> buildProcessTree(std::vector<ProcessInfo>& processMap);

	public:
		
		ExtendedInfo getExtendedProcessInfo(DWORD processID) const;
		std::map<DWORD, ProcessInfo> enumerateProcesses(bool withMemoryInfo) const;
		std::map<DWORD, ProcessInfo> enumerateProcessesTree(bool withMemoryInfo);
		std::unique_ptr<std::map<unsigned int, ProcessInfo>> enumerateProcessesCom();

	};
}
