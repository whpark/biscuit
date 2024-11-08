module;

export module biscuit.dxf:group;
import std;
import biscuit;

using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::dxf {

	//=============================================================================================================================
	// group
	using binary_t = std::vector<uint8>;
	using string_t = std::string;
	using group_code_t = int16;
	using group_value_t = std::variant<bool, int16, int32, int64, double, string_t, binary_t>;
	enum class eGROUP_VALUE : int8 { none = -1, boolean = 0, i16, i32, i64, dbl, str, hex_data };

	constexpr group_code_t const g_iMaxGroupCode = 1071;

	constexpr struct alignas(32) sGroup {	// alignas(32) - hoping for better cache performance (don't know if it works)
	public:
		group_code_t iGroupCode{};
		group_value_t value;

		bool operator == (sGroup const&) const = default;
		bool operator != (sGroup const&) const = default;

		template < std::convertible_to<group_value_t> T >
		std::optional<T> GetValue() const {
			constexpr static auto index = group_value_t{T{}}.index();
			if (value.index() != index)
				return std::nullopt;
			return std::get<T>(value);
		}
		std::optional<int> GetInt() const {
			std::optional<int> result;
			std::visit([&result]<typename T>(T const& v) {
				if constexpr (std::is_same_v<T, int64>) {
					//static_assert(false);
					if (v >= std::numeric_limits<int>::min() and v <= std::numeric_limits<int>::max()) {
						result = (int)v;
					}
				}
				else if constexpr (std::is_integral_v<T>)
					result = (int)v;
			}, value);
			return result;
		}

		template < typename T >
		bool Get(T& value) const {
			if (auto v = GetValue<T>()) {
				value = *v;
				return true;
			}
			return false;
		}

		eGROUP_VALUE GetValueTypeFromGroupCode() const { return GetValueTypeFromGroupCode(iGroupCode); }
		static eGROUP_VALUE GetValueTypeFromGroupCode(group_code_t iGroupCode);
	};

	//=============================================================================================================================
	//	source - https://images.autodesk.com/adsk/files/autocad_2012_pdf_dxf-reference_enu.pdf
	//
	//  Group code value types
	//	Code range		Group value type
	//
	//		   0-   9		String (with the introduction of extended symbol names in AutoCAD 2000,
	//						the 255-character limit has been increased to 2049 single-byte characters
	//						not including the newline at the end of the line)
	//		  10-  39		Double precision 3D point value
	//		  40-  59		Double-precision floating-point value
	//		  60-  79		16-bit integer value
	//		  90-  99		32-bit integer value
	//		      100		String (255-character maximum; less for Unicode strings)
	//		      102		String (255-character maximum; less for Unicode strings)
	//		      105		String representing hexadecimal (hex) handle value
	//		 110- 119		Double precision floating-point value
	//		 120- 129		Double precision floating-point value
	//		 130- 139		Double precision floating-point value
	//		 140- 149		Double precision scalar floating-point value
	//		 160- 169		64-bit integer value
	//		 170- 179		16-bit integer value
	//		 210- 239		Double-precision floating-point value
	//		 270- 279		16-bit integer value
	//		 280- 289		16-bit integer value
	//		 290- 299		Boolean flag value
	//		 300- 309		Arbitrary text string
	//		 310- 319		String representing hex value of binary chunk
	//		 320- 329		String representing hex handle value
	//		 330- 369		String representing hex object IDs
	//		 370- 379		16-bit integer value
	//		 380- 389		16-bit integer value
	//		 390- 399		String representing hex handle value
	//		 400- 409		16-bit integer value
	//		 410- 419		String
	//		 420- 429		32-bit integer value
	//		 430- 439		String
	//		 440- 449		32-bit integer value
	//		 450- 459		Long
	//		 460- 469		Double-precision floating-point value
	//		 470- 479		String
	//		 480- 481		String representing hex handle value
	//		      999		Comment (string)
	//		1000-1009		String (same limits as indicated with 0-9 code range)
	//		1010-1059		Double-precision floating-point value
	//		1060-1070		16-bit integer value
	//		     1071		32-bit integer value
	//=============================================================================================================================

	eGROUP_VALUE sGroup::GetValueTypeFromGroupCode(group_code_t iGroupCode) {
		constexpr static auto const s_mapGroupCodeToType = [] {
			std::array<eGROUP_VALUE, g_iMaxGroupCode+1> map{};
			for (group_code_t code{}; code < map.size(); code++) {
				using enum eGROUP_VALUE;
				if (false) ;
				else if (code >=   0 and code <=    9) map[code] = str;				//	String (with the introduction of extended symbol names in AutoCAD 2000,
				else if (code >=  10 and code <=   39) map[code] = dbl;				//	Double precision 3D point value
				else if (code >=  40 and code <=   59) map[code] = dbl;				//	Double-precision floating-point value
				else if (code >=  60 and code <=   79) map[code] = i16;				//	16-bit integer value
				else if (code >=  90 and code <=   99) map[code] = i32;				//	32-bit integer value
				else if (                code ==  100) map[code] = str;				//	String (255-character maximum; less for Unicode strings)
				else if (                code ==  102) map[code] = str;				//	String (255-character maximum; less for Unicode strings)
				else if (                code ==  105) map[code] = hex_data;		//	String representing hexadecimal (hex) handle value
				else if (code >= 110 and code <=  119) map[code] = dbl;				//	Double precision floating-point value
				else if (code >= 120 and code <=  129) map[code] = dbl;				//	Double precision floating-point value
				else if (code >= 130 and code <=  139) map[code] = dbl;				//	Double precision floating-point value
				else if (code >= 140 and code <=  149) map[code] = dbl;				//	Double precision scalar floating-point value
				else if (code >= 160 and code <=  169) map[code] = i64;				//	64-bit integer value
				else if (code >= 170 and code <=  179) map[code] = i16;				//	16-bit integer value
				else if (code >= 210 and code <=  239) map[code] = dbl;				//	Double-precision floating-point value
				else if (code >= 270 and code <=  279) map[code] = i16;				//	16-bit integer value
				else if (code >= 280 and code <=  289) map[code] = i16;				//	16-bit integer value
				else if (code >= 290 and code <=  299) map[code] = boolean;			//	Boolean flag value
				else if (code >= 300 and code <=  309) map[code] = str;				//	Arbitrary text string
				else if (code >= 310 and code <=  319) map[code] = hex_data;		//	String representing hex value of binary chunk
				else if (code >= 320 and code <=  329) map[code] = hex_data;		//	String representing hex handle value
				else if (code >= 330 and code <=  369) map[code] = hex_data;		//	String representing hex object IDs
				else if (code >= 370 and code <=  379) map[code] = i16;				//	16-bit integer value
				else if (code >= 380 and code <=  389) map[code] = i16;				//	16-bit integer value
				else if (code >= 390 and code <=  399) map[code] = str;				//	String representing hex handle value
				else if (code >= 400 and code <=  409) map[code] = i16;				//	16-bit integer value
				else if (code >= 410 and code <=  419) map[code] = str;				//	String
				else if (code >= 420 and code <=  429) map[code] = i32;				//	32-bit integer value
				else if (code >= 430 and code <=  439) map[code] = str;				//	String
				else if (code >= 440 and code <=  449) map[code] = i32;				//	32-bit integer value
				else if (code >= 450 and code <=  459) map[code] = i32;				//	Long
				else if (code >= 460 and code <=  469) map[code] = dbl;				//	Double-precision floating-point value
				else if (code >= 470 and code <=  479) map[code] = str;				//	String
				else if (code >= 480 and code <=  481) map[code] = hex_data;		//	String representing hex handle value
				else if (                code ==  999) map[code] = str;				//	Comment (string)
				else if (code >=1000 and code <= 1009) map[code] = str;				//	String (same limits as indicated with 0-9 code range)
				else if (code >=1010 and code <= 1059) map[code] = dbl;				//	Double-precision floating-point value
				else if (code >=1060 and code <= 1070) map[code] = i16;				//	16-bit integer value
				else if (                code == 1071) map[code] = i32;				//	32-bit integer value
				else map[code] = none;
			}
			return map;
		}();

		if (iGroupCode < 0 or iGroupCode >= s_mapGroupCodeToType.size())
			return eGROUP_VALUE::none;
		return s_mapGroupCodeToType[iGroupCode];
	}

}	// namespace biscuit::dxf

