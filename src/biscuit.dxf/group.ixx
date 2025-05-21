module;

#include "biscuit/dependencies/glaze.h"

export module biscuit.dxf:group;
import std;
import biscuit;

using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::dxf {

	using binary_t = std::vector<std::uint8_t>;
	using string_t = std::string;
	using string_view_t = std::string_view;
	using group_value_t = std::variant<bool, std::int16_t, std::int32_t, std::int64_t, double, string_t, binary_t>;

	//=============================================================================================================================
	// group
	struct eGROUP_CODE {
		using value_t = int16;
		enum : value_t {
			entity = 0,
			subclass = 100,
			control = 102,

			extended = 1000,
			last = 1072,
		};
		value_t eCode;

		constexpr eGROUP_CODE() = default;
		constexpr eGROUP_CODE(eGROUP_CODE const&) = default;
		constexpr eGROUP_CODE(eGROUP_CODE&&) = default;
		constexpr eGROUP_CODE& operator = (eGROUP_CODE const&) = default;
		constexpr eGROUP_CODE& operator = (eGROUP_CODE&&) = default;
		constexpr eGROUP_CODE(value_t eValueType) : eCode(eValueType) {}

		//constexpr operator value_t() const { return value; }
		eGROUP_CODE& operator = (value_t eValueType) { eCode = eValueType; return *this; }
		constexpr auto operator <=> (eGROUP_CODE const&) const = default;
		constexpr auto operator <=> (value_t const& eValueType) const { return eCode <=> eValueType; }
	};
	constexpr auto operator "" _g(unsigned long long v) { return eGROUP_CODE{ (int16)v }; }

	using group_code_t = eGROUP_CODE;

	struct alignas(32) sGroup : public eGROUP_CODE {	// alignas(32) - hoping for better cache performance (don't know if it works)
	public:
		enum eVALUE_TYPE : int8 { none = -1, boolean = 0, i16, i32, i64, dbl, str, hex_str };

		//group_code_t eCode{};
		group_value_t value;

		constexpr sGroup() = default;
		constexpr sGroup(sGroup const&) = default;
		constexpr sGroup(sGroup&&) = default;
		constexpr sGroup& operator = (sGroup const&) = default;
		constexpr sGroup& operator = (sGroup&&) = default;

		constexpr sGroup(group_code_t code, group_value_t v) : eGROUP_CODE(code), value(std::move(v)) {}

		constexpr bool operator == (sGroup const&) const = default;
		constexpr bool operator != (sGroup const&) const = default;

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
		bool GetValue(T& value) const {
			if constexpr ( std::is_integral_v<T> and sizeof(T) > sizeof(bool) and sizeof(T) < sizeof(int64) ) {
				if (auto v = GetInt()) {
					value = *v;
					return true;
				}
			}
			else if (auto v = GetValue<T>()) {
				value = *v;
				return true;
			}
			return false;
		}

	#if 1
		constexpr static sGroup::eVALUE_TYPE GET_VALUE_TYPE_ENUM(group_code_t code) {
			//static_assert(std::is_constant_evaluated());
			using enum sGroup::eVALUE_TYPE;

			auto InRange = [](auto&& code, int16 min, int16 max) {
				return code >= min and code <= max;
			};

		#if 0
																//================================================================================================
																//	2012 DXF Reference
																//	source - https://images.autodesk.com/adsk/files/autocad_2012_pdf_dxf-reference_enu.pdf
																//------------------------------------------------------------------------------------------------
																//  Group code value types
																//	Code range	| Group value type
																//--------------+---------------------------------------------------------------------------------
			if (false) ;										//				|
			else if (InRange(code,    0,    9)) return str;		//	   0-   9	| String (with the introduction of extended symbol names in AutoCAD 2000,
																//				| the 255-character limit has been increased to 2049 single-byte characters
																//				| not including the newline at the end of the line)
			else if (InRange(code,   10,   39)) return dbl;		//	  10-  39	| Double precision 3D point value
			else if (InRange(code,   40,   59)) return dbl;		//	  40-  59	| Double-precision floating-point value
			else if (InRange(code,   60,   79)) return i16;		//	  60-  79	| 16-bit integer value
			else if (InRange(code,   90,   99)) return i32;		//	  90-  99	| 32-bit integer value
			else if (InRange(code,	100,  100)) return str;		//	      100	| String (255-character maximum; less for Unicode strings)
			else if (InRange(code,	102,  102)) return str;		//	      102	| String (255-character maximum; less for Unicode strings)
			else if (InRange(code,	105,  105)) return hex_str;	//	      105	| String representing hexadecimal (hex) handle value
			else if (InRange(code,  110,  119)) return dbl;		//	 110- 119	| Double precision floating-point value
			else if (InRange(code,  120,  129)) return dbl;		//	 120- 129	| Double precision floating-point value
			else if (InRange(code,  130,  139)) return dbl;		//	 130- 139	| Double precision floating-point value
			else if (InRange(code,  140,  149)) return dbl;		//	 140- 149	| Double precision scalar floating-point value
			else if (InRange(code,  160,  169)) return i64;		//	 160- 169	| 64-bit integer value
			else if (InRange(code,  170,  179)) return i16;		//	 170- 179	| 16-bit integer value
			else if (InRange(code,  210,  239)) return dbl;		//	 210- 239	| Double-precision floating-point value
			else if (InRange(code,  270,  279)) return i16;		//	 270- 279	| 16-bit integer value
			else if (InRange(code,  280,  289)) return i16;		//	 280- 289	| 16-bit integer value
			else if (InRange(code,  290,  299)) return boolean;	//	 290- 299	| Boolean flag value
			else if (InRange(code,  300,  309)) return str;		//	 300- 309	| Arbitrary text string
			else if (InRange(code,  310,  319)) return hex_str;	//	 310- 319	| String representing hex value of binary chunk
			else if (InRange(code,  320,  329)) return hex_str;	//	 320- 329	| String representing hex handle value
			else if (InRange(code,  330,  369)) return hex_str;	//	 330- 369	| String representing hex object IDs
			else if (InRange(code,  370,  379)) return i16;		//	 370- 379	| 16-bit integer value
			else if (InRange(code,  380,  389)) return i16;		//	 380- 389	| 16-bit integer value
			else if (InRange(code,  390,  399)) return str;		//	 390- 399	| String representing hex handle value
			else if (InRange(code,  400,  409)) return i16;		//	 400- 409	| 16-bit integer value
			else if (InRange(code,  410,  419)) return str;		//	 410- 419	| String
			else if (InRange(code,  420,  429)) return i32;		//	 420- 429	| 32-bit integer value
			else if (InRange(code,  430,  439)) return str;		//	 430- 439	| String
			else if (InRange(code,  440,  449)) return i32;		//	 440- 449	| 32-bit integer value
			else if (InRange(code,  450,  459)) return i32;		//	 450- 459	| Long
			else if (InRange(code,  460,  469)) return dbl;		//	 460- 469	| Double-precision floating-point value
			else if (InRange(code,  470,  479)) return str;		//	 470- 479	| String
			else if (InRange(code,  480,  481)) return hex_str;	//	 480- 481	| String representing hex handle value
			else if (InRange(code,  999,  999)) return str;		//	      999	| Comment (string)
			else if (InRange(code, 1000, 1009)) return str;		//	1000-1009	| String (same limits as indicated with 0-9 code range)
			else if (InRange(code, 1010, 1059)) return dbl;		//	1010-1059	| Double-precision floating-point value
			else if (InRange(code, 1060, 1070)) return i16;		//	1060-1070	| 16-bit integer value
			else if (InRange(code, 1071, 1071)) return i32;		//	     1071	| 32-bit integer value
			else return none;									//================================================================================================
			*/
		#endif

																//================================================================================================
																// 2024
																// source - https://help.autodesk.com/view/OARX/2024/ENU/?guid=GUID-2553CF98-44F6-4828-82DD-FE3BC7448113
																//------------------------------------------------------------------------------------------------
			if (false);											//
			else if (InRange(code,    0,    9)) return str;	 	//	    0-   9	| String (with the introduction of extended symbol names in AutoCAD 2000, 
																//				| the 255-character limit has been increased to 2049 single-byte characters
																//				| not including the newline at the end of the line)
																//				| see the "Storage of String Values" section for more information
			else if (InRange(code,   10,   19)) return dbl;		//	   10-  17	| Double precision 3D point value
			else if (InRange(code,   20,   29)) return dbl;		//	   20-  27	| Double precision 3D point value
			else if (InRange(code,   30,   37)) return dbl;		//	   30-  37	| Double precision 3D point value
			else if (InRange(code,   38,   59)) return dbl;		//	   38-  59	| Double-precision floating-point value
			else if (InRange(code,   60,   79)) return i16;		//	   60-  79	| 16-bit integer value
			else if (InRange(code,   90,   99)) return i32;		//	   90-  99	| 32-bit integer value
			else if (InRange(code,  100,  102)) return str;		//	  100- 102	| String (255-character maximum, less for Unicode strings)
			else if (InRange(code,  105,  105)) return hex_str;	//		   105	| String representing hexadecimal (hex) handle value
			else if (InRange(code,  110,  119)) return dbl;		//	  110- 119	| Double precision floating-point value
			else if (InRange(code,  120,  129)) return dbl;		//	  120- 129	| Double precision floating-point value
			else if (InRange(code,  130,  139)) return dbl;		//	  130- 139	| Double precision floating-point value
			else if (InRange(code,  140,  149)) return dbl;		//	  140- 149	| Double precision scalar floating-point value
			else if (InRange(code,  160,  169)) return i64;		//	  160- 169	| 64-bit integer value
			else if (InRange(code,  170,  179)) return i16;		//	  170- 179	| 16-bit integer value
			else if (InRange(code,  210,  239)) return dbl;		//	  210- 239	| Double-precision floating-point value
			else if (InRange(code,  270,  279)) return i16;		//	  270- 279	| 16-bit integer value
			else if (InRange(code,  280,  289)) return i16;		//	  280- 289	| 16-bit integer value
			else if (InRange(code,  290,  299)) return boolean;	//	  290- 299	| Boolean flag value
			else if (InRange(code,  300,  309)) return str;		//	  300- 309	| Arbitrary text string;
			else if (InRange(code,  310,  319)) return hex_str;	//	  310- 319	| String representing hex value of binary chunk
			else if (InRange(code,  320,  329)) return hex_str;	//	  320- 329	| String representing hex handle value
			else if (InRange(code,  330,  369)) return hex_str;	//	  330- 369	| String representing hex object IDs
			else if (InRange(code,  370,  379)) return i16;		//	  370- 379	| 16-bit integer value
			else if (InRange(code,  380,  389)) return i16;		//	  380- 389	| 16-bit integer value
			else if (InRange(code,  390,  399)) return hex_str;	//	  390- 399	| String representing hex handle value
			else if (InRange(code,  400,  409)) return i16;		//	  400- 409	| 16-bit integer value
			else if (InRange(code,  410,  419)) return str;		//	  410- 419	| String;
			else if (InRange(code,  420,  429)) return i32;		//	  420- 429	| 32-bit integer value
			else if (InRange(code,  430,  439)) return str;		//	  430- 439	| String;
			else if (InRange(code,  440,  449)) return i32;		//	  440- 449	| 32-bit integer value
			else if (InRange(code,  450,  459)) return i32;		//	  450- 459	| Long ???
			else if (InRange(code,  460,  469)) return dbl;		//	  460- 469	| Double-precision floating-point value
			else if (InRange(code,  470,  479)) return str;		//	  470- 479	| String;
			else if (InRange(code,  480,  481)) return hex_str;	//	  480- 481	| String representing a hex handle value
			else if (InRange(code,  999,  999)) return str;		//		   999	| Comment (string);
			else if (InRange(code, 1000, 1003)) return str;		//	 1000-1003	| String (same limits as indicated with 0-9 code range)
			else if (InRange(code, 1004, 1004)) return hex_str;	//		  1004	| String representing a hex value of binary chunk
			else if (InRange(code, 1005, 1005)) return str;		//		  1005	| String (same limits as indicated with 0-9 code range)
			else if (InRange(code, 1010, 1013)) return dbl;		//	 1010-1013	| Double-precision floating-point value
			else if (InRange(code, 1020, 1023)) return dbl;		//	 1020-1023	| Double-precision floating-point value
			else if (InRange(code, 1030, 1033)) return dbl;		//	 1030-1033	| Double-precision floating-point value
			else if (InRange(code, 1040, 1042)) return dbl;		//	 1040-1042	| Double-precision floating-point value
			else if (InRange(code, 1070, 1070)) return i16;		//		  1070	| 16-bit integer value
			else if (InRange(code, 1071, 1071)) return i32;		//		  1071	| 32-bit integer value
			else return none;
		}
	#endif
		sGroup::eVALUE_TYPE GetValueTypeEnumByCode() const { return GetValueTypeEnum(eCode); }
		static sGroup::eVALUE_TYPE GetValueTypeEnum(group_code_t eCode) {
			constinit static auto const s_tblGroupCodeToType = [] {
				std::array<sGroup::eVALUE_TYPE, group_code_t::last> tbl{};
				for (group_code_t code{}; code.eCode < tbl.size(); code.eCode++) {
					tbl[code.eCode] = GET_VALUE_TYPE_ENUM(code);
				}
				return tbl;
			}();

			if (eCode < 0 or eCode.eCode >= s_tblGroupCodeToType.size())
				return sGroup::none;
			return s_tblGroupCodeToType[eCode.eCode];
		}
	};

	auto format_as(group_code_t eValueType) { return eValueType.eCode; }
	auto format_as(sGroup const& group) {
		return std::pair{ group.eCode, group.value };
	}

	using groups_t = std::vector<sGroup>;
	using group_iter_t = groups_t::const_iterator;

#if 1
	//-----------------------------------------------------------------------------------------------------------------------------
	namespace detail {

		template < sGroup::eGROUP_CODE eGroupCode_, typename TUserDefined = void >
		struct TGroupValue {
			constexpr inline static sGroup::eGROUP_CODE const eGroupCode{eGroupCode_};
			constexpr inline static sGroup::eVALUE_TYPE const eValueType = sGroup::GET_VALUE_TYPE_ENUM(eGroupCode);

			using implicit_value_type =
				std::conditional_t<eValueType == sGroup::boolean, bool,
					std::conditional_t<eValueType == sGroup::i16, int16,
						std::conditional_t<eValueType == sGroup::i32, int32,
							std::conditional_t<eValueType == sGroup::i64, int64,
								std::conditional_t<eValueType == sGroup::dbl, double,
									std::conditional_t<eValueType == sGroup::str, string_t,
										std::conditional_t<eValueType == sGroup::hex_str, binary_t, void>>>>>>>;

			using value_t = std::conditional_t<std::is_same_v<TUserDefined, void>, implicit_value_type, TUserDefined>;

			static_assert(sizeof(value_t) == sizeof(implicit_value_type));

			value_t value{};

			auto operator <=> (TGroupValue const&) const = default;

			TGroupValue& operator = (TGroupValue const&) = default;
			TGroupValue& operator = (value_t const& v) { value = v; return *this; }
			TGroupValue& operator = (value_t&& v) { value = std::move(v); return *this; }

			//value_t& operator() () { return value; }
			value_t const& operator() () const { return value; }
			operator value_t& () { return value; }
			operator value_t const& () const { return value; }

		};

	}

	//template < sGroup::eVALUE_TYPE eValueType, typename TUserDefined = void >
	//using enum_to_value_t = typename detail::TGroupValueTypeByEnum<eValueType, TUserDefined>::value_t;

	template < group_code_t::value_t code, typename TUserDefined = void >
	using group_code_value_t = detail::TGroupValue<(group_code_t{code}), TUserDefined>;

#endif		

	//=============================================================================================================================
	template < typename TField >
	bool ReadItemSingleMember(TField& item, sGroup const& group, size_t& index) {
		bool bFound{};

		// direct access to entity members as is using glz::reflect
		constexpr static size_t nMemberSize = glz::reflect<TField>::size;
		bFound = ForEachIntSeq<nMemberSize>(
			[&]<int I>{
			auto& v = glz::reflect<TField>::elem<I>(item);
			using member_t = std::remove_cvref_t<decltype(v)>;
			if constexpr (requires (member_t m) { m.eGroupCode; }) {
				if (v.eGroupCode == group.eCode) {
					if (index > 0) {
						index--;
						return false;
					}
					if constexpr (requires (member_t m) { m.value; }) {
						group.GetValue(v.value);
					}
					else if constexpr (requires (member_t m) { m.Value(item); }) {
						group.GetValue(v.Value(item));
					}
					return true;
				}
			}
			return false;
		});
		if (bFound) return true;

		// indirect access via group_members
		if constexpr (requires (TField ) { TField::custom_field; }) {
			constexpr static size_t nTupleSize = std::tuple_size_v<decltype(TField::custom_field)>;
			bFound = ForEachIntSeq<nTupleSize>([&]<int I>{
				auto const& pair = std::get<I>(TField::custom_field);
				if constexpr (std::is_integral_v<std::remove_cvref_t<decltype(pair)>>) {
					// end marker. do nothing.
				}
				else {
					if (pair.first == group.eCode) {
						if (index > 0) {
							index--;
							return false;
						}
						group.GetValue(pair.second(item));
						return true;
					}
				}
				return false;
			});

			if (bFound)
				return true;
		}

		return false;
	}
	//-----------------------------------------------------------------------------------------------------------------------------
	template < typename TField >
	bool ReadItemSingleMember(TField& item, sGroup const& group) {
		size_t index{};
		return ReadItemSingleMember(item, group, index);
	}

}	// namespace biscuit::dxf

