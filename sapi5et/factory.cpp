#include "stdafx.h"
#include "factory.h"
#include "engine.h"

extern long g_lLockCount;

CSapi5Factory::CSapi5Factory() : m_lRefCount(0)
{
//	InterlockedIncrement(&g_ObjectCount);
}

CSapi5Factory::~CSapi5Factory()
{
//	ASSERT(InterlockedDecrement(&g_ObjectCount) >= 0);
}


STDMETHODIMP CSapi5Factory::QueryInterface(REFIID refIID, void** ppInterface)
{
	HRESULT theResult;

	if (refIID == IID_IUnknown || refIID == IID_IClassFactory) {
		*ppInterface = this;
	}
	else {
		*ppInterface = NULL;
	}

	if (*ppInterface)	{
		AddRef();
		theResult = S_OK;
	}
	else
		theResult = E_NOINTERFACE;

	return theResult;
}

STDMETHODIMP_(ULONG) CSapi5Factory::AddRef()
{
	return InterlockedIncrement(&m_lRefCount);
}

STDMETHODIMP_(ULONG) CSapi5Factory::Release()
{
	long count = InterlockedDecrement(&m_lRefCount);
	// since we are statically allocated, we don't delete ourselves
	return count;
}

STDMETHODIMP CSapi5Factory::LockServer(BOOL bLock)
{
	if (bLock) {
		InterlockedIncrement(&g_lLockCount);
	}
	else {
		ASSERT(g_lLockCount >= 0);
		InterlockedDecrement(&g_lLockCount);
	}
	return S_OK;
}

STDMETHODIMP CSapi5Factory::CreateInstance(IUnknown *pUnkOuter, REFIID refIID, void **ppInterface)
{
	TRACE(L"CSapi5Factory::CreateInstance\n");

	*ppInterface = 0;
	if (pUnkOuter != NULL) {// we don't support aggregation
		return CLASS_E_NOAGGREGATION;
	}

	CSapi5Engine *pEngine = new CSapi5Engine();
	if (!pEngine) {
		return E_OUTOFMEMORY;
	}
/*	if (!SUCCEEDED(pPlugin->Init())) {
		delete pPlugin;
		return E_FAIL;
	}*/

	pEngine->AddRef();
	HRESULT hr = pEngine->QueryInterface(refIID, ppInterface);
	pEngine->Release();

	return hr;
}
