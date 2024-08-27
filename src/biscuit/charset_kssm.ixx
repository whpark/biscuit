module;

#include "biscuit/macro.h"

export module biscuit.charset_kssm;
import std;
import biscuit.aliases;

namespace biscuit::charset {
	/// @brief KSSM is always in Big Endian. if you want it to be little-endian, use this function
	/// @param v KSSM value
	/// @return little-endian KSSM Value (MSB : 1)
	export BSC__NODISCARD constexpr charKSSM_t SwapKSSMByteOrder(charKSSM_t v) {
		// KSSM is always Big Endian
		if constexpr (std::endian::native == std::endian::big) {
			return v;
		} else {
			return std::byteswap(v);
		}
	}

	namespace detail {
		BSC__NODISCARD constexpr charKSSM_t SwapOrNot(charKSSM_t c) {
			if constexpr (false) {
				return SwapKSSMByteOrder(c);
			}
			else {
				return c;
			}
		}
	}
}

export namespace biscuit::charset {

	//=========================================================================
	// https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=xsnake&logNo=100205421762
	// 
	//	상용조합테이블 from FLBP-8SR IBM 2Byte (F)
	//	----------------------------------------
	//	10진   2진        초성   중성   종성
	//	----------------------------------------
	//	 0   00000       불가   불가   불가
	//	 1   00001       채움   불가   채움
	//	 2   00010        ㄱ    채움    ㄱ
	//	 3   00011        ㄲ     ㅏ     ㄲ
	//	----------------------------------------
	//	 4   00100        ㄴ     ㅐ     ㄳ
	//	 5   00101        ㄷ     ㅑ     ㄴ
	//	 6   00110        ㄸ     ㅒ     ㄵ
	//	 7   00111        ㄹ     ㅓ     ㄶ
	//	----------------------------------------
	//	 8   01000        ㅁ    불가    ㄷ
	//	 9   01001        ㅂ    불가    ㄹ
	//	10   01010        ㅃ     ㅔ     ㄺ
	//	11   01011        ㅅ     ㅕ     ㄻ
	//	----------------------------------------
	//	12   01100        ㅆ     ㅖ     ㄼ
	//	13   01101        ㅇ     ㅗ     ㄽ
	//	14   01110        ㅈ     ㅘ     ㄾ
	//	15   01111        ㅉ     ㅙ     ㄿ
	//	----------------------------------------
	//	16   10000        ㅊ    불가    ㅀ
	//	17   10001        ㅋ    불가    ㅁ
	//	18   10010        ㅌ     ㅚ    <ㅭ>
	//	19   10011        ㅍ     ㅛ     ㅂ
	//	----------------------------------------
	//	20   10100        ㅎ     ㅜ     ㅄ
	//	21   10101       한자    ㅝ     ㅅ
	//	22   10110       한자    ㅞ     ㅆ
	//	23   10111       한자    ㅟ     ㅇ
	//	----------------------------------------
	//	24   11000       한자   불가    ㅈ
	//	25   11001       한자   불가    ㅊ
	//	26   11010       한자    ㅠ     ㅋ
	//	27   11011       한자    ㅡ     ㅌ
	//	----------------------------------------
	//	28   11100       한자    ㅢ     ㅍ
	//	29   11101       한자    ㅣ     ㅎ
	//	30   11110       한자   <ㆍ>   <ㆁ>
	//	31   11111       불가   <ㆎ>   불가
	//	----------------------------------------
	// 
	//=========================================================================

	//=============================================================================================================================================
	// https://ko.wikipedia.org/wiki/%ED%95%9C%EA%B8%80_%EC%83%81%EC%9A%A9_%EC%A1%B0%ED%95%A9%ED%98%95_%EC%9D%B8%EC%BD%94%EB%94%A9
	//	코드 	0 	1 	2 	3 	4 	5 	6 	7 	8 	9 	10 	11 	12 	13 	14 	15 	16 	17 	18 	19 	20 	21 	22 	23 	24 	25 	26 	27 	28 	29 	30 	31
	//	초성 	- 	F 	ㄱ 	ㄲ 	ㄴ 	ㄷ 	ㄸ 	ㄹ 	ㅁ 	ㅂ 	ㅃ 	ㅅ 	ㅆ 	ㅇ 	ㅈ 	ㅉ 	ㅊ 	ㅋ 	ㅌ 	ㅍ 	ㅎ 	- 	- 	- 	- 	- 	- 	- 	- 	- 	- 	-
	//	중성 	- 	- 	F 	ㅏ 	ㅐ 	ㅑ 	ㅒ 	ㅓ 	- 	- 	ㅔ 	ㅕ 	ㅖ 	ㅗ 	ㅘ 	ㅙ 	- 	- 	ㅚ 	ㅛ 	ㅜ 	ㅝ 	ㅞ 	ㅟ 	- 	- 	ㅠ 	ㅡ 	ㅢ 	ㅣ 	- 	-
	//	종성 	- 	F 	ㄱ 	ㄲ 	ㄳ 	ㄴ 	ㄵ 	ㄶ 	ㄷ 	ㄹ 	ㄺ 	ㄻ 	ㄼ 	ㄽ 	ㄾ 	ㄿ 	ㅀ 	ㅁ 	- 	ㅂ 	ㅄ 	ㅅ 	ㅆ 	ㅇ 	ㅈ 	ㅊ 	ㅋ 	ㅌ 	ㅍ 	ㅎ 	- 	- 
	//=============================================================================================================================================

	enum class eKSSM : charKSSM_t {
		maskChar	= detail::SwapOrNot(0b1'00000'00000'00000),	// MSB : 1 (Big Endian)
		maskIni		= detail::SwapOrNot(0b0'11111'00000'00000),	// 초성
		maskMid		= detail::SwapOrNot(0b0'00000'11111'00000),	// 중성
		maskFin		= detail::SwapOrNot(0b0'00000'00000'11111),	// 종성

		// 초성 (19+1)
		Ini_Fill	= detail::SwapOrNot( 1 << 10),	// 채움
		Ini_G		= detail::SwapOrNot( 2 << 10),	// ㄱ
		Ini_GG		= detail::SwapOrNot( 3 << 10),	// ㄲ
		Ini_N		= detail::SwapOrNot( 4 << 10),	// ㄴ
		Ini_D		= detail::SwapOrNot( 5 << 10),	// ㄷ
		Ini_DD		= detail::SwapOrNot( 6 << 10),	// ㄸ
		Ini_L		= detail::SwapOrNot( 7 << 10),	// ㄹ
		Ini_M		= detail::SwapOrNot( 8 << 10),	// ㅁ
		Ini_B		= detail::SwapOrNot( 9 << 10),	// ㅂ
		Ini_BB		= detail::SwapOrNot(10 << 10),	// ㅃ
		Ini_S		= detail::SwapOrNot(11 << 10),	// ㅅ
		Ini_SS		= detail::SwapOrNot(12 << 10),	// ㅆ
		Ini_O		= detail::SwapOrNot(13 << 10),	// ㅇ
		Ini_J		= detail::SwapOrNot(14 << 10),	// ㅈ
		Ini_JJ		= detail::SwapOrNot(15 << 10),	// ㅉ
		Ini_CH		= detail::SwapOrNot(16 << 10),	// ㅊ
		Ini_K		= detail::SwapOrNot(17 << 10),	// ㅋ
		Ini_T		= detail::SwapOrNot(18 << 10),	// ㅌ
		Ini_P		= detail::SwapOrNot(19 << 10),	// ㅍ
		Ini_H		= detail::SwapOrNot(20 << 10),	// ㅎ

		// 중성. (21+1) U : Up, D : Down, R : Right, L : Left, H : Horizontal bar, V : Vertical bar
		Mid_Fill	= detail::SwapOrNot( 2 <<  5),	// 채움		00010
		Mid_R		= detail::SwapOrNot( 3 <<  5),	// ㅏ		00011
		Mid_RV		= detail::SwapOrNot( 4 <<  5),	// ㅐ		00100
		Mid_RR		= detail::SwapOrNot( 5 <<  5),	// ㅑ		00101
		Mid_RRV		= detail::SwapOrNot( 6 <<  5),	// ㅒ		00110
		Mid_L		= detail::SwapOrNot( 7 <<  5),	// ㅓ		00111
		Mid_LV		= detail::SwapOrNot(10 <<  5),	// ㅔ		01010
		Mid_LL		= detail::SwapOrNot(11 <<  5),	// ㅕ		01011
		Mid_LLV		= detail::SwapOrNot(12 <<  5),	// ㅖ		01100
		Mid_U		= detail::SwapOrNot(13 <<  5),	// ㅗ		01101
		Mid_UR		= detail::SwapOrNot(14 <<  5),	// ㅘ		01110
		Mid_URV		= detail::SwapOrNot(15 <<  5),	// ㅙ		01111
		Mid_UV		= detail::SwapOrNot(18 <<  5),	// ㅚ		10010
		Mid_UU		= detail::SwapOrNot(19 <<  5),	// ㅛ		10011
		Mid_D		= detail::SwapOrNot(20 <<  5),	// ㅜ		10100
		Mid_DL		= detail::SwapOrNot(21 <<  5),	// ㅝ		10101
		Mid_DLV		= detail::SwapOrNot(22 <<  5),	// ㅞ		10110
		Mid_DV		= detail::SwapOrNot(23 <<  5),	// ㅟ		10111
		Mid_DD		= detail::SwapOrNot(26 <<  5),	// ㅠ		11010
		Mid_H		= detail::SwapOrNot(27 <<  5),	// ㅡ		11011
		Mid_HV		= detail::SwapOrNot(28 <<  5),	// ㅢ		11100
		Mid_V		= detail::SwapOrNot(29 <<  5),	// ㅣ		11101

		// 종성 (27+1)
		Fin_Fill	= detail::SwapOrNot( 1),		// 채움
		Fin_G		= detail::SwapOrNot( 2),		// ㄱ
		Fin_GG		= detail::SwapOrNot( 3),		// ㄲ
		Fin_GS		= detail::SwapOrNot( 4),		// ㄳ
		Fin_N		= detail::SwapOrNot( 5),		// ㄴ
		Fin_NJ		= detail::SwapOrNot( 6),		// ㄵ
		Fin_NH		= detail::SwapOrNot( 7),		// ㄶ
		Fin_D		= detail::SwapOrNot( 8),		// ㄷ
		Fin_L		= detail::SwapOrNot( 9),		// ㄹ
		Fin_LG		= detail::SwapOrNot(10),		// ㄺ
		Fin_LM		= detail::SwapOrNot(11),		// ㄻ
		Fin_LB		= detail::SwapOrNot(12),		// ㄼ
		Fin_LS		= detail::SwapOrNot(13),		// ㄽ
		Fin_LT		= detail::SwapOrNot(14),		// ㄾ
		Fin_LP		= detail::SwapOrNot(15),		// ㄿ
		Fin_LH		= detail::SwapOrNot(16),		// ㅀ
		Fin_M		= detail::SwapOrNot(17),		// ㅁ
		Fin_B		= detail::SwapOrNot(19),		// ㅂ
		Fin_BS		= detail::SwapOrNot(20),		// ㅄ
		Fin_S		= detail::SwapOrNot(21),		// ㅅ
		Fin_SS		= detail::SwapOrNot(22),		// ㅆ
		Fin_O		= detail::SwapOrNot(23),		// ㅇ
		Fin_J		= detail::SwapOrNot(24),		// ㅈ
		Fin_CH		= detail::SwapOrNot(25),		// ㅊ
		Fin_K		= detail::SwapOrNot(26),		// ㅋ
		Fin_T		= detail::SwapOrNot(27),		// ㅌ
		Fin_P		= detail::SwapOrNot(28),		// ㅍ
		Fin_H		= detail::SwapOrNot(29),		// ㅎ

	};
	std::map<wchar_t, eKSSM> const s_mapW2KSSM_Ini {
		// 초성
		{ L'\0', eKSSM::Ini_Fill	}, { L'ㄱ', eKSSM::Ini_G		}, { L'ㄲ', eKSSM::Ini_GG	}, { L'ㄴ', eKSSM::Ini_N		}, { L'ㄷ', eKSSM::Ini_D		},
		{ L'ㄸ', eKSSM::Ini_DD		}, { L'ㄹ', eKSSM::Ini_L		}, { L'ㅁ', eKSSM::Ini_M		}, { L'ㅂ', eKSSM::Ini_B		}, { L'ㅃ', eKSSM::Ini_BB	},
		{ L'ㅅ', eKSSM::Ini_S		}, { L'ㅆ', eKSSM::Ini_SS	}, { L'ㅇ', eKSSM::Ini_O		}, { L'ㅈ', eKSSM::Ini_J		}, { L'ㅉ', eKSSM::Ini_JJ	},
		{ L'ㅊ', eKSSM::Ini_CH		}, { L'ㅋ', eKSSM::Ini_K		}, { L'ㅌ', eKSSM::Ini_T		}, { L'ㅍ', eKSSM::Ini_P		}, { L'ㅎ', eKSSM::Ini_H		},
	};
	std::map<wchar_t, eKSSM> const s_mapW2KSSM_Mid {
		// 중성
		{ L'\0', eKSSM::Mid_Fill	}, { L'ㅏ', eKSSM::Mid_R		}, { L'ㅐ', eKSSM::Mid_RV	}, { L'ㅑ', eKSSM::Mid_RR	}, { L'ㅒ', eKSSM::Mid_RRV	},
		{ L'ㅓ', eKSSM::Mid_L		}, { L'ㅔ', eKSSM::Mid_LV	}, { L'ㅕ', eKSSM::Mid_LL	}, { L'ㅖ', eKSSM::Mid_LLV	}, { L'ㅗ', eKSSM::Mid_U		},
		{ L'ㅘ', eKSSM::Mid_UR		}, { L'ㅙ', eKSSM::Mid_URV	}, { L'ㅚ', eKSSM::Mid_UV	}, { L'ㅛ', eKSSM::Mid_UU	}, { L'ㅜ', eKSSM::Mid_D		},
		{ L'ㅝ', eKSSM::Mid_DL		}, { L'ㅞ', eKSSM::Mid_DLV	}, { L'ㅟ', eKSSM::Mid_DV	}, { L'ㅠ', eKSSM::Mid_DD	}, { L'ㅡ', eKSSM::Mid_H		},
		{ L'ㅢ', eKSSM::Mid_HV		}, { L'ㅣ', eKSSM::Mid_V		},
	};
	std::map<wchar_t, eKSSM> const s_mapW2KSSM_Fin {
		// 종성
		{ L'\0', eKSSM::Fin_Fill	}, { L'ㄱ', eKSSM::Fin_G		}, { L'ㄲ', eKSSM::Fin_GG	}, { L'ㄳ', eKSSM::Fin_GS	}, { L'ㄴ', eKSSM::Fin_N		},
		{ L'ㄵ', eKSSM::Fin_NJ		}, { L'ㄶ', eKSSM::Fin_NH	}, { L'ㄷ', eKSSM::Fin_D		}, { L'ㄹ', eKSSM::Fin_L		}, { L'ㄺ', eKSSM::Fin_LG	},
		{ L'ㄻ', eKSSM::Fin_LM		}, { L'ㄼ', eKSSM::Fin_LB	}, { L'ㄽ', eKSSM::Fin_LS	}, { L'ㄾ', eKSSM::Fin_LT	}, { L'ㄿ', eKSSM::Fin_LP	},
		{ L'ㅀ', eKSSM::Fin_LH		}, { L'ㅁ', eKSSM::Fin_M		}, { L'ㅂ', eKSSM::Fin_B		}, { L'ㅄ', eKSSM::Fin_BS	}, { L'ㅅ', eKSSM::Fin_S		},
		{ L'ㅆ', eKSSM::Fin_SS		}, { L'ㅇ', eKSSM::Fin_O		}, { L'ㅈ', eKSSM::Fin_J		}, { L'ㅊ', eKSSM::Fin_CH	}, { L'ㅋ', eKSSM::Fin_K		},
		{ L'ㅌ', eKSSM::Fin_T		}, { L'ㅍ', eKSSM::Fin_P		}, { L'ㅎ', eKSSM::Fin_H		},
	};
	std::map<eKSSM, wchar_t> const s_mapKSSM2W_Ini {
		// 초성
		{ eKSSM::Ini_Fill,	L'\0' }, { eKSSM::Ini_G,	L'ㄱ' }, { eKSSM::Ini_GG,	L'ㄲ' }, { eKSSM::Ini_N,		L'ㄴ' }, { eKSSM::Ini_D,		L'ㄷ' },
		{ eKSSM::Ini_DD,	L'ㄸ' }, { eKSSM::Ini_L,		L'ㄹ' }, { eKSSM::Ini_M,		L'ㅁ' }, { eKSSM::Ini_B,		L'ㅂ' }, { eKSSM::Ini_BB,	L'ㅃ' },
		{ eKSSM::Ini_S,		L'ㅅ' }, { eKSSM::Ini_SS,	L'ㅆ' }, { eKSSM::Ini_O,		L'ㅇ' }, { eKSSM::Ini_J,		L'ㅈ' }, { eKSSM::Ini_JJ,	L'ㅉ' },
		{ eKSSM::Ini_CH,	L'ㅊ' }, { eKSSM::Ini_K,		L'ㅋ' }, { eKSSM::Ini_T,		L'ㅌ' }, { eKSSM::Ini_P,		L'ㅍ' }, { eKSSM::Ini_H,		L'ㅎ' },
	};
	std::map<eKSSM, wchar_t> const s_mapKSSM2W_Mid {
		// 중성
		{ eKSSM::Mid_Fill,	L'\0' }, { eKSSM::Mid_R,	L'ㅏ' }, { eKSSM::Mid_RV,	L'ㅐ' }, { eKSSM::Mid_RR,	L'ㅑ' }, { eKSSM::Mid_RRV,	L'ㅒ' },
		{ eKSSM::Mid_L,		L'ㅓ' }, { eKSSM::Mid_LV,	L'ㅔ' }, { eKSSM::Mid_LL,	L'ㅕ' }, { eKSSM::Mid_LLV,	L'ㅖ' }, { eKSSM::Mid_U,		L'ㅗ' },
		{ eKSSM::Mid_UR,	L'ㅘ' }, { eKSSM::Mid_URV,	L'ㅙ' }, { eKSSM::Mid_UV,	L'ㅚ' }, { eKSSM::Mid_UU,	L'ㅛ' }, { eKSSM::Mid_D,		L'ㅜ' },
		{ eKSSM::Mid_DL,	L'ㅝ' }, { eKSSM::Mid_DLV,	L'ㅞ' }, { eKSSM::Mid_DV,	L'ㅟ' }, { eKSSM::Mid_DD,	L'ㅠ' }, { eKSSM::Mid_H,		L'ㅡ' },
		{ eKSSM::Mid_HV,	L'ㅢ' }, { eKSSM::Mid_V,		L'ㅣ' },
	};
	std::map<eKSSM, wchar_t> const s_mapKSSM2W_Fin {
		// 종성
		{ eKSSM::Fin_Fill,	L'\0' }, { eKSSM::Fin_G,	L'ㄱ' }, { eKSSM::Fin_GG,	L'ㄲ' }, { eKSSM::Fin_GS,	L'ㄳ' }, { eKSSM::Fin_N,		L'ㄴ'},
		{ eKSSM::Fin_NJ,	L'ㄵ' }, { eKSSM::Fin_NH,	L'ㄶ' }, { eKSSM::Fin_D,		L'ㄷ' }, { eKSSM::Fin_L,		L'ㄹ' }, { eKSSM::Fin_LG,	L'ㄺ' },
		{ eKSSM::Fin_LM,	L'ㄻ' }, { eKSSM::Fin_LB,	L'ㄼ' }, { eKSSM::Fin_LS,	L'ㄽ' }, { eKSSM::Fin_LT,	L'ㄾ' }, { eKSSM::Fin_LP,	L'ㄿ' },
		{ eKSSM::Fin_LH,	L'ㅀ' }, { eKSSM::Fin_M,		L'ㅁ' }, { eKSSM::Fin_B,		L'ㅂ' }, { eKSSM::Fin_BS,	L'ㅄ' }, { eKSSM::Fin_S,		L'ㅅ' },
		{ eKSSM::Fin_SS,	L'ㅆ' }, { eKSSM::Fin_O,		L'ㅇ' }, { eKSSM::Fin_J,		L'ㅈ' }, { eKSSM::Fin_CH,	L'ㅊ' }, { eKSSM::Fin_K,		L'ㅋ' },
		{ eKSSM::Fin_T,		L'ㅌ' }, { eKSSM::Fin_P,		L'ㅍ' }, { eKSSM::Fin_H,		L'ㅎ' },
	};

	BSC__NODISCARD constexpr charKSSM_t ComposeKSSM(eKSSM ini, eKSSM mid, eKSSM fin) {
		return std::to_underlying(eKSSM::maskChar) | std::to_underlying(ini) | std::to_underlying(mid) | std::to_underlying(fin);
	}

	BSC__NODISCARD eKSSM GetKSSM_Syllable(std::map<wchar_t, eKSSM> const& map, wchar_t c, eKSSM cDefault) {
		auto iter = map.find(c);
		if (iter != map.end())
			return iter->second;
		else
			return cDefault;
	}

	BSC__NODISCARD eKSSM GetKSSM_Ini(wchar_t c) { return GetKSSM_Syllable(s_mapW2KSSM_Ini, c, eKSSM::Ini_Fill); }
	BSC__NODISCARD eKSSM GetKSSM_Mid(wchar_t c) { return GetKSSM_Syllable(s_mapW2KSSM_Mid, c, eKSSM::Mid_Fill); }
	BSC__NODISCARD eKSSM GetKSSM_Fin(wchar_t c) { return GetKSSM_Syllable(s_mapW2KSSM_Fin, c, eKSSM::Fin_Fill); }

	BSC__NODISCARD charKSSM_t ComposeKSSM(wchar_t i, wchar_t m, wchar_t f = L'\0') {
		auto ini = GetKSSM_Ini(i);
		auto mid = GetKSSM_Mid(m);
		auto fin = GetKSSM_Fin(f);
		if (ini == eKSSM::Ini_Fill and mid == eKSSM::Mid_Fill and fin == eKSSM::Fin_Fill)
			return {};
		return ComposeKSSM(ini, mid, fin);
	}

	BSC__NODISCARD constexpr std::tuple<charKSSM_t, charKSSM_t, charKSSM_t> DeComposeKSSM(charKSSM_t c) {
		if (c & (charKSSM_t)eKSSM::maskChar) {
			return {
				(charKSSM_t)(c & std::to_underlying(eKSSM::maskIni)),
				(charKSSM_t)(c & std::to_underlying(eKSSM::maskMid)),
				(charKSSM_t)(c & std::to_underlying(eKSSM::maskFin))
			};
		}
		else
			return { };
	}

}
