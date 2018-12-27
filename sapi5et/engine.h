#pragma once

#include "speech.h"
#include "textprocessor.h"

class CSapi5Engine : public CSpeechEngine, public ISpObjectWithToken, public ISpTTSEngine
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
	CFSArray<CMorphInfo> Analyze(const CFSWString &szWord);

	CFSWString GetTokenStringValue(ISpObjectToken *pToken, const CFSWString &szKey, const CFSWString &szDefault = L"");
	DWORD GetTokenDWORDValue(ISpObjectToken *pToken, const CFSWString &szKey, DWORD dwDefault = 0);
	double DWORDtoDouble(DWORD dwNumber) { return 0.01 * (INT32)(UINT32)dwNumber; }

	HRESULT OutputSilence(const WAVEFORMATEX *pFormat, UINT uiMSec, ISpTTSEngineSite *pOutputSite, ULONG *pWritten);
	BOOL GetEventInterest(ISpTTSEngineSite *pOutputSite, SPEVENTENUM EventId);

	long m_lRefCount;
	ISpObjectToken *m_pObjectToken;

	CTextProcessor m_TextProcessor;
	CSpeechLinguistic m_Linguistic;
	CDisambiguator m_Disambiguator;
	CFSMutex m_LinguisticMutex;

	BOOL m_bInitialized;

	int m_iBaseRate; // -10 ... 10
	int m_iBaseVolume; // 0 ... 100%
};