# Eesti keele kõnesüntees (SAPI5)

**Eesti keele HTS-kõnesüntees**
  Copyright (c) 2015-16 [Eesti Keele Instituut](http://www.eki.ee/)  
  Indrek Kiissel

**SAPI 5 liides**
  Copyright (c) 2017 [Ränivägi OÜ](http://www.ränivägi.ee/)  
  Rene Prillop
	
**HTS Engine**
  [HTS Working Group](http://hts-engine.sourceforge.net/)  

All rights reserved.

Redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following
conditions are met:
- Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials provided
with the distribution.
- Neither the name of the HTS working group nor the names of its
contributors may be used to endorse or promote products derived
from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

**Morfoloogiline analüsaator ja ühestaja [vabamorf](https://github.com/Filosoft/vabamorf)**
  Copyright (c) 2015 [Filosoft](http://www.filosoft.ee/)

## Vajalik
- Visual Studio 2017
- WIX Toolkit 3.11 (installeri jaoks)

## Kompileerimiseks
- Ava sapi5et.sln
- Kompileeri sapi5et projekt
- Kopeeri dct kataloogis olevad failid sapi5et.dll-iga samasse kataloogi
- Registreeri valminud komponent sapi5et.dll administraatori õigustes (regsvr32 sapi5et.dll)
- Installeri loomiseks kompileeri sapi5et projekti kompileerimise järel projekt setup
