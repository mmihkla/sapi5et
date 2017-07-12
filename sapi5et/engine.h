#pragma once

#include "lib/hts/HTS_engine.h"
#include "lib/proof/proof.h"
#include "fragment.h"
#include "textprocessor.h"

class CSapi5Engine : public ISpObjectWithToken, public ISpTTSEngine
{
public:
	CSapi5Engine();
	virtual ~CSapi5Engine();

	// IUnknown
	STDMETHOD(QueryInterface)(REFIID refIID, void** ppInterface);
	STDMETHOD_(ULONG, AddRef());
	STDMETHOD_(ULONG, Release());

	// ISpObjectWithToken
	STDMETHOD(GetObjectToken)(ISpObjectToken **ppToken);
	STDMETHOD(SetObjectToken)(ISpObjectToken *pToken);

	// ISpTTSEngine
	STDMETHOD(Speak)(DWORD dwFlags, REFGUID formatId, const WAVEFORMATEX *pFormat,
		const SPVTEXTFRAG *pTextFrag, ISpTTSEngineSite *pOutputSite);
	STDMETHOD(GetOutputFormat)(const GUID *pTargetFormatId, const WAVEFORMATEX *pTargetFormat,
		GUID *pFormatId, WAVEFORMATEX **ppFormat);

private:
	CFSWString GetTokenStringValue(ISpObjectToken *pToken, const CFSString &szKey);
	HRESULT OutputAudio(const CFragment &Fragment, ISpTTSEngineSite *pOutputSite, ULONG *pWritten);
	HRESULT OutputSilence(const WAVEFORMATEX *pFormat, UINT uiMSec, ISpTTSEngineSite *pOutputSite, ULONG *pWritten);
	BOOL GetEventInterest(ISpTTSEngineSite *pOutputSite, SPEVENTENUM EventId);

	long m_lRefCount;
	ISpObjectToken *m_pObjectToken;

	BOOL m_bInitialized;
	HTS_Engine m_HTS;
	CTextProcessor m_TextProcessor;
	CLinguistic m_Linguistic;
	CDisambiguator m_Disambiguator;
	CFSMutex m_Mutex;

	int m_iBaseRate; // -10 ... 10
	int m_iBaseVolume; // 0 ... 100%
};