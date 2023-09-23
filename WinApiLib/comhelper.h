#pragma once
#include <string>
#include <wbemcli.h>

class ComHelper
{
public:
	template <typename T> static T readVariant(IWbemClassObject* pclsObj, std::wstring propertyName) {}

	template <> static std::wstring readVariant(IWbemClassObject* pclsObj, std::wstring propertyName)
	{
		VARIANT vtProp;
		auto hr = pclsObj->Get(propertyName.c_str(), 0, &vtProp, nullptr, nullptr);
		if (SUCCEEDED(hr))
		{
			std::wstring propertyValue(vtProp.bstrVal);
			VariantClear(&vtProp);
			return propertyValue;
		}
		return L"";
	}

	template <> static unsigned int readVariant(IWbemClassObject* pclsObj, std::wstring propertyName)
	{
		VARIANT vtProp;
		auto hr = pclsObj->Get(propertyName.c_str(), 0, &vtProp, nullptr, nullptr);
		if (SUCCEEDED(hr))
		{
			const auto propertyValue = vtProp.uintVal;
			VariantClear(&vtProp);
			return propertyValue;
		}
		return -1;
	}
};

