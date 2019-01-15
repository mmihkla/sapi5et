#include "htssyn.h"

void CSyl::Process() {
	int k = 0;
	CFSWString Rareph[6] = { L"j", L"v", L"f", L"d", L"b", L"h" }; // harvad või puuduvad pikad foneemid kõnekorpuses, mis puuduvad süntesaatoris
	for (INTPTR i = 0; i < syl.GetLength(); i++) {
		CFSWString s = syl.Mid(i, 1);
		if (s == L'š' || s == L'ž') s = L"sh";
		if (s == L'õ') s = L"q";
		if (s == L'ä') s = L"x";
		if (s == L'ö') s = L"c";
		if (s == L'ü') s = L"y";

		if (s == L":") {
			// 1. nihutusvigade kaitseks (vt "piirkonda")
			// 2-3. kolmandas vältes v ja j on kõnebaasis sedavõrd haruldased,
			// et väljundis kuuleb nende asemel mingit r-i laadset hääikut.
			// Kellel on parem baas, kommenteerigu 2. ja 3. tingimus välja.            
			//if (phone_vector.GetSize() > 0 && phone_vector[phone_vector.GetSize() - 1].phone != L"j" && phone_vector[phone_vector.GetSize() - 1].phone != L"v") {
			//	phone_vector[phone_vector.GetSize() - 1].phone += L"w";
			if (phone_vector.GetSize() > 0){
				k = 0;
				for (INTPTR j = 0; j < 6; j++) {
					if (phone_vector[phone_vector.GetSize() - 1].phone == Rareph[j]) {
						k = 1;
						break;
					}
				}
				if (k == 0) phone_vector[phone_vector.GetSize() - 1].phone += L"w";
			}
		}
		else {
			CPhone p;
			p.phone = s;
			phone_vector.AddItem(p);
		}

	}
}
