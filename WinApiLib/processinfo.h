#pragma once
#include <string>

#include "framework.h"

struct ModuleInfo
{
    LPVOID lpBaseOfDll;
    DWORD  SizeOfImage;
    LPVOID EntryPoint;
    std::wstring szModName;
};
struct MemoryInfo
{
    DWORD PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PageFileUsage;
    SIZE_T PeakPageFileUsage;
};

struct ExtendedInfo
{
    MemoryInfo memoryInfo;
    std::vector<ModuleInfo> moduleInfo;

};
struct ProcessInfo
{
    ProcessInfo(DWORD id, std::wstring name, DWORD sessionId):
    id(id),
    name(std::move(name)),
        sessionId(sessionId)
	{}

    DWORD id;     // process id
    DWORD parentProcessId;     // process id
    std::wstring name; // name of process
    DWORD sessionId;     // session id
    ProcessInfo* parent;     // process id
    std::vector<ProcessInfo*> childProcesses;
    std::wstring userSid;       // user's SID
    ExtendedInfo extendedInfo;

    USHORT windowsIs32Bit;
    USHORT processIs32Bit;
};

