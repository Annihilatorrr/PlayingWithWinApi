#pragma once
#include <set>
#include <string>

#include "dllexport.h"

class DLLEXPORT CimHelper
{
public:
	static std::set<std::wstring> getWbemProperties(const wchar_t* query);
};

