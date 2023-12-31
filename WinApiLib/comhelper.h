#pragma once
#include <comutil.h>
#include <atlcomcli.h>
#include <string>
#include <wbemcli.h>

class ComHelper
{
public:
	template <typename T> static T readVariant(IWbemClassObject* pclsObj, const std::wstring& propertyName);

	template <> static std::wstring readVariant(IWbemClassObject* pclsObj, const std::wstring& propertyName)
	{
		CComVariant vtProp;
		auto hr = pclsObj->Get(propertyName.c_str(), 0, &vtProp, nullptr, nullptr);
		if (SUCCEEDED(hr))
		{
			if (vtProp.bstrVal != nullptr)
			{
				std::wstring propertyValue(vtProp.bstrVal);
				return propertyValue;
			}
			VariantClear(&vtProp);
		}
		
		return L"";
	}

	template <> static unsigned int readVariant(IWbemClassObject* pclsObj, const std::wstring& propertyName)
	{
		CComVariant vtProp;
		auto hr = pclsObj->Get(propertyName.c_str(), 0, &vtProp, nullptr, nullptr);
		if (SUCCEEDED(hr))
		{
			const auto propertyValue = vtProp.uintVal;
			return propertyValue;
		}
		return -1;
	}

	template <> static unsigned long readVariant(IWbemClassObject* pclsObj, const std::wstring& propertyName)
	{
		CComVariant vtProp;
		auto hr = pclsObj->Get(propertyName.c_str(), 0, &vtProp, nullptr, nullptr);
		if (SUCCEEDED(hr))
		{
			const auto propertyValue = vtProp.ulVal;
			return propertyValue;
		}
		return 0UL;
	}

	template <> static BSTR readVariant(IWbemClassObject* pclsObj, const std::wstring& propertyName)
	{
		CComVariant vtProp;
		auto hr = pclsObj->Get(propertyName.c_str(), 0, &vtProp, nullptr, nullptr);
		if (SUCCEEDED(hr))
		{
			if (vtProp.vt == VT_BSTR)
			{
				const auto propertyValue = vtProp.bstrVal;
				return propertyValue;
			}
		}
		wchar_t a[4]{};
		wcscpy_s(a, 1, L"");
		const BSTR bstr(a);
		return bstr;
	}
};

