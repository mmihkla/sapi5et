#include "stdafx.h"
#include "audio.h"

// helper class to be adjusted to synthesis needs
class CAudioFragment {
public:
	CAudioFragment(INTPTR ipWeight, CFragment *pFragment) :
		m_ipWeight(ipWeight),
		m_pFragment(pFragment)
	{ }

	INTPTR m_ipWeight;
	CFragment *m_pFragment;
};

BOOL CreateLabels(CFSClassArray<CFragment> &Sentence, CFSAStringArray &Labels, CFSArray<CAudioFragment> &AudioFragments)
{
	for (INTPTR ipFrag = 0; ipFrag < Sentence.GetSize(); ipFrag++) {
		CFragment &Fragment = Sentence[ipFrag];
		if (!Fragment.IsSpeakAction()) continue;

		if (Fragment.m_eAction == SPVA_Speak && Fragment.m_eType == CFragment::TYPE_WORD) {
			INTPTR ipWeight = 3;
			if (Fragment.m_szText == L"mees") {
				Labels.AddItem("x^x-pau+m=e@x-x/A:0_0/B:x-x@x-x&x-x/C:0+0/D:0_0/E:0+x@x+x/F:0_0/G:0_0/H:x=x^1=1/I:0=0/J:5+3-1");
				Labels.AddItem("x^pau-m+e=ew@1-4/A:0_0/B:2-4@1-1&1-5/C:2+2/D:0_0/E:a+1@1+3/F:v_2/G:0_0/H:5=3^1=1/I:0=0/J:5+3-1");
				Labels.AddItem("pau^m-e+ew=s@2-3/A:0_0/B:2-4@1-1&1-5/C:2+2/D:0_0/E:a+1@1+3/F:v_2/G:0_0/H:5=3^1=1/I:0=0/J:5+3-1");
				Labels.AddItem("m^e-ew+s=t@3-2/A:0_0/B:2-4@1-1&1-5/C:2+2/D:0_0/E:a+1@1+3/F:v_2/G:0_0/H:5=3^1=1/I:0=0/J:5+3-1");
				Labels.AddItem("e^ew-s+t=u@4-1/A:0_0/B:2-4@1-1&1-5/C:2+2/D:0_0/E:a+1@1+3/F:v_2/G:0_0/H:5=3^1=1/I:0=0/J:5+3-1");
				Labels.AddItem("ew^s-t+u=l@1-2/A:2_4/B:2-2@1-2&2-4/C:0+2/D:a_1/E:v+2@2+2/F:a_2/G:0_0/H:5=3^1=1/I:0=0/J:5+3-1");
				ipWeight = 4;
			}
			else if (Fragment.m_szText == L"tuli") {
				Labels.AddItem("s^t-u+l=i@2-1/A:2_4/B:2-2@1-2&2-4/C:0+2/D:a_1/E:v+2@2+2/F:a_2/G:0_0/H:5=3^1=1/I:0=0/J:5+3-1");
				Labels.AddItem("t^u-l+i=p@1-2/A:2_2/B:0-2@2-1&3-3/C:2+3/D:a_1/E:v+2@2+2/F:a_2/G:0_0/H:5=3^1=1/I:0=0/J:5+3-1");
				Labels.AddItem("u^l-i+p=o@2-1/A:2_2/B:0-2@2-1&3-3/C:2+3/D:a_1/E:v+2@2+2/F:a_2/G:0_0/H:5=3^1=1/I:0=0/J:5+3-1");
				Labels.AddItem("l^i-p+o=ow@1-3/A:0_2/B:2-3@1-2&4-2/C:0+2/D:v_2/E:a+2@3+1/F:0_0/G:0_0/H:5=3^1=1/I:0=0/J:5+3-1");
				ipWeight = 4;
			}
			else if (Fragment.m_szText == L"poodi") {
				Labels.AddItem("i^p-o+ow=D@2-2/A:0_2/B:2-3@1-2&4-2/C:0+2/D:v_2/E:a+2@3+1/F:0_0/G:0_0/H:5=3^1=1/I:0=0/J:5+3-1");
				Labels.AddItem("p^o-ow+D=i@3-1/A:0_2/B:2-3@1-2&4-2/C:0+2/D:v_2/E:a+2@3+1/F:0_0/G:0_0/H:5=3^1=1/I:0=0/J:5+3-1");
				Labels.AddItem("o^ow-D+i=pau@1-2/A:2_3/B:0-2@2-1&5-1/C:0+0/D:v_2/E:a+2@3+1/F:0_0/G:0_0/H:5=3^1=1/I:0=0/J:5+3-1");
				Labels.AddItem("ow^D-i+pau=x@2-1/A:2_3/B:0-2@2-1&5-1/C:0+0/D:v_2/E:a+2@3+1/F:0_0/G:0_0/H:5=3^1=1/I:0=0/J:5+3-1");
				Labels.AddItem("D^i-pau+x=x@x-x/A:0_0/B:x-x@x-x&x-x/C:0+0/D:0_0/E:0+x@x+x/F:0_0/G:0_0/H:x=x^1=1/I:0=0/J:5+3-1");
				ipWeight = 5;
			}
			else {
				Labels.AddItem("x^x-j+uh=h@1_2/A:0_0_0/B:0-0-2@1+1&1-12#|0/C:0+0+4/D:0_0/E:content+1@1+10&/F:content_1/G:0_0/H:12=10@1=1|L-L%/I:0=0/J:12+10-1");
				Labels.AddItem("x^j-uh+h=a@2_1/A:0_0_0/B:0-0-2@1+1&1-12#|0/C:0+0+4/D:0_0/E:content+1@1+10&/F:content_1/G:0_0/H:12=10@1=1|L-L%/I:0=0/J:12+10-1");
				Labels.AddItem("j^uh-h+a=v@1_4/A:0_0_2/B:0-0-4@1+1&2-11#|0/C:0+0+8/D:content_1/E:content+1@2+9&/F:content_1/G:0_0/H:12=10@1=1|L-L%/I:0=0/J:12+10-1");

			}
			AudioFragments.AddItem(CAudioFragment(ipWeight, &Fragment));
		}
		else if (Fragment.m_eAction == SPVA_SpellOut && Fragment.m_eType == CFragment::TYPE_WORD) {
			// Remove word and insert letters
			CFragment OldFragment = Fragment;
			Sentence.RemoveItem(ipFrag); ipFrag--;

			for (INTPTR ip = 0; ip < OldFragment.m_szText.GetLength(); ip++) {
				// todo: remove hidden symbols!
				CFragment NewFragment(OldFragment.m_ipStartPos + ip, CFSWString(OldFragment.m_szText[ip]), CFragment::TYPE_WORD);
				NewFragment.m_eAction = OldFragment.m_eAction;
				NewFragment.m_iVolume = OldFragment.m_iVolume;
				NewFragment.m_iRate = OldFragment.m_iRate;

				Sentence.InsertItem(ipFrag, NewFragment);
				Labels.AddItem("x^x-j+uh=h@1_2/A:0_0_0/B:0-0-2@1+1&1-12#|0/C:0+0+4/D:0_0/E:content+1@1+10&/F:content_1/G:0_0/H:12=10@1=1|L-L%/I:0=0/J:12+10-1");
				Labels.AddItem("x^j-uh+h=a@2_1/A:0_0_0/B:0-0-2@1+1&1-12#|0/C:0+0+4/D:0_0/E:content+1@1+10&/F:content_1/G:0_0/H:12=10@1=1|L-L%/I:0=0/J:12+10-1");
				Labels.AddItem("j^uh-h+a=v@1_4/A:0_0_2/B:0-0-4@1+1&2-11#|0/C:0+0+8/D:content_1/E:content+1@2+9&/F:content_1/G:0_0/H:12=10@1=1|L-L%/I:0=0/J:12+10-1");
				AudioFragments.AddItem(CAudioFragment(3, &Sentence[ipFrag]));
				ipFrag++;
			}
		}
		else if (Fragment.m_eType == CFragment::TYPE_SYMBOL) {
			Labels.AddItem("x^x-j+uh=h@1_2/A:0_0_0/B:0-0-2@1+1&1-12#|0/C:0+0+4/D:0_0/E:content+1@1+10&/F:content_1/G:0_0/H:12=10@1=1|L-L%/I:0=0/J:12+10-1");
			Labels.AddItem("x^j-uh+h=a@2_1/A:0_0_0/B:0-0-2@1+1&1-12#|0/C:0+0+4/D:0_0/E:content+1@1+10&/F:content_1/G:0_0/H:12=10@1=1|L-L%/I:0=0/J:12+10-1");
			Labels.AddItem("j^uh-h+a=v@1_4/A:0_0_2/B:0-0-4@1+1&2-11#|0/C:0+0+8/D:content_1/E:content+1@2+9&/F:content_1/G:0_0/H:12=10@1=1|L-L%/I:0=0/J:12+10-1");
			AudioFragments.AddItem(CAudioFragment(3, &Fragment));
		}
		else if (Fragment.m_eAction == SPVA_SpellOut && Fragment.m_eType == CFragment::TYPE_PUNCTUATION) {
			Labels.AddItem("x^x-j+uh=h@1_2/A:0_0_0/B:0-0-2@1+1&1-12#|0/C:0+0+4/D:0_0/E:content+1@1+10&/F:content_1/G:0_0/H:12=10@1=1|L-L%/I:0=0/J:12+10-1");
			Labels.AddItem("x^j-uh+h=a@2_1/A:0_0_0/B:0-0-2@1+1&1-12#|0/C:0+0+4/D:0_0/E:content+1@1+10&/F:content_1/G:0_0/H:12=10@1=1|L-L%/I:0=0/J:12+10-1");
			Labels.AddItem("j^uh-h+a=v@1_4/A:0_0_2/B:0-0-4@1+1&2-11#|0/C:0+0+8/D:content_1/E:content+1@2+9&/F:content_1/G:0_0/H:12=10@1=1|L-L%/I:0=0/J:12+10-1");
			AudioFragments.AddItem(CAudioFragment(3, &Fragment));
		}
	}

	return TRUE;
}

BOOL CreateAudio(HTS_Engine &HTS, CFSMutex &Mutex, CFSClassArray<CFragment> &Sentence)
{
	CFSArray<CAudioFragment> AudioFragments;
	CFSAStringArray Labels;

	// Calculate weights and create HTS labels
	if (!CreateLabels(Sentence, Labels, AudioFragments)) return false;
	if (!Labels.GetSize()) return TRUE;

	// Synthesize audio
	char **pLabelArray = new char*[Labels.GetSize()];
	for (INTPTR ip = 0; ip < Labels.GetSize(); ip++) {
		pLabelArray[ip] = (char *)(const char *)Labels[ip];
	}

	CFSData Audio;
	short *pAudioBuf = NULL;
	INTPTR ipSamples = 0;
	{
		CFSAutoLock AutoLock(&Mutex);

		HTS_Boolean bResult = HTS_Engine_synthesize_from_strings(&HTS, pLabelArray, Labels.GetSize());
		delete[] pLabelArray;
		if (!bResult) return E_FAIL;

		ipSamples = HTS_Engine_get_nsamples(&HTS);
		Audio.SetSize(ipSamples * sizeof(short));
		pAudioBuf = (short *)Audio.GetData();
		for (INTPTR ip = 0; ip < ipSamples; ip++) {
			double fValue = HTS_Engine_get_generated_speech(&HTS, ip);
			short sValue;
			if (fValue >= 32767.0) sValue = 32767;
			else if (fValue <= -32768.0) sValue = -32768;
			else sValue = (short)round(fValue);
			pAudioBuf[ip] = sValue;
		}
	}

	// Split autio to fragments
	INTPTR ipTotalWeight = 0;
	for (INTPTR ip = 0; ip < AudioFragments.GetSize(); ip++) {
		ipTotalWeight += AudioFragments[ip].m_ipWeight;
	}

	INTPTR ipWeightOffset = 0;
	for (INTPTR ip = 0; ip < AudioFragments.GetSize(); ip++) {
		INTPTR ipStartSample = ipWeightOffset * ipSamples / ipTotalWeight;
		INTPTR ipEndSample = (ipWeightOffset + AudioFragments[ip].m_ipWeight) * ipSamples / ipTotalWeight;
		INTPTR ipSampleCount = ipEndSample - ipStartSample;
		if (!ipSampleCount) continue;

		CFSData &FragmentAudio = AudioFragments[ip].m_pFragment->m_Audio;
		FragmentAudio.SetSize(ipSampleCount * sizeof(short));
		memcpy(FragmentAudio.GetData(), pAudioBuf + ipStartSample, ipSampleCount * sizeof(short));

		// Adjust fragment volume
		int iVolume = AudioFragments[ip].m_pFragment->m_iVolume;
		if (iVolume < 100) {
			short *pSamples = (short *)FragmentAudio.GetData();
			for (INTPTR ipSample = 0; ipSample < ipSampleCount; ipSample++) {
				pSamples[ipSample] = (short)(iVolume * pSamples[ipSample] / 100);
			}
		}

		ipWeightOffset += AudioFragments[ip].m_ipWeight;
	}

	return TRUE;
}
