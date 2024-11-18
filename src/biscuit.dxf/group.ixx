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
	using string_view_t = std::string_view;
	using group_code_t = int16;
	using group_value_t = std::variant<bool, int16, int32, int64, double, string_t, binary_t>;
	enum class eGROUP_VALUE_TYPE : int8 { none = -1, boolean = 0, i16, i32, i64, dbl, str, hex_data };

	constexpr group_code_t const g_iMaxGroupCode = 1071;

	struct alignas(32) sGroup {	// alignas(32) - hoping for better cache performance (don't know if it works)
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

		constexpr static eGROUP_VALUE_TYPE GET_VALUE_TYPE_ENUM(group_code_t code) {
			//static_assert(std::is_constant_evaluated());
			using enum eGROUP_VALUE_TYPE;									
																			//================================================================================================
																			//	source - https://images.autodesk.com/adsk/files/autocad_2012_pdf_dxf-reference_enu.pdf
																			//------------------------------------------------------------------------------------------------
																			//  Group code value types
																			//	Code range	| Group value type
																			//--------------+---------------------------------------------------------------------------------
			if (false) ;													//				|
			else if (code >=   0 and code <=    9) return str;				//	   0-   9	| String (with the introduction of extended symbol names in AutoCAD 2000,
																			//				| the 255-character limit has been increased to 2049 single-byte characters
																			//				| not including the newline at the end of the line)
			else if (code >=  10 and code <=   39) return dbl;				//	  10-  39	| Double precision 3D point value
			else if (code >=  40 and code <=   59) return dbl;				//	  40-  59	| Double-precision floating-point value
			else if (code >=  60 and code <=   79) return i16;				//	  60-  79	| 16-bit integer value
			else if (code >=  90 and code <=   99) return i32;				//	  90-  99	| 32-bit integer value
			else if (                code ==  100) return str;				//	      100	| String (255-character maximum; less for Unicode strings)
			else if (                code ==  102) return str;				//	      102	| String (255-character maximum; less for Unicode strings)
			else if (                code ==  105) return hex_data;			//	      105	| String representing hexadecimal (hex) handle value
			else if (code >= 110 and code <=  119) return dbl;				//	 110- 119	| Double precision floating-point value
			else if (code >= 120 and code <=  129) return dbl;				//	 120- 129	| Double precision floating-point value
			else if (code >= 130 and code <=  139) return dbl;				//	 130- 139	| Double precision floating-point value
			else if (code >= 140 and code <=  149) return dbl;				//	 140- 149	| Double precision scalar floating-point value
			else if (code >= 160 and code <=  169) return i64;				//	 160- 169	| 64-bit integer value
			else if (code >= 170 and code <=  179) return i16;				//	 170- 179	| 16-bit integer value
			else if (code >= 210 and code <=  239) return dbl;				//	 210- 239	| Double-precision floating-point value
			else if (code >= 270 and code <=  279) return i16;				//	 270- 279	| 16-bit integer value
			else if (code >= 280 and code <=  289) return i16;				//	 280- 289	| 16-bit integer value
			else if (code >= 290 and code <=  299) return boolean;			//	 290- 299	| Boolean flag value
			else if (code >= 300 and code <=  309) return str;				//	 300- 309	| Arbitrary text string
			else if (code >= 310 and code <=  319) return hex_data;			//	 310- 319	| String representing hex value of binary chunk
			else if (code >= 320 and code <=  329) return hex_data;			//	 320- 329	| String representing hex handle value
			else if (code >= 330 and code <=  369) return hex_data;			//	 330- 369	| String representing hex object IDs
			else if (code >= 370 and code <=  379) return i16;				//	 370- 379	| 16-bit integer value
			else if (code >= 380 and code <=  389) return i16;				//	 380- 389	| 16-bit integer value
			else if (code >= 390 and code <=  399) return str;				//	 390- 399	| String representing hex handle value
			else if (code >= 400 and code <=  409) return i16;				//	 400- 409	| 16-bit integer value
			else if (code >= 410 and code <=  419) return str;				//	 410- 419	| String
			else if (code >= 420 and code <=  429) return i32;				//	 420- 429	| 32-bit integer value
			else if (code >= 430 and code <=  439) return str;				//	 430- 439	| String
			else if (code >= 440 and code <=  449) return i32;				//	 440- 449	| 32-bit integer value
			else if (code >= 450 and code <=  459) return i32;				//	 450- 459	| Long
			else if (code >= 460 and code <=  469) return dbl;				//	 460- 469	| Double-precision floating-point value
			else if (code >= 470 and code <=  479) return str;				//	 470- 479	| String
			else if (code >= 480 and code <=  481) return hex_data;			//	 480- 481	| String representing hex handle value
			else if (                code ==  999) return str;				//	      999	| Comment (string)
			else if (code >=1000 and code <= 1009) return str;				//	1000-1009	| String (same limits as indicated with 0-9 code range)
			else if (code >=1010 and code <= 1059) return dbl;				//	1010-1059	| Double-precision floating-point value
			else if (code >=1060 and code <= 1070) return i16;				//	1060-1070	| 16-bit integer value
			else if (                code == 1071) return i32;				//	     1071	| 32-bit integer value
			else return none;												//================================================================================================
		}

		eGROUP_VALUE_TYPE GetValueTypeEnumByCode() const { return GetValueTypeEnum(iGroupCode); }
		static eGROUP_VALUE_TYPE GetValueTypeEnum(group_code_t iGroupCode) {
			constinit static auto const s_mapGroupCodeToType = [] {
				std::array<eGROUP_VALUE_TYPE, g_iMaxGroupCode+1> map{};
				for (group_code_t code{}; code < map.size(); code++) {
					map[code] = GET_VALUE_TYPE_ENUM(code);
				}
				return map;
			}();

			if (iGroupCode < 0 or iGroupCode >= s_mapGroupCodeToType.size())
				return eGROUP_VALUE_TYPE::none;
			return s_mapGroupCodeToType[iGroupCode];
		}
	};

	using groups_t = std::vector<sGroup>;
	using group_iter_t = groups_t::const_iterator;

	//-----------------------------------------------------------------------------------------------------------------------------
	namespace detail {

		template < eGROUP_VALUE_TYPE e, typename TUserDefined = void >
		struct TGroupValueTypeByEnum {
			using implicit_value_type =
				std::conditional_t<e == eGROUP_VALUE_TYPE::boolean, bool,
				std::conditional_t<e == eGROUP_VALUE_TYPE::i16, int16,
				std::conditional_t<e == eGROUP_VALUE_TYPE::i32, int32,
				std::conditional_t<e == eGROUP_VALUE_TYPE::i64, int64,
				std::conditional_t<e == eGROUP_VALUE_TYPE::dbl, double,
				std::conditional_t<e == eGROUP_VALUE_TYPE::str, string_t,
				std::conditional_t<e == eGROUP_VALUE_TYPE::hex_data, binary_t, void>
				>>>>>>;

			using value_type = std::conditional_t<std::is_same_v<TUserDefined, void>, implicit_value_type, TUserDefined>;

			static_assert(sizeof(value_type) == sizeof(implicit_value_type));
		};
	}

	template < eGROUP_VALUE_TYPE e, typename TUserDefined = void >
	using enum_to_value_t = typename detail::TGroupValueTypeByEnum<e, TUserDefined>::value_type;

	template < group_code_t code, typename TUserDefined = void >
	using code_to_value_t = enum_to_value_t<sGroup::GET_VALUE_TYPE_ENUM(code), TUserDefined>;


	//=============================================================================================================================
	template < typename T >
	struct TGroupHandler;
	//-----------------------------------------------------------------------------------------------------------------------------
	template < typename TItem >
	bool ReadItemSingleMember(TItem& item, sGroup const& group, size_t& index) {
		if constexpr (requires (TItem v) { v.base(); }) {
			if (ReadItemSingleMember<TItem::base_t>(item.base(), group, index))
				return true;
		}
		constexpr static size_t nTupleSize = std::tuple_size_v<decltype(TGroupHandler<TItem>::handlers)>/2;
		return ForEachIntSeq<nTupleSize>([&]<int I>{
			constexpr int code = std::get<I*2>(TGroupHandler<TItem>::handlers);
			constexpr auto const offset_ptr = std::get<I*2+1>(TGroupHandler<TItem>::handlers);
			if (group.iGroupCode != code)
				return false;

			if (index > 0) {
				index--;
				return false;
			}

			if constexpr (std::is_member_object_pointer_v<decltype(offset_ptr)>) {
				if constexpr (std::is_enum_v<std::remove_cvref_t<decltype(item.*offset_ptr)>>) {
					using underlying_t = std::underlying_type_t<std::remove_cvref_t<decltype(item.*offset_ptr)>>;
					group.GetValue((underlying_t&)(item.*offset_ptr));
				}
				else {
					group.GetValue(item.*offset_ptr);
				}
				return true;
			}
			else if constexpr (std::invocable<decltype(offset_ptr), TItem&, sGroup const&>) {
				return offset_ptr(item, group);
			}
			else if constexpr (std::invocable<decltype(offset_ptr), TItem&>) {
				using value_t = std::invoke_result_t<decltype(offset_ptr), TItem&>;
				if constexpr (std::is_enum_v<value_t>) {
					using underlying_t = std::underlying_type_t<value_t>;
					group.GetValue((underlying_t&)offset_ptr(item));
				}
				else {
					group.GetValue(offset_ptr(item));
				}
				return true;
			}
		});
	}
	//-----------------------------------------------------------------------------------------------------------------------------
	template < typename TItem >
	bool ReadItemSingleMember(TItem& item, sGroup const& group) {
		size_t index{};
		return ReadItemSingleMember(item, group, index);
	}

}	// namespace biscuit::dxf

