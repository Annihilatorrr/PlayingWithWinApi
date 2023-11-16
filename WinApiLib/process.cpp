#include "process.h"
#include <comdef.h>
#include <Wbemidl.h>
#include <map>

#include "comhelper.h"

std::map<unsigned int, ProcessInfo> WA::Process::getProcessTreeByCom(IWbemServices* pServices)
{
	IEnumWbemClassObject* pEnumerator = nullptr;
	auto hr = pServices->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_Process"), WBEM_FLAG_FORWARD_ONLY, nullptr, &pEnumerator);
	if (FAILED(hr))
	{
		return {};
	}

	IWbemClassObject* pclsObj = nullptr;
	ULONG uReturn = 0;

	std::map<unsigned int, ProcessInfo> processMap;

	while (pEnumerator)
	{
		hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
		if (uReturn == 0)
		{
			break;
		}
		
		SAFEARRAY* psaNames = nullptr;
		hr = pclsObj->GetNames(
			nullptr,
			WBEM_FLAG_ALWAYS | WBEM_FLAG_NONSYSTEM_ONLY,
			nullptr,
			&psaNames);
		
		// Get the number of properties.
		long lLower, lUpper;
		SafeArrayGetLBound(psaNames, 1, &lLower);
		SafeArrayGetUBound(psaNames, 1, &lUpper);

		const auto name = ComHelper::readVariant<std::wstring>(pclsObj, std::wstring(L"Name"));
		auto processId = ComHelper::readVariant<unsigned int>(pclsObj, std::wstring(L"ProcessId"));
		const auto sessionId = ComHelper::readVariant<unsigned int>(pclsObj, std::wstring(L"SessionId"));
		const auto parentProcessId = ComHelper::readVariant<unsigned int>(pclsObj, std::wstring(L"ParentProcessId"));

		ProcessInfo pi{ processId, name, sessionId };
		pi.parentProcessId = parentProcessId;
		pi.extendedInfo.memoryInfo.PageFaultCount = ComHelper::readVariant<unsigned int>(pclsObj, std::wstring(L"PageFaultCount"));
		pi.extendedInfo.memoryInfo.QuotaPeakNonPagedPoolUsage = ComHelper::readVariant<unsigned int>(pclsObj, std::wstring(L"QuotaPeakNonPagedPoolUsage"));
		pi.extendedInfo.memoryInfo.QuotaPagedPoolUsage = ComHelper::readVariant<unsigned int>(pclsObj, std::wstring(L"QuotaPeakPagedPoolUsage"));
		pi.extendedInfo.memoryInfo.PageFileUsage = ComHelper::readVariant<unsigned int>(pclsObj, std::wstring(L"PageFileUsage"));
		pi.extendedInfo.memoryInfo.PeakWorkingSetSize = ComHelper::readVariant<unsigned int>(pclsObj, std::wstring(L"PeakWorkingSetSize"));
		pi.extendedInfo.memoryInfo.QuotaPagedPoolUsage = ComHelper::readVariant<unsigned int>(pclsObj, std::wstring(L"QuotaPagedPoolUsage"));
		pi.extendedInfo.memoryInfo.QuotaNonPagedPoolUsage = ComHelper::readVariant<unsigned int>(pclsObj, std::wstring(L"QuotaNonPagedPoolUsage"));
		pi.extendedInfo.memoryInfo.PeakPageFileUsage = ComHelper::readVariant<unsigned int>(pclsObj, std::wstring(L"PeakPageFileUsage"));
		pi.extendedInfo.memoryInfo.QuotaNonPagedPoolUsage = ComHelper::readVariant<unsigned int>(pclsObj, std::wstring(L"QuotaNonPagedPoolUsage"));

		auto wssString = ComHelper::readVariant<std::wstring>(pclsObj, std::wstring(L"WorkingSetSize"));
		const auto workingSetSize = std::stoll(wssString);
		pi.extendedInfo.memoryInfo.WorkingSetSize = workingSetSize >> 10;

		USHORT processMachine;
		USHORT nativeMachine;
		if (IsWow64Process2(GetCurrentProcess(), &processMachine, &nativeMachine))
		{
			if (processMachine == IMAGE_FILE_MACHINE_UNKNOWN)
			{
				if (nativeMachine == IMAGE_FILE_MACHINE_IA64 || nativeMachine == IMAGE_FILE_MACHINE_AMD64 || nativeMachine == IMAGE_FILE_MACHINE_ARM64) {
					pi.windowsIs32Bit = false;
					pi.processIs32Bit = false;
				}

				if (nativeMachine == IMAGE_FILE_MACHINE_I386 || nativeMachine == IMAGE_FILE_MACHINE_ARM) {
					pi.windowsIs32Bit = true;
					pi.processIs32Bit = true;
				}
			}
		}
		SafeArrayDestroy(psaNames);
		processMap.insert({ pi.id, pi });
		pclsObj->Release();
	}

	pEnumerator->Release();

	return processMap;
}

ExtendedInfo WA::Process::getExtendedProcessInfo(DWORD processID) const
{
	PROCESS_MEMORY_COUNTERS pmc;

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);

	if (nullptr == hProcess)
	{
		if (processID == 172)
		{
			SIZE_T inSz;
			SIZE_T outSz;
			GetProcessWorkingSetSize(hProcess, &inSz, &outSz);
			int r{};
		}
		return {};
	}

	HMODULE hMods[1024];
	DWORD cbNeeded;
	ExtendedInfo extendedInfo{};
	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];

			// Get the full path to the module's file.
			ModuleInfo moduleInfo{};
			MODULEINFO mi;
			if (GetModuleFileNameEx(hProcess, hMods[i], szModName,
				sizeof(szModName) / sizeof(TCHAR)))
			{
				// Print the module name and handle value.

				moduleInfo.szModName.assign(szModName);
			}
			if (GetModuleInformation(hProcess, hMods[i], &mi, sizeof(MODULEINFO)))
			{
				moduleInfo.EntryPoint = mi.EntryPoint;
				moduleInfo.SizeOfImage = mi.SizeOfImage;
			}
			extendedInfo.moduleInfo.push_back(moduleInfo);
		}
	}

	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
	{
		MemoryInfo mi
		{
			pmc.PageFaultCount,
			pmc.PeakWorkingSetSize,
			pmc.WorkingSetSize,
			pmc.QuotaPeakNonPagedPoolUsage,
			pmc.QuotaPagedPoolUsage,
			pmc.QuotaPeakNonPagedPoolUsage,
			pmc.QuotaNonPagedPoolUsage,
			pmc.PagefileUsage,
			pmc.PeakPagefileUsage
		};
		extendedInfo.memoryInfo = mi;
	}

	CloseHandle(hProcess);
	return extendedInfo;
}

std::map<DWORD, ProcessInfo> WA::Process::getProcessTreeBySnapshot(bool withMemoryInfo) const
{
	WTS_PROCESS_INFO* pWPIs = nullptr;
	DWORD dwProcCount = 0;
	std::map<DWORD, ProcessInfo> processInfos;
	if (WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, NULL, 1, &pWPIs, &dwProcCount))
	{
		//Go through all processes retrieved
		for (DWORD i = 0; i < dwProcCount; i++)
		{
			const auto& wtsInfo = pWPIs[i];
			ProcessInfo pi(wtsInfo.ProcessId, wtsInfo.pProcessName, wtsInfo.SessionId);

			LPWSTR sidStringBuffer{};

			if (wtsInfo.pUserSid != nullptr)
			{
				// Allocate a buffer of the required size
				if (ConvertSidToStringSid(wtsInfo.pUserSid, &sidStringBuffer))
				{
					pi.userSid.assign(sidStringBuffer);// = user SID that started the process
				}
				else
				{
					std::cout << "Failed to convert to string: error code " << GetLastError() << std::endl;
				}
			}
			if (withMemoryInfo)
			{
				pi.extendedInfo = getExtendedProcessInfo(pi.id);
			}

			processInfos.insert({ wtsInfo.ProcessId, pi });
		}
	}
	else
	{
		std::cout << "Failed to enum processes " << GetLastError() << std::endl;
	}

	//Free memory
	if (pWPIs)
	{
		WTSFreeMemory(pWPIs);
		pWPIs = nullptr;
	}
	return processInfos;
}

std::map<DWORD, ProcessInfo> WA::Process::getProcessTreeBySnapshot()
{
	auto processMap = getProcessTreeBySnapshot(true);
	
	return processMap;
}

std::map<unsigned int, ProcessInfo> WA::Process::getProcessTreeByCom()
{
    CoInitialize(nullptr);
	IWbemLocator* pLocator = nullptr;
	IWbemServices* pServices = nullptr;

	HRESULT hr = CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<LPVOID*>(&pLocator));

	if (SUCCEEDED(hr))
	{
		// The WMI namespace root/cimv2 is the default namespace and contains classes for computer hardware and configuration.
		hr = pLocator->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, nullptr, NULL, nullptr, nullptr, &pServices);
		if (FAILED(hr))
		{
			pLocator->Release();
			CoUninitialize();
			return {};
		}

		// Sets the authentication information that will be used to make calls on the specified proxy
		hr = CoSetProxyBlanket(pServices,
			RPC_C_AUTHN_WINNT,
			RPC_C_AUTHZ_NONE,
			nullptr,
			RPC_C_AUTHN_LEVEL_CALL,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			nullptr,
			EOAC_NONE);
		if (FAILED(hr))
		{
			pServices->Release();
			pLocator->Release();
			CoUninitialize();
			return {};
		}
	}

	auto processTree = getProcessTreeByCom(pServices);
	pServices->Release();
	pLocator->Release();
	CoUninitialize();
	return processTree;
}
