#include "stdafx.h"
#include "engine.h"

extern long g_lObjectCount;

CSapi5Engine::CSapi5Engine() :
	m_lRefCount(0),
	m_bInitialized(FALSE),
	m_pObjectToken(NULL),
	m_iBaseRate(0),
	m_iBaseVolume(100)
{
	TRACE(L"CSapi5Engine::CSapi5Engine\n");
	InterlockedIncrement(&g_lObjectCount);
}

CSapi5Engine::~CSapi5Engine()
{
	TRACE(L"CSapi5Engine::~CSapi5Engine\n");
	InterlockedDecrement(&g_lObjectCount);
}

STDMETHODIMP CSapi5Engine::QueryInterface(REFIID refIID, void **ppInterface)
{
	*ppInterface = NULL;
	if (IsEqualIID(refIID, IID_IUnknown) || IsEqualIID(refIID, IID_ISpTTSEngine)) {
		*ppInterface = (ISpTTSEngine *)this;
	}
	else if (IsEqualIID(refIID, IID_ISpObjectWithToken)) {
		*ppInterface = (ISpObjectWithToken *)this;
	}
	else {
		return E_NOINTERFACE;
	}

	this->AddRef();
	return S_OK;
}

STDMETHODIMP_(ULONG) CSapi5Engine::AddRef()
{
	return InterlockedIncrement(&m_lRefCount);
}

STDMETHODIMP_(ULONG) CSapi5Engine::Release()
{
	ULONG lCount = InterlockedDecrement(&m_lRefCount);
	if (lCount == 0) {
		delete this;
	}
	return lCount;
}

STDMETHODIMP CSapi5Engine::GetObjectToken(ISpObjectToken **ppToken)
{
	TRACE(L"CSapi5Engine::GetObjectToken\n");

	if (!ppToken) {
		return E_POINTER;
	}

	*ppToken = m_pObjectToken;
	if (m_pObjectToken) {
		m_pObjectToken->AddRef();
		return S_OK;
	}

	return S_FALSE;
}

STDMETHODIMP CSapi5Engine::SetObjectToken(ISpObjectToken *pToken)
{
	TRACE(L"CSapi5Engine::SetObjectToken\n");

	if (!pToken) {
		return E_INVALIDARG;
	}
	if (m_pObjectToken) {
		return SPERR_ALREADY_INITIALIZED;
	}

	m_pObjectToken = pToken;
	m_pObjectToken->AddRef();

	try {
		CVoiceSettings VoiceSettings;
		VoiceSettings.speed = DWORDtoDouble(GetTokenDWORDValue(pToken, L"VoiceSpeed", 100));
		VoiceSettings.ht = DWORDtoDouble(GetTokenDWORDValue(pToken, L"VoiceHalfTone", 0));
		VoiceSettings.gvw1 = DWORDtoDouble(GetTokenDWORDValue(pToken, L"VoiceWeight1", 100));
		VoiceSettings.gvw2 = DWORDtoDouble(GetTokenDWORDValue(pToken, L"VoiceWeight2", 100));

		CFSFileName szPath = FSGetModulePath();
		CFSAutoLock AutoLock(&m_LinguisticMutex);
		m_Linguistic.Open(szPath + L"et.dct");
		m_Disambiguator.Open(szPath + L"et3.dct");
		OpenVoice(szPath + GetTokenStringValue(pToken, L"VoiceFile"), VoiceSettings);

		m_bInitialized = TRUE;
		return S_OK;
	}
	catch (...) {
		return E_FAIL;
	}
}

#define CHECK_ACTIONS { \
	DWORD dwActions = pOutputSite->GetActions(); \
	if (dwActions & SPVES_ABORT) { \
		TRACE(L"CSapi5Engine::Speak ABORT\n"); \
		return S_OK; \
	} \
	if (dwActions & SPVES_SKIP) { \
		SPVSKIPTYPE eSkipType; \
		long lNumItems; \
		hr = pOutputSite->GetSkipInfo(&eSkipType, &lNumItems); \
		if (FAILED(hr)) return hr; \
		if (lNumItems > 0 && eSkipType == SPVST_SENTENCE) { \
			if (ip + lNumItems > Sentences.GetSize()) lNumItems = Sentences.GetSize() - lNumItems; \
			ip += lNumItems - 1; \
			hr = pOutputSite->CompleteSkip(lNumItems); \
			if (FAILED(hr)) return hr; \
			goto NEXTSENTENCE; \
		} \
	} \
	if (dwActions & SPVES_RATE) { \
		long lTmp; \
		hr = pOutputSite->GetRate(&lTmp); \
		if (FAILED(hr)) return hr; \
		m_iBaseRate = FSMINMAX((int)lTmp, -10, 10); \
	} \
	if (dwActions & SPVES_VOLUME) { \
		USHORT usTmp; \
		hr = pOutputSite->GetVolume(&usTmp); \
		if (FAILED(hr)) return hr; \
		m_iBaseVolume = FSMINMAX((int)usTmp, 0, 100); \
	} \
}

STDMETHODIMP CSapi5Engine::Speak(DWORD dwFlags, REFGUID formatId, const WAVEFORMATEX *pFormat,
	const SPVTEXTFRAG *pTextFrag, ISpTTSEngineSite *pOutputSite)
{
	// dwFlags not implemented, also seems to be missing from MS voices
	TRACE(L"CSapi5Engine::Speak\n");
	if (!m_bInitialized) return E_FAIL;

	try {
		if (!IsEqualGUID(formatId, SPDFID_WaveFormatEx)) return SPERR_UNSUPPORTED_FORMAT;
		if (!pFormat || pFormat->nChannels != 1 || pFormat->wBitsPerSample != 16 || pFormat->nSamplesPerSec != 48000) return SPERR_UNSUPPORTED_FORMAT;

		HRESULT hr;
		ULONGLONG ullAudioOffset = 0;

		CFSClassArray<CFragment> Fragments;

		for (; pTextFrag; pTextFrag = pTextFrag->pNext) {

			CFSWString szText;
			if (pTextFrag->pTextStart) szText = CFSWString(pTextFrag->pTextStart, pTextFrag->ulTextLen);
			switch (pTextFrag->State.eAction) {
			case SPVA_Speak:
			case SPVA_SpellOut: {
				TRACE(L"- %s '%s'\n", (pTextFrag->State.eAction  == SPVA_Speak ? L"SPVA_Speak" : L"SPVA_SpellOut"), (const wchar_t *)szText);
				CFSClassArray<CFragment> Fragments2 = m_TextProcessor.SplitText(szText);
				for (INTPTR ip = 0; ip < Fragments2.GetSize(); ip++) {
					CFragment &Fragment = Fragments2[ip];
					Fragment.m_ipStartPos += pTextFrag->ulTextSrcOffset;
					Fragment.m_eAction = pTextFrag->State.eAction;
					Fragment.m_iVolume = FSMINMAX((int)pTextFrag->State.Volume, 0, 100);
					Fragment.m_iRate = FSMINMAX((int)pTextFrag->State.RateAdj, -10, 10);

					CFSWString szCleanWord = m_TextProcessor.GetCleanWord(Fragment.m_szText);
					if (!szCleanWord.GetLength()) continue;
					Fragment.m_Morph.m_szWord = szCleanWord;
					Fragment.m_Morph.m_MorphInfo = Analyze(Fragment.m_Morph.m_szWord);
					Fragments.AddItem(Fragment);
				}

				// Add a whitespace as some readers skip those
				if (pTextFrag->State.eAction == SPVA_Speak &&
					Fragments.GetSize() > 0 &&
					Fragments[Fragments.GetSize() - 1].m_eType != CFragment::TYPE_SPACE)
				{
					CFragment Fragment(pTextFrag->ulTextSrcOffset + szText.GetLength(), L"", CFragment::TYPE_SPACE);
					Fragment.m_eAction = pTextFrag->State.eAction;
					Fragments.AddItem(Fragment);
				}
			} break;
			case SPVA_Silence: {
				TRACE(L"- SPVA_Silence %d\n", (int)pTextFrag->State.SilenceMSecs);
				CFragment Fragment;
				Fragment.m_eAction = SPVA_Silence;
				Fragment.m_lParam = pTextFrag->State.SilenceMSecs;
				Fragments.AddItem(Fragment);
			} break;
			case SPVA_Bookmark: {
				TRACE(L"- SPVA_Bookmark '%s'\n", (const wchar_t *)szText);
				CFragment Fragment;
				Fragment.m_eAction = SPVA_Bookmark;
				Fragment.m_lParam = (LPARAM)pTextFrag->pTextStart;
				Fragments.AddItem(Fragment);
			} break;
			case SPVA_Pronounce:
			case SPVA_Section:
			case SPVA_ParseUnknownTag:
			default:
				TRACE(L"- SPVA_* unsupported action %d\n", (int)pTextFrag->State.eAction);
				break;
			}
		}

		// Remove extra whitespaces
		CFragment::FRAGMENTTYPE ePrevType = CFragment::TYPE_OTHER;
		for (INTPTR ip = 0; ip < Fragments.GetSize(); ip++) {
			if (Fragments[ip].IsSpeakAction()) continue;
			if (Fragments[ip].m_eType == CFragment::TYPE_SPACE && ePrevType == CFragment::TYPE_SPACE) {
				Fragments.RemoveItem(ip);
				ip--;
			}
			else {
				ePrevType = Fragments[ip].m_eType;
			}
		}
		CFSClassArray<CFSClassArray<CFragment> > Sentences = m_TextProcessor.CreateSentences(Fragments);
		Fragments.Cleanup();

		for (INTPTR ip = 0; ip < Sentences.GetSize(); ip++) {
			CFSClassArray<CFragment> &Sentence = Sentences[ip];
			CFSArray<CMorphInfos> Analysis, Disambiguated;

			// Let's check if host has changed its mind.
			CHECK_ACTIONS;

			// Inform host about beginning of the sentence
			if (GetEventInterest(pOutputSite, SPEI_SENTENCE_BOUNDARY)) {
				INTPTR ipStart = -1;
				INTPTR ipEnd = 0;
				for (INTPTR ip2 = 0; ip2 < Sentence.GetSize(); ip2++) {
					if (Sentence[ip2].IsSpeakAction()) {
						if (ipStart == -1) ipStart = Sentence[ip2].m_ipStartPos;
						ipEnd = Sentence[ip2].m_ipStartPos + Sentence[ip2].m_szText.GetLength();
					}
				}
				if (ipStart == -1) ipStart = 0;

				SPEVENT Event; memset(&Event, 0, sizeof(Event));
				Event.eEventId = SPEI_SENTENCE_BOUNDARY;
				Event.ullAudioStreamOffset = ullAudioOffset;
				Event.lParam = (LPARAM)ipStart;
				Event.wParam = (WPARAM)(ipEnd - ipStart);
				// TRACE(L"Sending Event SPEI_SENTENCE_BOUNDARY\n");
				hr = pOutputSite->AddEvents(&Event, 1);
				if (FAILED(hr)) return hr;
			}

			// Disambiguate sentence
			for (INTPTR ip2 = 0; ip2 < Sentence.GetSize(); ip2++) {
				if (!Sentence[ip2].IsSpeakAction()) continue;
				switch (Sentence[ip2].m_eType) {
				case CFragment::TYPE_WORD:
				case CFragment::TYPE_PUNCTUATION:
				case CFragment::TYPE_SYMBOL:
					Analysis.AddItem(Sentence[ip2].m_Morph);
				}
			}
			{
				CFSAutoLock AutoLock(&m_LinguisticMutex);
				Disambiguated = m_Disambiguator.Disambiguate(Analysis);
			}
			if (Disambiguated.GetSize() != Analysis.GetSize()) {
				TRACE(L"ERROR disambiguator result does not match input\n");
				ASSERT(false); // should never reach here.
				return E_FAIL;
			}
			INTPTR ipDisamb = 0;
			for (INTPTR ip2 = 0; ip2 < Sentence.GetSize(); ip2++) {
				if (!Sentence[ip2].IsSpeakAction()) continue;
				switch (Sentence[ip2].m_eType) {
				case CFragment::TYPE_WORD:
				case CFragment::TYPE_PUNCTUATION:
				case CFragment::TYPE_SYMBOL:
					Sentence[ip2].m_Morph = Disambiguated[ipDisamb++];
				}
			}

			// Double check if host has changed its mind.
			CHECK_ACTIONS;

			double fVolume = (m_iBaseVolume ? 20.0 * log10(0.01 * m_iBaseVolume) : -144.0);
			double fSpeed = pow(1.1, m_iBaseRate);
			CreateAudio(Sentence, fVolume, fSpeed);

			// Let's start outputting
			for (INTPTR ip2 = 0; ip2 < Sentence.GetSize(); ip2++) {
				const CFragment &Fragment = Sentence[ip2];

				// Double check if host has changed its mind.
				CHECK_ACTIONS;

				switch (Fragment.m_eAction) {
				case SPVA_Speak:
				case SPVA_SpellOut: {
					if (Fragment.m_Audio.GetSize() == 0) {
						break;
					}
					if (GetEventInterest(pOutputSite, SPEI_WORD_BOUNDARY)) {
						SPEVENT Event; memset(&Event, 0, sizeof(Event));
						Event.eEventId = SPEI_WORD_BOUNDARY;
						Event.ullAudioStreamOffset = ullAudioOffset;
						Event.lParam = Fragment.m_ipStartPos;
						Event.wParam = Fragment.m_szText.GetLength();
						// TRACE(L"Sending Event SPEI_WORD_BOUNDARY\n");
						hr = pOutputSite->AddEvents(&Event, 1);
						if (FAILED(hr)) return hr;
					}

					const BYTE *pAudioData = (const BYTE *)Fragment.m_Audio.GetData();
					INTPTR ipDataLeft = Fragment.m_Audio.GetSize();
					while (ipDataLeft > 0) {
						CHECK_ACTIONS;

						INTPTR ipWrite = FSMIN(ipDataLeft, 9600); // <= 100ms

						ULONG ulWritten = 0;
						hr = pOutputSite->Write(pAudioData, ipWrite, &ulWritten);
						if (FAILED(hr)) return hr;

						pAudioData += ulWritten;
						ipDataLeft -= ulWritten;
						ullAudioOffset += ulWritten;
					}
				} break;
				case SPVA_Silence: {
					INTPTR ipMSec = Fragment.m_lParam;
					UINT uiSampleSize = pFormat->nChannels * pFormat->wBitsPerSample / 8;
					INTPTR ip100 = pFormat->nSamplesPerSec / 10 * uiSampleSize; // 100ms

					CFSData Silence;
					Silence.SetSize(ip100);
					memset(Silence.GetData(), 0, Silence.GetSize());

					while (ipMSec > 0) {
						CHECK_ACTIONS;

						INTPTR ipWrite;
						if (ipMSec >= 100) {
							ipWrite = ip100;
							ipMSec -= 100;
						}
						else {
							ipWrite = pFormat->nSamplesPerSec * ipMSec / 1000 * uiSampleSize;
							ipMSec = 0;
						}

						ULONG ulWritten = 0;
						HRESULT hr = pOutputSite->Write(Silence.GetData(), ipWrite, &ulWritten);
						if (FAILED(hr)) return hr;

						ullAudioOffset += ulWritten;
					}
				} break;
				case SPVA_Bookmark: {
					if (GetEventInterest(pOutputSite, SPEI_TTS_BOOKMARK)) {
						SPEVENT Event; memset(&Event, 0, sizeof(Event));
						Event.eEventId = SPEI_TTS_BOOKMARK;
						Event.elParamType = SPET_LPARAM_IS_STRING;
						Event.ullAudioStreamOffset = ullAudioOffset;
						Event.lParam = Fragment.m_lParam;
						Event.wParam = Fragment.m_lParam ? _wtol((LPCWSTR)Fragment.m_lParam) : 0;
						// TRACE(L"Sending Event SPEI_TTS_BOOKMARK\n");
						hr = pOutputSite->AddEvents(&Event, 1);
						if (FAILED(hr)) return hr;
					}
				} break;
				default:
					TRACE(L"Unknown eAction in the output, should not reach here!\n");
					ASSERT(false);
				}
			}
NEXTSENTENCE:
			Sentence.Cleanup();
		}
		TRACE(L"CSapi5Engine::Speak END\n");
		return S_OK;
	}
	catch (...) {
		return E_FAIL;
	}
}

STDMETHODIMP CSapi5Engine::GetOutputFormat(const GUID *pTargetFormatId, const WAVEFORMATEX *pTargetFormat,
	GUID *pFormatId, WAVEFORMATEX **ppFormat)
{
	TRACE(L"CSapi5Engine::GetOutputFormat\n");
	FSUNUSED(pTargetFormatId);
	FSUNUSED(pTargetFormat);
	return SpConvertStreamFormatEnum(SPSF_48kHz16BitMono, pFormatId, ppFormat);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

CFSArray<CMorphInfo> CSapi5Engine::Analyze(const CFSWString &szWord)
{
	CFSAutoLock AutoLock(&m_LinguisticMutex);
	return m_Linguistic.Analyze(szWord);
}

CFSWString CSapi5Engine::GetTokenStringValue(ISpObjectToken *pToken, const CFSWString &szKey, const CFSWString &szDefault)
{
	CFSWString result;
	if (!pToken) return szDefault;

	LPWSTR pwValue = NULL;
	HRESULT hr = pToken->GetStringValue(szKey, &pwValue);
	if (FAILED(hr)) return szDefault;

	result = pwValue;
	CoTaskMemFree(pwValue);
	return result;
}

DWORD CSapi5Engine::GetTokenDWORDValue(ISpObjectToken *pToken, const CFSWString &szKey, DWORD dwDefault)
{
	if (!pToken) return dwDefault;

	DWORD dwValue;
	HRESULT hr = pToken->GetDWORD(szKey, &dwValue);
	if (FAILED(hr)) return dwDefault;

	return dwValue;
}

BOOL CSapi5Engine::GetEventInterest(ISpTTSEngineSite *pOutputSite, SPEVENTENUM EventId)
{
	ULONGLONG ullEventInterest = 0;
	HRESULT hr = pOutputSite->GetEventInterest(&ullEventInterest);
	if (FAILED(hr)) return FALSE;
	return !!(ullEventInterest & SPFEI(EventId));
}
