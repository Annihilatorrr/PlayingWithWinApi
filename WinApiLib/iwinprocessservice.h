#pragma once
#include <map>

#include "dllexport.h"
#include "processinfo.h"

namespace WA {
	class DLLEXPORT IWinProcessService
	{
	public:
		IWinProcessService() = default;
		virtual ~IWinProcessService() = default;
		virtual std::map<DWORD, ProcessInfo> getProcessTreeBySnapshot() const = 0;
		virtual std::map<unsigned int, ProcessInfo> getProcessTreeByCom() = 0;
		virtual void kill(unsigned int processId) = 0;
	};
}
