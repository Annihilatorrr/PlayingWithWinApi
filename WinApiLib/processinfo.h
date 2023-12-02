#pragma once
#include <string>
#include <windows.h>
#include <vector>
#include "perfrawdata.h"

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
    size_t PeakWorkingSetSize;
    size_t WorkingSetSize;
    size_t QuotaPeakPagedPoolUsage;
    size_t QuotaPagedPoolUsage;
    size_t QuotaPeakNonPagedPoolUsage;
    size_t QuotaNonPagedPoolUsage;
    size_t PageFileUsage;
    size_t PeakPageFileUsage;
};

struct ExtendedInfo
{
    MemoryInfo memoryInfo;
    std::vector<ModuleInfo> moduleInfo;

};
struct ProcessInfo
{
    ProcessInfo() :id{}, parentProcessId{}
    {
	    
    }
    ProcessInfo(DWORD id, std::wstring name, DWORD sessionId):
    id(id),
    parentProcessId{},
	name(std::move(name)),
    sessionId(sessionId)
    {
	    
    }

    ProcessInfo(DWORD id, std::wstring name, DWORD parentId, DWORD sessionId):
        id(id),
        parentProcessId{parentId},
        name(std::move(name)),
        sessionId(sessionId)
    {

    }

    DWORD id;     // process id
    DWORD parentProcessId;     // process id
    std::wstring name; // name of process
    DWORD sessionId;     // session id
    std::wstring userSid;       // user's SID
    std::wstring commandLine;
    std::wstring executablePath;
    ExtendedInfo extendedInfo;

    USHORT windowsIs32Bit;
    USHORT processIs32Bit;
    std::wstring description;
    PerfRawData perfData;
};

