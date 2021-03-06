#include "htssyn.h"

namespace htssyn {

	wchar_t replace_fchar(wchar_t c) {
		if (c == L'À') return L'a';
		if (c == L'Á') return L'a';
		if (c == L'Â') return L'a';
		if (c == L'Ã') return L'a';
		if (c == L'à') return L'a';
		if (c == L'á') return L'a';
		if (c == L'â') return L'a';
		if (c == L'ã') return L'a';
		if (c == L'È') return L'e';
		if (c == L'É') return L'e';
		if (c == L'Ê') return L'e';
		if (c == L'Ë') return L'e';
		if (c == L'è') return L'e';
		if (c == L'é') return L'e';
		if (c == L'ê') return L'e';
		if (c == L'ë') return L'e';
		if (c == L'Ì') return L'i';
		if (c == L'Í') return L'i';
		if (c == L'Î') return L'i';
		if (c == L'Ï') return L'i';
		if (c == L'Ÿ') return L'i';
		if (c == L'Ý') return L'i';
		if (c == L'ì') return L'i';
		if (c == L'í') return L'i';
		if (c == L'î') return L'i';
		if (c == L'ï') return L'i';
		if (c == L'ÿ') return L'ü';
		if (c == L'ý') return L'ü';
		if (c == L'Ò') return L'o';
		if (c == L'Ó') return L'o';
		if (c == L'Ô') return L'o';
		if (c == L'Å') return L'o';
		if (c == L'ò') return L'o';
		if (c == L'ó') return L'o';
		if (c == L'ô') return L'o';
		if (c == L'å') return L'o';
		if (c == L'Ù') return L'u';
		if (c == L'Ú') return L'u';
		if (c == L'Û') return L'u';
		if (c == L'ù') return L'u';
		if (c == L'ú') return L'u';
		if (c == L'û') return L'u';
		if (c == L'æ') return L'ä';
		if (c == L'Æ') return L'ä';
		if (c == L'Ø') return L'ö';
		if (c == L'Œ') return L'ö';
		if (c == L'ø') return L'ö';
		if (c == L'œ') return L'ö';
		if (c == L'ñ') return L'n';
		if (c == L'Ñ') return L'n';
		if (c == L'ç') return L's';
		if (c == L'Ç') return L's';
		if (c == L'ß') return L's';
		if (c == L'Ð') return L'k';
		if (c == L'Þ') return L'k';
		if (c == L'ð') return L'k';
		if (c == L'þ') return L'k';
		return 0;
	}

	CFSWString replace_abbreviation(CFSWString s) {
		// Lühendid EKRst
		if (s.EndsWith(L".")) s.Delete(s.GetLength() - 1, 1);
		if (s == L"aj") return L"ajutine";
		if (s == L"ak") return L"arvelduskonto";
		if (s == L"apr") return L"aprill";
		if (s == L"AS") return L"aktsiaselts";
		if (s == L"aug") return L"august";
		if (s == L"aü") return L"ametiühing";
		if (s == L"dets") return L"detsember";
		if (s == L"dl") return L"dessertlusikatäis";
		if (s == L"dots") return L"dotsent";
		if (s == L"dr") return L"doktor";
		if (s == L"eKr") return L"enne Kristuse sündi";
		if (s == L"EL") return L"Euroopa Liit";
		if (s == L"e.m.a") return L"enne meie ajaarvamist";
		//    if (s == L"end") return L"endine";
		if (s == L"FIE") return L"füüsilisest isikust ettevõtja";
		if (s == L"hr") return L"härra";
		if (s == L"hrl") return L"harilikult";
		if (s == L"ik") return L"isikukood";
		if (s == L"ins") return L"insener";
		if (s == L"it") return L"infotehnoloogia";
		if (s == L"IT") return L"infotehnoloogia";
		if (s == L"j.a") return L"juures asuv";
		if (s == L"jaan") return L"jaanuar";
		if (s == L"jj") return L"ja järgmine";
		if (s == L"jm") return L"ja muud";
		if (s == L"jms") return L"ja muud sellised";
		if (s == L"jmt") return L"ja mitmed teised";
		if (s == L"jn") return L"joonis";
		if (s == L"jne") return L"ja nii edasi";
		if (s == L"jpt") return L"ja paljud teised";
		if (s == L"jr") return L"juunior";
		if (s == L"jrk") return L"järjekord";
		if (s == L"jsk") return L"jaoskond";
		if (s == L"jt") return L"ja teised";
		if (s == L"juh") return L"juhataja";
		if (s == L"jun") return L"juunior";
		if (s == L"jv") return L"järv";
		if (s == L"k.a") return L"kaasa arvatud";
		if (s == L"kd") return L"köide";
		if (s == L"khk") return L"kihelkond";
		if (s == L"kk") return L"keskkool";
		if (s == L"kl") return L"kell";
		if (s == L"knd") return L"kandidaat";
		if (s == L"kod") return L"kodanik";
		if (s == L"kpl") return L"kauplus";
		if (s == L"kr") return L"kroon";
		if (s == L"kt") return L"kohusetäitja";
		if (s == L"kub") return L"kubermang";
		if (s == L"kv") return L"kvartal";
		if (s == L"KÜ") return L"korteriühistu";
		if (s == L"lg") return L"lõige";
		if (s == L"lj") return L"linnajagu";
		if (s == L"lk") return L"lehekülg";
		if (s == L"LK") return L"looduskaitse all";
		if (s == L"lm") return L"liidumaa";
		if (s == L"lo") return L"linnaosa";
		if (s == L"lp") return L"lugupeetud";
		if (s == L"lüh") return L"lühemalt";
		//if (s == L"M") return L"meestele";
		if (s == L"mag") return L"magister";
		if (s == L"m.a.j") return L"meie ajaarvamise järgi";
		if (s == L"min") return L"minut";
		if (s == L"mk") return L"maakond";
		if (s == L"mld") return L"miljard";
		if (s == L"mln") return L"miljon";
		if (s == L"mnt") return L"maantee";
		if (s == L"mob") return L"mobiiltelefon";
		if (s == L"ms") return L"muuseas";
		if (s == L"MTÜ") return L"mittetulundusühing";
		if (s == L"nim") return L"nimeline";
		if (s == L"nn") return L"niinimetatud";
		if (s == L"nov") return L"november";
		if (s == L"nr") return L"number";
		if (s == L"nt") return L"näiteks";
		if (s == L"nö") return L"nii öelda";
		if (s == L"okt") return L"oktoober";
		if (s == L"osk") return L"osakond";
		if (s == L"OÜ") return L"osaühing";
		if (s == L"pa") return L"poolaasta";
		if (s == L"pk") return L"postkast";
		if (s == L"pKr") return L"pärast Kristuse sündi";
		if (s == L"pms") return L"peamiselt";
		if (s == L"p.o") return L"peab olema";
		if (s == L"pr") return L"proua";
		if (s == L"prl") return L"preili";
		if (s == L"prof") return L"professor";
		if (s == L"ps") return L"poolsaar";
		if (s == L"pst") return L"puiestee";
		if (s == L"ptk") return L"peatükk";
		if (s == L"raj") return L"rajoon";
		if (s == L"rbl") return L"rubla";
		if (s == L"RE") return L"riigiettevõte";
		if (s == L"reg") return L"registreerimis";
		if (s == L"rg") return L"registri";
		if (s == L"rmtk") return L"raamatukogu";
		if (s == L"rkl") return L"riigikoguliige";
		if (s == L"rtj") return L"raudteejaam";
		if (s == L"SA") return L"sihtasutus";
		if (s == L"s.a") return L"sel aastal";
		if (s == L"saj") return L"sajand";
		if (s == L"sh") return L"sealhulgas";
		if (s == L"sen") return L"seenior";
		if (s == L"sept") return L"september";
		if (s == L"skp") return L"selle kuu päeval";
		if (s == L"spl") return L"supilusikatäis";
		if (s == L"sl") return L"supilusikatäis";
		if (s == L"sm") return L"seltsimees";
		if (s == L"s.o") return L"see on";
		if (s == L"s.t") return L"see tähendab";
		if (s == L"st") return L"see tähendab";
		if (s == L"stj") return L"saatja";
		if (s == L"srn") return L"surnud";
		if (s == L"sü") return L"säilitusüksus";
		if (s == L"snd") return L"sündinud";
		if (s == L"tehn") return L"tehniline";
		if (s == L"tel") return L"telefon";
		if (s == L"tk") return L"tükk(i)";
		if (s == L"tl") return L"teelusikatäis";
		if (s == L"tlk") return L"tõlkija";
		if (s == L"Tln") return L"Tallinn";
		if (s == L"tn") return L"tänav";
		if (s == L"tr") return L"trükk";
		if (s == L"Trt") return L"Tartu";
		//if (s == L"u") return L"umbes";
		if (s == L"ukj") return L"uue kalendri järgi";
		if (s == L"UÜ") return L"usaldusühing";
		if (s == L"v.a") return L"välja arvatud";
		if (s == L"van") return L"vananenud";
		if (s == L"VE") return L"väikeettevõte";
		if (s == L"veebr") return L"veebruar";
		if (s == L"vkj") return L"vana kalendri järgi";
		if (s == L"vm") return L"või muu(d)";
		if (s == L"vms") return L"või muu seesugune";
		if (s == L"vrd") return L"võrdle";
		if (s == L"vt") return L"vaata";
		if (s == L"õa") return L"õppeaasta";
		if (s == L"õp") return L"õpetaja";
		if (s == L"õpil") return L"õpilane";
		if (s == L"ÄÜ") return L"äriühing";
		if (s == L"ÜE") return L"ühisettevõte";
		if (s == L"SEK") return L"Rootsi krooni";
		if (s == L"NOK") return L"Norra krooni";
		if (s == L"RUR") return L"Vene rubla";
		if (s == L"USD") return L"USA dollarit";
		if (s == L"GBP") return L"Inglise naela";
		if (s == L"LVL") return L"Läti latti";
		if (s == L"LTL") return L"Leedu litti";
		if (s == L"EEK") return L"Eesti krooni";
		return CFSWString();
	}

	/*
	by fun2code. http://www.cplusplus.com/forum/lounge/74394/
	*/
	CFSWString int_to_words(CFSWString numStr) {
		CFSWString rs;

		CFSWString onesName[] = {
			L"üks", L"kaks", L"kolm", L"neli", L"viis", L"kuus", L"seitse", L"kaheksa", L"üheksa"
		};

		CFSWString teensName[] = {
			L"kümme", L"üksteist", L"kaksteist", L"kolmteist", L"neliteist", L"viisteist", L"kuusteist", L"seitseteist", L"kaheksateist", L"üheksateist"
		};

		CFSWString tensName[] = {
			L"kakskümmend", L"kolmkümmend", L"nelikümmend", L"viiskümmend", L"kuuskümmend", L"seitsekümmend", L"kaheksakümmend", L"üheksakümmend"
		};

		CFSWString onesNameO[] = {
			L"esimene", L"teine", L"kolmas", L"neljas", L"viies", L"kuues", L"seitsmes", L"kaheksas", L"üheksas",
		};

		CFSWString teensNameO[] = {
			L"kümmes", L"üheteistkümnes", L"kaheteistkümnes", L"kolmeteistkümnes", L"neljateistkümnes",
			L"viieteistkümnes", L"kuueteistkümnes", L"seitsmeteistkümnes", L"kaheksateistkümnes", L"üheksateistkümnes"
		};

		CFSWString tensNameO[] = {
			L"kahekümne", L"kolmekümne", L"neljakümne", L"viiekümne", L"kuuekümne", L"seitsmekümne", L"kaheksakümne", L"üheksakümne"
		};


		CFSWString illion_preName[] = { L"m", L"b", L"tr", L"kvadr", L"kvint", L"sekst", L"sept", L"okt", L"non", L"dets" };
		CFSWString decillion_preName[] = { L"un", L"duo", L"tre", L"kvattuor", L"kvin", L"seks", L"septen", L"okto", L"novem" };

		while (numStr[0] == L'0') {
			rs += L"null ";
			numStr.Delete(0, 1);
		}
		if (numStr.GetLength() == 0) {
			numStr.Trim();
			return rs;
		}

		while (numStr.GetLength() % 3 != 0)
			numStr = L'0' + numStr;

		for (INTPTR i = 0; i < numStr.GetLength(); i += 3) {
			if (numStr[i] == L'0' && numStr[i + 1] == L'0' && numStr[i + 2] == L'0')
				continue;

			if (numStr[i] > L'0') {
				rs += onesName[numStr[i] - 48 - 1] + L"sada ";
			}

			if (numStr[i + 1] == L'0' || numStr[i + 1] > L'1') {
				if (numStr[i + 1] > L'1') {
					rs += tensName[numStr[i + 1] - 48 - 2] + L" ";
				}
				if (numStr[i + 2] > L'0') {
					rs += onesName[numStr[i + 2] - 48 - 1] + L" ";
				}
			}
			else {
				rs += teensName[numStr[i + 2] - 48] + L" ";
			}

			// naming each factor of 1,000
			unsigned int j = (numStr.GetLength() - i) / 3;
			if (j == 2) {
				rs += L"tuhat ";
			}
			else if (j > 2) {

				if (j <= 12) {
					rs += illion_preName[j - 3];
				}
				else if (j <= 21) {
					rs += decillion_preName[j - 13] + L"dets";
				}
				else if (j == 22) {
					rs += L"vigint";
				}
				rs += L"iljon ";
			}
		}

		rs.Trim();
		return rs;

	}

}
