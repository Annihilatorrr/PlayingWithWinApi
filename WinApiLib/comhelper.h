#pragma once
#include <comutil.h>
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
		if (SUCCEEDED(hr) && vtProp.bstrVal != nullptr)
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

	template <> static unsigned long readVariant(IWbemClassObject* pclsObj, std::wstring propertyName)
	{
		VARIANT vtProp;
		auto hr = pclsObj->Get(propertyName.c_str(), 0, &vtProp, nullptr, nullptr);
		if (SUCCEEDED(hr))
		{
			const auto propertyValue = vtProp.ulVal;
			VariantClear(&vtProp);
			return propertyValue;
		}
		return 0UL;
	}

	template <> static BSTR readVariant(IWbemClassObject* pclsObj, std::wstring propertyName)
	{
		VARIANT vtProp;
		auto hr = pclsObj->Get(propertyName.c_str(), 0, &vtProp, nullptr, nullptr);
		if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR)
		{
			const auto propertyValue = vtProp.bstrVal;
			VariantClear(&vtProp);
			return propertyValue;
		}
		wchar_t a[4]{};
		wcscpy_s(a, 1, L"");
		BSTR bstr1 = _bstr_t(a);
		return bstr1;
	}
};

