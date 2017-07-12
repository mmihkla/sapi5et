#pragma once

class CSapi5Factory : public IClassFactory
{
public:
	CSapi5Factory();
	virtual ~CSapi5Factory();

	// IUnknown
	STDMETHOD(QueryInterface)(REFIID refIID, void **ppInterface);
	STDMETHOD_(ULONG, AddRef());
	STDMETHOD_(ULONG, Release());

	// IClassFactory
	STDMETHOD(CreateInstance)(LPUNKNOWN pUnkOuter, REFIID refIID, void** ppInterface);
	STDMETHOD(LockServer)(BOOL bLock);

private:
	long m_lRefCount;
};
