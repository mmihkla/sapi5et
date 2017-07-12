#pragma once

#include "lib/proof/proof.h"


class CFragment {
public:
	enum FRAGMENTTYPE {
		TYPE_UNKNOWN,
		TYPE_WORD,
		TYPE_PUNCTUATION,
		TYPE_SYMBOL,
		TYPE_SPACE,
		TYPE_OTHER
	};

	CFragment() :
		m_eType(TYPE_UNKNOWN),
		m_lParam(0),
		m_iRate(0),
		m_iVolume(100),
		m_ipStartPos(0)
	{}

	CFragment(INTPTR ipStartPos, const CFSWString &szText, FRAGMENTTYPE eType) :
		m_eType(eType),
		m_lParam(0),
		m_iRate(0),
		m_iVolume(100),
		m_ipStartPos(ipStartPos),
		m_szText(szText)
	{}

	BOOL IsSpeakAction() const {
		return (m_eAction == SPVA_Speak || m_eAction == SPVA_SpellOut);
	}

	SPVACTIONS m_eAction;
	FRAGMENTTYPE m_eType;

	LPARAM m_lParam;
	int m_iRate;
	int m_iVolume;

	INTPTR m_ipStartPos;
	CFSWString m_szText;
	CMorphInfos m_Morph;

	CFSData m_Audio;
};