#include "stdafx.h"
#include <initguid.h>
#include <olectl.h>
#include "factory.h"

const wchar_t *pwszCLSID_Sapi5ET = L"{8900F658-092B-4ADF-92EC-8CE13593F0E1}";
DEFINE_GUID(CLSID_Sapi5ET, 0x8900f658, 0x92b, 0x4adf, 0x92, 0xec, 0x8c, 0xe1, 0x35, 0x93, 0xf0, 0xe1);

long g_lObjectCount = 0;
long g_lLockCount = 0;

FSDLL_ENTRYPOINT;

STDAPI DllCanUnloadNow(void)
{
	TRACE(L"DllCanUnloadNow\n");

	return (g_lObjectCount == 0 && g_lLockCount == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	TRACE(L"DllGetClassObject\n");

	static CSapi5Factory factory;

	if (rclsid == CLSID_Sapi5ET) {
		return factory.QueryInterface(riid, ppv);
	}
	*ppv = NULL;
	return CLASS_E_CLASSNOTAVAILABLE;
}

STDAPI DllUnregisterServer(void)
{
	TRACE(L"DllUnregisterServer\n");

	HRESULT hr = S_OK;
	if (CFSReg::Delete(CFSString(L"HKEY_CLASSES_ROOT\\CLSID\\") + pwszCLSID_Sapi5ET + L"\\") != 0) hr = S_FALSE;

	ISpObjectToken *pToken;
	if (SUCCEEDED(hr)) hr = SpCreateNewToken(SPCAT_VOICES, L"T\x00f5nu - Estonian", &pToken);
	if (SUCCEEDED(hr)) hr = pToken->Remove(NULL);

	return hr;
}

STDAPI DllRegisterServer(void)
{
	TRACE(L"DllRegisterServer\n");

	HRESULT hr = S_OK;
	CFSString szPath;
	GetModuleFileName(g_hFSInst, szPath.GetBuffer(_MAX_PATH + 1), _MAX_PATH + 1);
	szPath.ReleaseBuffer();
		
	if (CFSReg::WriteString(CFSString(L"HKEY_CLASSES_ROOT\\CLSID\\") + pwszCLSID_Sapi5ET + L"\\", L"Estonian TTS") != 0 ||
		CFSReg::WriteString(CFSString(L"HKEY_CLASSES_ROOT\\CLSID\\") + pwszCLSID_Sapi5ET + L"\\InprocServer32\\", szPath) != 0 ||
		CFSReg::WriteString(CFSString(L"HKEY_CLASSES_ROOT\\CLSID\\") + pwszCLSID_Sapi5ET + L"\\InprocServer32\\ThreadingModel", L"Both") != 0) hr = SELFREG_E_CLASS;

	ISpObjectToken *pToken;
	ISpDataKey *pDataKeyAttribs;
	if (SUCCEEDED(hr)) hr = SpCreateNewTokenEx(SPCAT_VOICES, L"T\x00f5nu - Estonian", &CLSID_Sapi5ET,
		L"T\x00f5nu - Estonian", 0x425, L"T\x00f5nu - Eesti", &pToken, &pDataKeyAttribs);
	if (SUCCEEDED(hr)) hr = pToken->SetStringValue(L"VoiceFile", L"tonu.htsvoice");
	if (SUCCEEDED(hr)) hr = pToken->SetStringValue(L"Language", L"et-ee");
	if (SUCCEEDED(hr)) hr = pDataKeyAttribs->SetStringValue(L"Language", L"425");
	if (SUCCEEDED(hr)) hr = pDataKeyAttribs->SetStringValue(L"Gender", L"Male");
	if (SUCCEEDED(hr)) hr = pDataKeyAttribs->SetStringValue(L"Age", L"Adult");
	if (SUCCEEDED(hr)) hr = pDataKeyAttribs->SetStringValue(L"Name", L"T\x00f5nu");
	if (SUCCEEDED(hr)) hr = pDataKeyAttribs->SetStringValue(L"Vendor", L"EKI");
	if (SUCCEEDED(hr)) hr = pDataKeyAttribs->SetStringValue(L"Version", L"1.0");

	if (SUCCEEDED(hr)) return hr;

	DllUnregisterServer();
	return SELFREG_E_CLASS;
}
