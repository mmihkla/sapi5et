#include "stdafx.h"
#include "engine.h"
#include "audio.h"

extern long g_lObjectCount;

CSapi5Engine::CSapi5Engine() :
	m_lRefCount(0),
	m_bInitialized(FALSE),
	m_pObjectToken(NULL),
	m_iBaseRate(0),
	m_iBaseVolume(100)
{
	InterlockedIncrement(&g_lObjectCount);
	m_Linguistic.m_bProperName = true;
	m_Linguistic.m_bGuess = true;
	HTS_Engine_initialize(&m_HTS);
	// HTS_Engine_set_audio_buff_size(&m_HTS, 1024 * 1024)
}

CSapi5Engine::~CSapi5Engine()
{
	HTS_Engine_clear(&m_HTS);
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

	{
		CFSAutoLock AutoLock(&m_Mutex);
		CFSString szPath = FSGetModulePath();
		try {
			m_Linguistic.Open(szPath + L"et.dct");
			m_Disambiguator.Open(szPath + L"et3.dct");
		}
		catch (...) {
			TRACE(L"ERROR opening morf files\n");
			return S_OK;
		}

		HTS_Engine_clear(&m_HTS);
		CFSAString szVoice = FSStrWtoA(szPath + GetTokenStringValue(pToken, L"VoiceFile"), FSCP_SYSTEM);
		char *pVoice = (char *)(LPCSTR)szVoice;
		if (!HTS_Engine_load(&m_HTS, &pVoice, 1)) {
			TRACE(L"ERROR loading HTS voice file\n");
			return S_OK;
		}
		HTS_Engine_set_sampling_frequency(&m_HTS, (size_t)48000);

		/*  todo: Move some to attributes, other hardcode. Noture which ones.
		HTS_Engine_set_phoneme_alignment_flag(&m_HTS, FALSE);
		HTS_Engine_set_fperiod(&m_HTS, (size_t)240);
		HTS_Engine_set_alpha(&m_HTS, 0.55);
		HTS_Engine_set_speed(&m_HTS, 1.2);*/

		m_bInitialized = TRUE;
		return S_OK;
	}
}

#define CHECK_ACTIONS { \
	DWORD dwActions = pOutputSite->GetActions(); \
	if (dwActions & SPVES_ABORT) { \
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

	HRESULT hr;
	ULONGLONG ullAudioOffset = 0;

	CFSClassArray<CFragment> Fragments;

	for (; pTextFrag; pTextFrag = pTextFrag->pNext) {

		CFSString szText;
		if (pTextFrag->pTextStart) szText = CFSString(pTextFrag->pTextStart, pTextFrag->ulTextLen);
		switch (pTextFrag->State.eAction) {
		case SPVA_Speak:
		case SPVA_SpellOut: {
			TRACE(L"- %s '%s'\n", (pTextFrag->State.eAction  == SPVA_Speak ? L"SPVA_Speak" : L"SPVA_SpellOut"), szText);
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
				try {
					CFSAutoLock AutoLock(&m_Mutex);
					Fragment.m_Morph.m_MorphInfo = m_Linguistic.Analyze(Fragment.m_Morph.m_szWord);
				}
				catch (...) {
					TRACE(L"ERROR while morphanalyzing text\n");
					return E_FAIL;
				}
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
			TRACE(L"- SPVA_Bookmark '%s'\n", szText);
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
		try {
			CFSAutoLock AutoLock(&m_Mutex);
			Disambiguated = m_Disambiguator.Disambiguate(Analysis);
		}
		catch (...) {
			TRACE(L"ERROR while disambiguating text\n");
			return E_FAIL;
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

		{
			CFSAutoLock AutoLock(&m_Mutex);
			double fVolume = (m_iBaseVolume ? 20.0 * log10(0.01 * m_iBaseVolume) : -144.0);
			HTS_Engine_set_volume(&m_HTS, fVolume);
			double fSpeed = pow(1.1, m_iBaseRate);
			HTS_Engine_set_speed(&m_HTS, fSpeed);
		}
		if (!::CreateAudio(m_HTS, m_Mutex, Sentence)) return E_FAIL;

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
				ULONG ulWritten;
				hr = pOutputSite->Write(Fragment.m_Audio.GetData(), Fragment.m_Audio.GetSize(), &ulWritten);
				ullAudioOffset += ulWritten;
				if (FAILED(hr)) return hr;
			} break;
			case SPVA_Silence: {
				ULONG ulWritten;
				hr = OutputSilence(pFormat, Fragment.m_lParam, pOutputSite, &ulWritten);
				ullAudioOffset += ulWritten;
				if (FAILED(hr)) return hr;
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

STDMETHODIMP CSapi5Engine::GetOutputFormat(const GUID *pTargetFormatId, const WAVEFORMATEX *pTargetFormat,
	GUID *pFormatId, WAVEFORMATEX **ppFormat)
{
	TRACE(L"CSapi5Engine::GetOutputFormat\n");
	return SpConvertStreamFormatEnum(SPSF_48kHz16BitMono, pFormatId, ppFormat);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

CFSWString CSapi5Engine::GetTokenStringValue(ISpObjectToken *pToken, const CFSString &szKey)
{
	CFSString result;
	if (!pToken) return result;

	LPWSTR pwValue = NULL;
	HRESULT hr = pToken->GetStringValue(szKey, &pwValue);
	if (FAILED(hr)) return result;

	result = pwValue;
	CoTaskMemFree(pwValue);
	return result;
}

BOOL CSapi5Engine::GetEventInterest(ISpTTSEngineSite *pOutputSite, SPEVENTENUM EventId)
{
	ULONGLONG ullEventInterest = 0;
	HRESULT hr = pOutputSite->GetEventInterest(&ullEventInterest);
	if (FAILED(hr)) return FALSE;
	return !!(ullEventInterest & SPFEI(EventId));
}

/*!!!
HRESULT CSapi5Engine::OutputAudio(const CFragment &Fragment, ISpTTSEngineSite *pOutputSite, ULONG *pWritten)
{
	*pWritten = 0;

	if (!Fragment.m_HTSInstructions.GetSize()) return S_OK;
	char **pLineArray = new char*[Fragment.m_HTSInstructions.GetSize()];
	for (INTPTR ip = 0; ip < Fragment.m_HTSInstructions.GetSize(); ip++) {
		pLineArray[ip] = (char *)(const char *)Fragment.m_HTSInstructions[ip];
	}

	CFSData Data;
	{
		CFSAutoLock AutoLock(&m_Mutex);
		HTS_Engine_refresh(&m_HTS);

		double fVolume = (100.0 - (m_iBaseVolume * Fragment.m_iVolume / 100.0)) * -0.1;
		HTS_Engine_set_volume(&m_HTS, fVolume);

		long lRate = FSMINMAX(m_iBaseRate + Fragment.m_iRate, -10, 10);
		double fSpeed = pow(1.08, lRate);
		HTS_Engine_set_speed(&m_HTS, fSpeed);

		HTS_Boolean bResult = HTS_Engine_synthesize_from_strings(&m_HTS, pLineArray, Fragment.m_HTSInstructions.GetSize());
		delete[] pLineArray;
		if (!bResult) return E_FAIL;

		INTPTR ipSpeechSize = HTS_Engine_get_nsamples(&m_HTS);
		Data.SetSize(ipSpeechSize * sizeof(short));
		short *pDataBuf = (short *)Data.GetData();
		for (INTPTR ip = 0; ip < ipSpeechSize; ip++) {
			double fValue = HTS_Engine_get_generated_speech(&m_HTS, ip);
			short sValue;
			if (fValue >= 32767.0) sValue = 32767;
			else if (fValue <= -32768.0) sValue = -32768;
			else sValue = (short)round(fValue);
			pDataBuf[ip] = sValue;
		}
	}

	TRACE(L"Speaking '%s'\n", Fragment.m_szText);
	return pOutputSite->Write(Data.GetData(), Data.GetSize(), pWritten);
}
*/

HRESULT CSapi5Engine::OutputSilence(const WAVEFORMATEX *pFormat, UINT uiMSec, ISpTTSEngineSite *pOutputSite, ULONG *pWritten)
{
	*pWritten = 0;
	UINT uiSampleSize = pFormat->nChannels * pFormat->wBitsPerSample / 8;
	INTPTR ui100 = pFormat->nSamplesPerSec / 10 * uiSampleSize;

	CFSData Data;
	Data.SetSize(ui100);
	memset(Data.GetData(), 0, Data.GetSize());

	while (uiMSec > 0) {
		ULONG ulWrite;
		if (uiMSec >= 100) {
			ulWrite = ui100;
			uiMSec -= 100;
		}
		else {
			ulWrite = pFormat->nSamplesPerSec * uiMSec / 1000 * uiSampleSize;
			uiMSec = 0;
		}
		ULONG ulWritten = 0;
		HRESULT hr = pOutputSite->Write(Data.GetData(), ulWrite, &ulWritten);
		*pWritten += ulWritten;
		if (FAILED(hr)) return hr;
	}
	return S_OK;
}
