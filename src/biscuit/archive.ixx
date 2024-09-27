module;

//////////////////////////////////////////////////////////////////////
//
// archive.ixx: TArchive : binary in/out, string encoding converting, Read/Write Text Strings (with encodings...)
//
// PWH
// 2018.12.10.
// 2019.07.24. QL -> GTL
// 2019.10.22. 2019.11.22. archive --> iarchive / oarchive 분리
// 2020.11.30. GTL.2020
// 2024-09-20. biscuit
//
// stream helper
//
//////////////////////////////////////////////////////////////////////

#include "biscuit/config.h"
#include "biscuit/macro.h"

#include "biscuit/dependencies_fmt.h"

export module biscuit.archive;
import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.string;
import biscuit.codepage;
import biscuit.convert_codepage;
import biscuit.iconv_wrapper;
import biscuit.misc;

namespace concepts = biscuit::concepts;

export namespace biscuit {

	//-----------------------------------------------------------------------------
	//

	enum class eARCHIVE_BYTE_ORDERING {
		host,
		network
	};

	/// @brief TArchive : class for file io.
	///   i) Reads line by line for text file.
	///  ii) UTF Encoding/Decoding. ByteSwapping...
	/// @tparam stream_t : any of std::basic_stream
	/// @tparam bSTORE : is a storing stream
	/// @tparam bLOAD : is a loading stream
	/// @tparam bSWAP_BYTE_ORDER : swap bytes order for integral types. (ex, for network stream)
	template < std::derived_from<std::ios_base> tstream,
		eARCHIVE_BYTE_ORDERING eBYTE_ORDERING = eARCHIVE_BYTE_ORDERING::host,
		bool bSTORE = std::is_base_of_v<std::basic_ostream<typename tstream::char_type>, tstream>,
		bool bLOAD = std::is_base_of_v<std::basic_istream<typename tstream::char_type>, tstream>
	>
	class TArchive {

	public:
		constexpr static inline bool is_storing_only = bSTORE && !bLOAD;
		constexpr static inline bool is_loading_only = !bSTORE && bLOAD;
		constexpr static inline bool is_storing = bSTORE;
		constexpr static inline bool is_loading = bLOAD;
		constexpr static inline eARCHIVE_BYTE_ORDERING byte_ordering = eBYTE_ORDERING;
		constexpr static inline bool swap_byte_order = (byte_ordering == eARCHIVE_BYTE_ORDERING::network) && (std::endian::native == std::endian::little);

	protected:
		std::optional<tstream> m_streamInternal;
		eCODEPAGE m_eCodepage = eCODEPAGE_DEFAULT<typename tstream::char_type>;

	public:
		tstream& m_stream;
		bool const m_bStore;

	public:
		TArchive() = delete;
		TArchive(TArchive const&) = delete;
		TArchive(TArchive&&) = delete;
		TArchive& operator = (TArchive const&) = delete;
		TArchive& operator = (TArchive&&) = delete;

		explicit TArchive(tstream& stream) requires (is_storing_only || is_loading_only)
			: m_stream{stream}, m_bStore{is_storing}
		{ }
		TArchive(tstream& stream, bool bStore) requires (is_storing && is_loading)
			: m_stream{stream}, m_bStore{bStore}
		{ }

		TArchive(std::filesystem::path const& path, std::ios_base::openmode mode = std::ios_base::binary)
			requires (is_storing_only || is_loading_only)
			: m_streamInternal{tstream{path, std::ios_base::binary|mode}}, m_stream(m_streamInternal.value()), m_bStore(is_storing)
		{ }
		TArchive(std::filesystem::path const& path, bool bStore, std::ios_base::openmode mode = std::ios_base::binary)
			requires (is_storing && is_loading)
			: m_streamInternal{tstream{path, std::ios_base::binary|mode}}, m_stream(m_streamInternal.value()), m_bStore(bStore)
		{ }

	public:
		/// @brief Set/Get Codepage.
		eCODEPAGE SetCodepage(eCODEPAGE eCodepage) { return std::exchange(m_eCodepage, eCodepage); }
		eCODEPAGE GetCodepage() const { return m_eCodepage; }

	public:
		/// @brief Write byte buffer
		inline void Write(void const* data, std::streamsize size) requires (is_storing) {
			CheckArchiveStorable();
			m_stream.write((typename tstream::char_type const*)data, size);
		}

		/// @brief Write an object.
		/// @param tvalue trivially_copyable objects. (for integral value, byte ordering can be applied)
		template < concepts::trivially_copyable tvalue >
		inline void Write(tvalue const& v) requires (is_storing) {
			CheckArchiveStorable();
			if constexpr (swap_byte_order and std::is_integral_v<tvalue>) {
				auto v2 = std::byteswap(v);
				m_stream.write((typename tstream::char_type const*)&v2, sizeof(v2));
			} else {
				m_stream.write((typename tstream::char_type const*)&v, sizeof(v));
			}
		}

		template < std::ranges::contiguous_range trange >
			requires (std::is_trivially_copyable_v<std::ranges::range_value_t<std::remove_cvref_t<trange>>>)
		inline void WriteRange(trange const& range) requires (is_storing) {
			CheckArchiveStorable();
			using value_t = std::ranges::range_value_t<std::remove_cvref_t<trange>>;
			using char_t = typename tstream::char_type;
			if constexpr (swap_byte_order and std::is_integral_v<value_t> and sizeof(value_t) >= 2) {
				for (auto const& v : range) {
					auto v2 = std::byteswap(v);
					m_stream.write((char_t const*)&v2, sizeof(v2));
				}
			}
			else {
				m_stream.write((char_t const*)std::data(range), std::size(range)*sizeof(value_t));
			}
		}


	public:
		/// @brief Read byte buffer
		inline std::streamsize Read(void* data, std::streamsize size) requires (is_loading) {
			CheckArchiveLoadable();
			auto pos0 = m_stream.tellg();
			if (!m_stream.read((typename tstream::char_type*)data, size))
				return -1;
//#ifdef _DEBUG
//			auto pos1 = m_stream.tellg();
//			auto nRead = pos1 - pos0;
//			return nRead;
//#else
			return (m_stream.tellg() - pos0);
//#endif
		}

		/// @brief Write an object.
		/// @param tvalue trivially_copyable objects. (when bSWAP_BYTE_ORDER is true, only integral or floating point value and their array  can be serialized.)
		template < concepts::trivially_copyable tvalue >
		inline std::streamsize Read(tvalue& v) requires (is_loading) {
			using value_t = std::remove_cvref_t<tvalue>;
			CheckArchiveLoadable();
			if constexpr (std::is_integral_v<value_t> and swap_byte_order) {
				value_t v2{};
				auto r = Read(&v2, sizeof(v2));
				v = std::byteswap(v2);
				return r;
			}
			else {
				return Read(&v, sizeof(v));
			}
		}

		template < std::ranges::contiguous_range trange >
			requires (std::is_trivially_copyable_v<std::ranges::range_value_t<std::remove_cvref_t<trange>>>)
		inline std::streamsize ReadRange(trange const& range) requires (is_loading) {
			CheckArchiveLoadable();
			using value_t = std::ranges::range_value_t<std::remove_cvref_t<trange>>;
			using char_t = typename tstream::char_type;
			auto pos0 = m_stream.tellg();
			auto r = Read(std::data(range), std::size(range)*sizeof(value_t));
			if (r <= 0)
				return r;
			if constexpr (swap_byte_order and std::is_integral_v<value_t> and sizeof(value_t) >= 2) {
				for (auto& v : range)
					v = std::byteswap(v);
			}
			return r;
		}

	public:
		//---------------------------------------------------------------------
		// operator >>, <<, &
		template < concepts::trivially_copyable tvalue >
		TArchive& operator << (tvalue const& v) requires (is_storing) {
			Write(v);
			return *this;
		};
		template < concepts::trivially_copyable tvalue >
		TArchive& operator >> (tvalue& v) requires (is_loading) {
			Read(v);
			return *this;
		};
		template < concepts::trivially_copyable tvalue >
		TArchive& operator & (tvalue& v) {
			if constexpr (is_storing && is_loading) {
				if (m_bStore)
					return *this << v;
				else
					return *this >> v;
			} else if constexpr (is_storing) {
				return *this << v;
			} else if constexpr (is_loading) {
				return *this >> v;
			} else {
				static_assert(false, "no way");
				return *this;
			}
		};
		template < typename tvalue >
		TArchive& operator & (tvalue const& v) requires (is_storing) {
			CheckArchiveStorable();
			return *this << v;
		};

		//---------------------------------------------------------------------

		/// @brief Write BOM
		void WriteCodepageBOM(eCODEPAGE eCodepage) requires (is_storing) {
			CheckArchiveStorable();
			if (eCodepage == eCODEPAGE::DEFAULT) {
				eCodepage = eCODEPAGE::UTF8;
			}
			std::string_view sv = GetCodepageBOM(eCodepage);
			if (sv.size() > 0)
				Write(sv.data(), sv.size());
			SetCodepage(eCodepage);
		};

		/// @brief Read BOM
		eCODEPAGE ReadCodepageBOM(eCODEPAGE eDefaultCodepage = eCODEPAGE::UTF8) requires (is_loading) {
			CheckArchiveLoadable();

			auto peek = [](tstream& stream, std::string_view sv) -> bool {
				std::ifstream s;
				auto pos = stream.tellg();
				bool bMatch {true};
				for (auto c : sv) {
					char ci{};
					if (!stream.get(ci)) {
						if (stream.eof())
							stream.clear(std::ios_base::eofbit);
						bMatch = false;
						break;
					}
					if (ci != c) {
						bMatch = false;
						break;
					}
				}
				if (bMatch)
					return true;
				stream.seekg(pos);
				return false;
			};

			constexpr static std::array<std::pair<std::string_view, eCODEPAGE>, 5> const codepages{{
				{GetCodepageBOM(eCODEPAGE::UTF8), eCODEPAGE::UTF8},
				{GetCodepageBOM(eCODEPAGE::UTF32LE), eCODEPAGE::UTF32LE},	// UTF32LE must precede UTF16LE
				{GetCodepageBOM(eCODEPAGE::UTF16LE), eCODEPAGE::UTF16LE},
				{GetCodepageBOM(eCODEPAGE::UTF16BE), eCODEPAGE::UTF16BE},
				{GetCodepageBOM(eCODEPAGE::UTF32BE), eCODEPAGE::UTF32BE},
			}};

			for (auto const& [sv, codepage] : codepages) {
				if (peek(m_stream, sv)) {
					SetCodepage(codepage);
					return codepage;
				}
			}

			SetCodepage(eDefaultCodepage);

			return eDefaultCodepage;
		};

	protected:
		template < concepts::string_elem tchar, tchar cDelimiter2 = (tchar)'\r' >
		std::optional<std::basic_string<tchar>> GetRawLine(tchar cDelimiter) requires (is_loading) {
			std::basic_string<tchar> str;

			static_assert(concepts::is_one_of<typename tstream::char_type, char, char8_t>);

			if (!m_stream or m_stream.eof())
				return {};

			for (tchar c{}; m_stream.read((char*)&c, sizeof(c)); ) {
				if (c == cDelimiter)
					break;
				str += c;
			}

			if (str.empty() && m_stream.eof())
				return {};

			if constexpr (cDelimiter2 != 0) {
				if (!str.empty() and str.back() == cDelimiter2)
					str.resize(str.size()-1);
			}

			return str;
		}

		template < eCODEPAGE eCodepageFrom, concepts::string_elem tchar_to, tchar_to cDelimiter2 = (tchar_to)'\n' >
		inline std::optional<std::basic_string<tchar_to>> TReadLine(tchar_to cDelimiter = (tchar_to)'\n') requires (is_loading) {
			using char_from_t = typename char_type_from<eCodepageFrom>::char_type;
			using char_to_t = std::remove_cvref_t<tchar_to>;
			if constexpr (std::is_same_v<char_to_t, char_from_t>
				or (sizeof(char_from_t) >= 2 and sizeof(char_to_t) == sizeof(char_from_t))
				)
			{
				constexpr bool bSwapStreamByteOrder = (sizeof(char_to_t) >= 2) and (eCodepageFrom == eCODEPAGE_OTHER_ENDIAN<char_to_t>);
				if constexpr (bSwapStreamByteOrder xor swap_byte_order) {
					auto r = GetRawLine<char_to_t, std::byteswap(cDelimiter2)>(std::byteswap<char_to_t>(cDelimiter));
					if (r) {
						for (auto& c : r.value())
							c = std::byteswap<char_to_t>(c);
					}
					return std::move(r);
				}
				else {
					return GetRawLine<char_to_t, cDelimiter2>(cDelimiter);
				}
			}
			else {
				if (auto r = TReadLine<eCodepageFrom, char_from_t, cDelimiter2>((char_from_t)cDelimiter); r) {
					if constexpr (eCodepageFrom == eCODEPAGE::DEFAULT and std::is_same_v<char_from_t, char>)
						return ConvertString_iconv<char_to_t>(*r, "", GetCodepageName(m_eCodepage));	// NOT eCodepageFrom
					else
						return ConvertString<char_to_t>(*r);
				}
				return {};
			}
		}

	public:
		/// @brief Read / Write String
		template < concepts::string_elem tchar_to, tchar_to cDelimiter2 = (tchar_to)'\r' >
		std::optional<std::basic_string<tchar_to>> ReadLine(tchar_to cDelimiter = '\n') requires (is_loading) {
			CheckArchiveLoadable();
			switch (m_eCodepage) {
			case eCODEPAGE::UTF8 :
				return TReadLine<eCODEPAGE::UTF8, tchar_to, cDelimiter2>(cDelimiter);
				break;

			case eCODEPAGE::UTF16LE :
				return TReadLine<eCODEPAGE::UTF16LE, tchar_to, cDelimiter2>(cDelimiter);
				break;
			case eCODEPAGE::UTF16BE :
				return TReadLine<eCODEPAGE::UTF16BE, tchar_to, cDelimiter2>(cDelimiter);
				break;

			case eCODEPAGE::UTF32LE :
				return TReadLine<eCODEPAGE::UTF32LE, tchar_to, cDelimiter2>(cDelimiter);
				break;
			case eCODEPAGE::UTF32BE :
				return TReadLine<eCODEPAGE::UTF32BE, tchar_to, cDelimiter2>(cDelimiter);
				break;

			case eCODEPAGE::DEFAULT :
			default :
				return TReadLine<eCODEPAGE::DEFAULT, tchar_to, cDelimiter2>(cDelimiter);
				break;

			}
		};

		template <char cDelimiter2 = '\r'>		inline auto	ReadLineA(char cDelimiter = '\n')			requires (is_loading) { return ReadLine<char, cDelimiter2>(cDelimiter); }
		template <char8_t cDelimiter2 = u8'\r'>	inline auto	ReadLineU8(char8_t cDelimiter = u8'\n')		requires (is_loading) { return ReadLine<char8_t, cDelimiter2>(cDelimiter); }
		template <char16_t cDelimiter2 = u'\r'>	inline auto	ReadLineU16(char16_t cDelimiter = u'\n')	requires (is_loading) { return ReadLine<char16_t, cDelimiter2>(cDelimiter); }
		template <char32_t cDelimiter2 = U'\r'>	inline auto	ReadLineU32(char32_t cDelimiter = U'\n')	requires (is_loading) { return ReadLine<char32_t, cDelimiter2>(cDelimiter); }
		template <wchar_t cDelimiter2 = L'\r'>	inline auto	ReadLineW(wchar_t cDelimiter = L'\n')		requires (is_loading) { return ReadLine<wchar_t, cDelimiter2>(cDelimiter); }

	protected:
		template < concepts::tstring_like tstring >
		inline void PutStringRaw(tstring const& str) requires (is_storing) {
			using char_t = concepts::value_t<tstring>;
			static_assert(concepts::is_one_of<typename tstream::char_type, char, char8_t>);
			//if (!m_stream)
			//	return {};
			m_stream.write((typename tstream::char_type const*)std::data(str), std::size(str)*sizeof(char_t));
		}

		template < eCODEPAGE eCodepage, concepts::tstring_like tstring >
		void PutStringCodepage(tstring const& sv) requires (is_storing) {
			using char_from_t = concepts::value_t<tstring>;
			using char_archive_t = typename char_type_from<eCodepage>::char_type;

			if constexpr (std::is_same_v<char_archive_t, char_from_t>
				or (sizeof(char_from_t) >= 2 and sizeof(char_archive_t) == sizeof(char_from_t))
				)
			{
				constexpr bool bSwapStreamByteOrder = (sizeof(char_archive_t) >= 2) and (eCodepage == eCODEPAGE_OTHER_ENDIAN<char_archive_t>);
				if constexpr (bSwapStreamByteOrder xor swap_byte_order) {
					std::basic_string<char_from_t> str2;
					str2.reserve(sv.size());
					std::ranges::transform(sv, std::back_inserter(str2), [](auto c) { return std::byteswap(c); });
					PutStringRaw(str2);
				}
				else {
					PutStringRaw(sv);
				}
			}
			else {
				if constexpr (!swap_byte_order and concepts::string_elem_utf<char_archive_t> and concepts::string_elem_utf<char_from_t>) {
					if (auto str2 = ConvertString<char_archive_t>(sv); str2) {
						PutStringRaw(*str2);
					}
					else {
						throw std::ios_base::failure(BSC__FUNCSIG "ConvertString failed");
					}
				}
				else {
					if (auto str2 = ConvertString_iconv<char_archive_t>(sv, GetCodepageName(m_eCodepage), ""); str2) {
						PutStringRaw(*str2);
					}
					else {
						throw std::ios_base::failure(BSC__FUNCSIG "ConvertString_iconv failed");
					}
				}
			}
		}

	public:
		template < xStringLiteral eol = "\r\n" >
		void PutNewLine() requires (is_storing) {
			switch (m_eCodepage) {
			case eCODEPAGE::UTF16:
				{
					constexpr static TStringLiteral<char16_t, eol, swap_byte_order> const str;
					PutStringRaw(str.value);
				}
				break;
			case eCODEPAGE::UTF16_OE:
				{
					constexpr static TStringLiteral<char16_t, eol, !swap_byte_order> const str;
					PutStringRaw(str.value);
				}
				break;
			case eCODEPAGE::UTF32:
				{
					constexpr static TStringLiteral<char32_t, eol, swap_byte_order> const str;
					PutStringRaw(str.value);
				}
				break;
			case eCODEPAGE::UTF32_OE:
				{
					constexpr static TStringLiteral<char32_t, eol, !swap_byte_order> const str;
					PutStringRaw(str.value);
				}
				break;
			case eCODEPAGE::DEFAULT:
			case eCODEPAGE::UTF8:
			default:
				PutStringRaw("\r\n");
				break;
			}
		}

		/// @brief Read / Write String
		template < concepts::tstring_like tstring >
		void PutString(tstring const& sv) requires (is_storing) {
			CheckArchiveStorable();

			switch (m_eCodepage) {
			case eCODEPAGE::UTF8:
				PutStringCodepage<eCODEPAGE::UTF8>(sv);
				break;

			case eCODEPAGE::UTF16LE:
				PutStringCodepage<eCODEPAGE::UTF16LE>(sv);
				break;
			case eCODEPAGE::UTF16BE:
				PutStringCodepage<eCODEPAGE::UTF16BE>(sv);
				break;

			case eCODEPAGE::UTF32LE:
				PutStringCodepage<eCODEPAGE::UTF32LE>(sv);
				break;
			case eCODEPAGE::UTF32BE:
				PutStringCodepage<eCODEPAGE::UTF32BE>(sv);
				break;

			case eCODEPAGE::DEFAULT:
			default :
				PutStringCodepage<eCODEPAGE::DEFAULT>(sv);
				break;
				//std::format("{}", 0);
			}
		};

		template < concepts::tstring_like tstring >
		void PutLine(tstring const& sv) requires (is_storing) {
			PutString(sv);
			PutNewLine();
		}

	public:
		template < typename tchar, typename ... targs >
		inline void TWriteString(fmt::basic_format_string<tchar, targs...> sv, targs&& ... args) requires (is_storing) {
			CheckArchiveStorable();
			std::basic_string<tchar> str = fmt::vformat(sv, std::forward<targs>(args)...);
			WriteString(str);
		}
		template < typename ... targs > inline void WriteString(fmt::basic_format_string<char, targs ...> sv, targs&& ... args) requires (is_storing) { TWriteString<char, targs ...>(sv, args...); }
		template < typename ... targs > inline void WriteString(fmt::basic_format_string<char8_t, targs ...> sv, targs&& ... args) requires (is_storing) { TWriteString<char8_t, targs ...>(sv, args...); }
		template < typename ... targs > inline void WriteString(fmt::basic_format_string<char16_t, targs ...> sv, targs&& ... args) requires (is_storing) { TWriteString<char16_t, targs ...>(sv, args...); }
		template < typename ... targs > inline void WriteString(fmt::basic_format_string<char32_t, targs ...> sv, targs&& ... args) requires (is_storing) { TWriteString<char32_t, targs ...>(sv, args...); }
		template < typename ... targs > inline void WriteString(fmt::basic_format_string<wchar_t, targs ...> sv, targs&& ... args) requires (is_storing) { TWriteString<wchar_t, targs ...>(sv, args...); }

		template < typename tchar, typename ... targs >
		inline void TWriteLine(fmt::basic_format_string<tchar, targs...> sv, targs&& ... args) requires (is_storing) {
			CheckArchiveStorable();
			std::basic_string<tchar> str = fmt::vformat(sv, std::forward<targs>(args)...);
			WriteString(str);
			PutNewLine();
		}
		template < typename ... targs > inline void WriteLine(fmt::basic_format_string<char, targs ...> sv, targs&& ... args) requires (is_storing) { TWriteLine<char, targs ...>(sv, args...); }
		template < typename ... targs > inline void WriteLine(fmt::basic_format_string<char8_t, targs ...> sv, targs&& ... args) requires (is_storing) { TWriteLine<char8_t, targs ...>(sv, args...); }
		template < typename ... targs > inline void WriteLine(fmt::basic_format_string<char16_t, targs ...> sv, targs&& ... args) requires (is_storing) { TWriteLine<char16_t, targs ...>(sv, args...); }
		template < typename ... targs > inline void WriteLine(fmt::basic_format_string<char32_t, targs ...> sv, targs&& ... args) requires (is_storing) { TWriteLine<char32_t, targs ...>(sv, args...); }
		template < typename ... targs > inline void WriteLine(fmt::basic_format_string<wchar_t, targs ...> sv, targs&& ... args) requires (is_storing) { TWriteLine<wchar_t, targs ...>(sv, args...); }

		//---------------------------------------------------------------------
		// Read / Write Size
		template < size_t nMinItemSize = 4 > requires (is_loading)
		size_t LoadFlexSize() {
			static_assert(std::ranges::any_of({1, 2, 4, sizeof(size_t)}, nMinItemSize));
			CheckArchiveLoadable();
			auto& ar = *this;
			union {
				uint8_t u8;
				uint16_t u16;
				uint32_t u32;
				uint64_t u64;
			} size;
			if constexpr (nMinItemSize <= 1) {
				if (!(ar >> size.u8).m_streamInternal)
					throw std::ios_base::failure(BSC__FUNCSIG "Stream exception");
				if (size.u8 < 0xff)
					return size.u8;
			}
			if constexpr (nMinItemSize <= 2) {
				if (!(ar >> size.u16).m_streamInternal)
					throw std::ios_base::failure(BSC__FUNCSIG "Stream exception");
				if (size.u8 < 0xffff)
					return size.u16;
			}
			if constexpr (nMinItemSize <= 4) {
				if (!(ar >> size.u32).m_streamInternal)
					throw std::ios_base::failure(BSC__FUNCSIG "Stream exception");

				if (size.u32 < 0xffff'ffff)
					return size.u32;
				if constexpr (sizeof(size_t) == sizeof(size.u32)) {
					throw std::ios_base::failure(BSC__FUNCSIG "Stream Size is too big");
				}
			}
			if constexpr (sizeof(size_t) >= sizeof(size.u64)) {
				if (!(ar >> size.u64).m_streamInternal)
					throw std::ios_base::failure(BSC__FUNCSIG "Stream exception");
				return size.u64;
			}
		}
		template < size_t nMinItemSize = 4 > requires (is_storing)
		TArchive& StoreFlexSize(size_t size) {
			static_assert(std::ranges::any_of({1, 2, 4, sizeof(size_t)}, nMinItemSize));
			CheckArchiveStorable();
			auto& ar = *this;
			if constexpr (nMinItemSize <= 1) {
				if (size < 0xff) {
					ar << (uint8_t) size;
					return *this;
				} else {
					ar << (uint8_t) 0xff;
				}
			}
			if constexpr (nMinItemSize <= 2) {
				if (size < 0xffff) {
					ar << (uint16_t) size;
					return *this;
				} else {
					ar << (uint16_t) 0xffff;
				}
			}
			if constexpr (nMinItemSize <= 4) {
				if constexpr (sizeof(size_t) >= sizeof(uint64_t)) {
					if (size < 0xffff'ffffull) {
						ar << (uint32_t) size;
					} else {
						ar << (uint32_t) 0xffff'ffff;
						ar << (uint64_t) size;
					}
				} else {
					ar << (uint32_t) size;
				}
			}
			return *this;
		}


		//---------------------------------------------------------------------
		// Flush / Close
		void Flush() {
			m_stream.flush();
		}
		void Close() {
			m_stream.close();
		}

		//-----------------------------------------------------------------------------
		// std::pair
		template < typename T1, typename T2 > requires (is_storing)
		TArchive& operator << (const std::pair<T1, T2>& p) {
			CheckArchiveStorable();
			return *this << p.first << p.second;
		}
		template < typename T1, typename T2 > requires (is_loading)
		TArchive& operator >> (std::pair<T1, T2>& p) {
			CheckArchiveLoadable();
			return *this >> p.first >> p.second;
		}
		template < typename T1, typename T2 >
		TArchive& operator & (std::pair<T1, T2>& p) {
			return (*this) & p.first & p.second;
		}
		template < typename T1, typename T2 > requires (is_storing)
		TArchive& operator & (std::pair<T1, T2> const& p) {
			CheckArchiveStorable();
			return *this << (p);
		}

	protected:
		constexpr inline void CheckArchiveLoadable() {
			if constexpr (is_storing && is_loading) {
				if (m_bStore) {
					throw std::ios_base::failure(BSC__FUNCSIG "NOT an archive for loading!");
				}
			}
		}
		constexpr inline void CheckArchiveStorable() {
			if constexpr (is_storing && is_loading) {
				if (!m_bStore) {
					throw std::ios_base::failure(BSC__FUNCSIG "NOT an archive for storing!");
				}
			}
		}


	};


	//-------------------------------------------------------------------------

	using xIFArchive = TArchive<std::ifstream>;
	using xOFArchive = TArchive<std::ofstream>;
	using xArchive = TArchive<std::fstream>;//, eARCHIVE_BYTE_ORDERING::host, true, true>;

	/// @brief file to std-container
	/// @tparam TContainer : such as std::vector<char> or std::string
	/// @param path 
	/// @return 
	template < std::ranges::contiguous_range TContainer = std::vector<char> >
		requires (std::is_trivially_copyable_v<std::ranges::range_value_t<TContainer>>)
	std::optional<TContainer> FileToContainer(std::filesystem::path const& path) {
		using T = typename TContainer::value_type;
		std::optional<TContainer> r;
		std::ifstream f(path, std::ios_base::binary);
		if (!f.seekg(0, std::ios_base::end))
			return r;
		auto len = f.tellg();
		if (len < 0)
			return r;
		if (len == 0) {
			r.emplace();
			return r;
		}
		auto nItem = len / sizeof(T);
		r.emplace((size_t)nItem, T{});
		f.seekg(0, std::ios_base::beg);
		f.read((char*)r->data(), r->size()*sizeof(T));
		return r;
	}

	template < typename T = char > requires (std::is_trivial_v<T>)
	constexpr std::optional<std::vector<T>> FileToVector(std::filesystem::path const& path) {
		return FileToContainer<std::vector<T>>(path);
	}

	template < typename T = char > requires (std::is_trivial_v<T>)
	constexpr std::optional<std::basic_string<T>> FileToString(std::filesystem::path const& path) {
		return FileToContainer<std::basic_string<T>>(path);
	}

	/// @brief 
	/// @tparam TContainer 
	/// @param  
	/// @param path 
	/// @return 
	template < std::ranges::contiguous_range TContainer >
		requires (std::is_trivially_copyable_v<std::ranges::range_value_t<std::remove_cvref_t<TContainer>>>)
	bool ContainerToFile(TContainer const& buf, std::filesystem::path const& path) {
		static_assert(std::is_trivial_v<typename std::decay_t<decltype(buf)>::value_type>, "TContainer::value_type must be trivial");
		std::ofstream f(path, std::ios_base::binary);
		f.write((char const*)buf.data(), buf.size() * sizeof(typename TContainer::value_type));
		return (bool)f;
	}

}	// namespace biscuit;

