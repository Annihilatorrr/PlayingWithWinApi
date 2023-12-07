#include "cimhelper.h"

#include <comdef.h>
#include <Wbemidl.h>
#include <wbemcli.h>

std::set<std::wstring> CimHelper::getWbemProperties(const wchar_t* query)
{
	CoInitialize(nullptr);
	IWbemLocator* pLocator = nullptr;

	HRESULT hr = CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<LPVOID*>(&pLocator));
	std::set<std::wstring> propertiesNames;

	if (SUCCEEDED(hr))
	{
		// The WMI namespace root/cimv2 is the default namespace and contains classes for computer hardware and configuration.
		IWbemServices* pServices = nullptr;
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

		IEnumWbemClassObject* pEnumerator = nullptr;
		auto hr = pServices->ExecQuery(bstr_t("WQL"), bstr_t(query),
			WBEM_FLAG_FORWARD_ONLY, nullptr, &pEnumerator);

		if (FAILED(hr))
		{
			pServices->Release();
			pLocator->Release();
			CoUninitialize();

			return{};
		}

		IWbemClassObject* pclsObj = nullptr;
		ULONG uReturn = 0;
		hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
		if (uReturn == 0 || FAILED(hr))
		{
			pServices->Release();
			pLocator->Release();
			CoUninitialize();
			return{};
		}
		SAFEARRAY* psaNames = nullptr;
		hr = pclsObj->GetNames(
			nullptr,
			WBEM_FLAG_ALWAYS | WBEM_FLAG_NONSYSTEM_ONLY,
			nullptr,
			&psaNames);
		if (propertiesNames.empty())
		{
			VARIANT* raw;
			HRESULT hr = SafeArrayAccessData(psaNames, reinterpret_cast<void**>(&raw)); // direct access to SA memory
			if (SUCCEEDED(hr))
			{
				long lLower, lUpper;
				SafeArrayGetLBound(psaNames, 1, &lLower);
				SafeArrayGetUBound(psaNames, 1, &lUpper);

				long elementCnt = lUpper - lLower + 1;
				for (LONG i = 0; i < elementCnt; ++i)  // iterate through returned values
				{
					_bstr_t		str;
					wchar_t* pwszPropName = nullptr;
					SafeArrayGetElement(psaNames, &i, &pwszPropName);
					propertiesNames.insert(pwszPropName);
				}
			}

			hr = ::SafeArrayUnaccessData(psaNames);
		}
		pServices->Release();
		pLocator->Release();
	}

	CoUninitialize();
	return propertiesNames;

}
