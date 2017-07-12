#pragma once

#include "lib/hts/HTS_engine.h"
#include "fragment.h"

BOOL CreateAudio(HTS_Engine &HTS, CFSMutex &Mutex, CFSClassArray<CFragment> &Sentence);
