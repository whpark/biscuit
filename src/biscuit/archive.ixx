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

export module biscuit.archive;
import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.string;
import biscuit.codepage;
import biscuit.convert_codepage;
import biscuit.iconv_wrapper;

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
				m_stream.write((typename tstream::char_type const*)&v2, sizeof(v2));
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
#ifdef _DEBUG
			auto pos1 = m_stream.tellg();
			auto nRead = pos1 - pos0;
			return nRead;
#else
			return (stream_.tellg() - pos0);
#endif
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
				if (IsStoring())
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
			unsigned char c = 0;

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
		template < typename tchar > requires (is_loading)
		std::optional<std::basic_string<tchar>> GetLine(tchar cDelimiter, tchar cDelimiter2 = 0/*'\r'*/) {
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

			if (cDelimiter2 and !str.empty() and str.back() == cDelimiter2)
				str.resize(str.size()-1);

			return str;
		}

		template < eCODEPAGE eCodepage, concepts::string_elem tchar >
		inline std::optional<std::basic_string<tchar>> TReadLine(tchar cDelimiter = (tchar)'\n', bool bTrimCR = true) requires (is_loading) {
			using char_codepage_t = typename char_type_from<eCodepage>::char_type;
			using char_target_t = tchar;
			if constexpr (std::is_same_v<std::remove_cvref_t<char_target_t>, char_codepage_t>
				or (std::is_same_v<std::remove_cvref_t<char_target_t>, wchar_t> and (sizeof(char_target_t) == sizeof(char_codepage_t)))
				)
			{
				constexpr bool bSwapStreamByteOrder = (sizeof(char_target_t) >= 2) and (eCodepage == eCODEPAGE_OTHER_ENDIAN<char_target_t>);
				constexpr bool bSwapByteOrder = bSwapStreamByteOrder xor swap_byte_order;
				if constexpr (bSwapByteOrder) {
					auto r = GetLine<char_target_t>(std::byteswap<char_target_t>(cDelimiter), bTrimCR ? std::byteswap<char_target_t>('\r') : char_target_t{});
					if (r) {
						for (auto& c : r.value())
							c = std::byteswap<char_target_t>(c);
					}
					return r;
				}
				else {
					return GetLine<char_target_t>(cDelimiter, bTrimCR ? (char_target_t)'\r' : char_target_t{});
				}
			}
			else {
				auto r = TReadLine<eCodepage, char_codepage_t>((char_codepage_t)cDelimiter, bTrimCR);
				if (r) {
					Ticonv<char_target_t, char_codepage_t> iconv(nullptr), GetCodepageName(m_eCodepage));
					return iconv.Convert(*r);
				}
				return r;
			}
		}

	public:
		/// @brief Read / Write String
		template < typename tchar > requires (is_loading)
		std::optional<std::basic_string<tchar>> ReadLine(tchar cDelimiter = '\n', bool bTrimCR = true) {
			CheckArchiveLoadable();
			auto eCodepageTarget = eCODEPAGE_DEFAULT<tchar>;
			// todo: here.........
			if (m_eCodepage == eCodepageTarget) {
				
			}
			else {

			}

			switch (m_eCodepage) {

			case eCODEPAGE::UTF8 :
				return TReadLine<eCODEPAGE::UTF8, tchar>(cDelimiter, bTrimCR);
				break;

			case eCODEPAGE::UTF16LE :
				return TReadLine<eCODEPAGE::UTF16LE, tchar>(cDelimiter, bTrimCR);
				break;
			case eCODEPAGE::UTF16BE :
				return TReadLine<eCODEPAGE::UTF16BE, tchar>(cDelimiter, bTrimCR);
				break;

			case eCODEPAGE::UTF32LE :
				return TReadLine<eCODEPAGE::UTF32LE, tchar>(cDelimiter, bTrimCR);
				break;
			case eCODEPAGE::UTF32BE :
				return TReadLine<eCODEPAGE::UTF32BE, tchar>(cDelimiter, bTrimCR);
				break;

			case eCODEPAGE::DEFAULT :
			default :
				return TReadLine<eCODEPAGE::DEFAULT, tchar>(cDelimiter, bTrimCR);
				break;

			}
		};

		inline std::optional<std::string>		ReadLineA(char cDelimiter = '\n', bool bTrimCR = true)			requires (is_loading) { return ReadLine<char>(cDelimiter, bTrimCR); }
		inline std::optional<std::u8string>		ReadLineU8(char8_t cDelimiter = u8'\n', bool bTrimCR = true)	requires (is_loading) { return ReadLine<char8_t>(cDelimiter, bTrimCR); }
		inline std::optional<std::u16string>	ReadLineU16(char16_t cDelimiter = u'\n', bool bTrimCR = true)	requires (is_loading) { return ReadLine<char16_t>(cDelimiter, bTrimCR); }
		inline std::optional<std::u32string>	ReadLineU32(char32_t cDelimiter = U'\n', bool bTrimCR = true)	requires (is_loading) { return ReadLine<char32_t>(cDelimiter, bTrimCR); }
		inline std::optional<std::wstring>		ReadLineW(wchar_t cDelimiter = L'\n', bool bTrimCR = true)		requires (is_loading) { return ReadLine<wchar_t>(cDelimiter, bTrimCR); }

	protected:
		template < eCODEPAGE eCodepage, concepts::string_elem tchar > requires (is_storing)
		void TWriteLine(std::basic_string_view<tchar> sv, tchar cDelimiter = '\n', bool bAddCR = true) {
			using char_codepage_t = typename char_type_from<eCodepage>::char_type;
			if constexpr (!concepts::is_same_utf<tchar, char_codepage_t>) {
				constexpr auto eCodepageNative = eCODEPAGE_DEFAULT<char_codepage_t>;
				S_CODEPAGE_OPTION codepage{
					.from = std::is_same_v<tchar, char> ? g_eCodepageMBCS : eCODEPAGE::DEFAULT,
					.to = std::is_same_v<char_codepage_t, char> ? m_eCodepage : eCodepageNative
				};
				auto str = ToString<char_codepage_t, tchar, false>(sv, codepage);
				return TWriteLine<eCodepage, char_codepage_t>(str, static_cast<char_codepage_t>(cDelimiter), bAddCR);
			}
			else {

				CheckArchiveStorable();

				constexpr bool bSwapStreamByteOrder = (eCodepage == eCODEPAGE_OTHER_ENDIAN<tchar>);
				constexpr bool bSwapByteOrder = (bSwapStreamByteOrder xor bSWAP_BYTE_ORDER);
				static tchar const cr {'\r'};
				if constexpr (bSwapByteOrder and (sizeof(tchar) >= 2)) {
					WriteIntsSwapByte<tchar>(sv.data(), sv.size());
					if (cDelimiter) {
						if (bAddCR) {
							WriteIntsSwapByte<tchar>(&cr, 1);
						}
						WriteIntsSwapByte<tchar>(&cDelimiter, 1);
					}
				}
				else {
					Write(sv.data(), sv.size()*sizeof(tchar));
					if (cDelimiter) {
						if (bAddCR) {
							Write(&cr, sizeof(cr));
						}
						Write(&cDelimiter, sizeof(cDelimiter));
					}
				}
			}

		}

	public:
		/// @brief Read / Write String
		template < concepts::string_elem tchar > requires (is_storing)
		void TWriteLine(std::basic_string_view<tchar> sv, tchar cDelimiter = '\n', bool bAddCR = true) {
			CheckArchiveStorable();

			switch (m_eCodepage) {

			case eCODEPAGE::UTF8 :
				return TWriteLine<eCODEPAGE::UTF8, tchar>(sv, cDelimiter, bAddCR);
				break;

			case eCODEPAGE::UTF16LE :
				return TWriteLine<eCODEPAGE::UTF16LE, tchar>(sv, cDelimiter, bAddCR);
				break;
			case eCODEPAGE::UTF16BE :
				return TWriteLine<eCODEPAGE::UTF16BE, tchar>(sv, cDelimiter, bAddCR);
				break;

			case eCODEPAGE::UTF32LE :
				return TWriteLine<eCODEPAGE::UTF32LE, tchar>(sv, cDelimiter, bAddCR);
				break;
			case eCODEPAGE::UTF32BE :
				return TWriteLine<eCODEPAGE::UTF32BE, tchar>(sv, cDelimiter, bAddCR);
				break;

			case eCODEPAGE::DEFAULT :
			default :
				return TWriteLine<eCODEPAGE::DEFAULT, tchar>(sv, cDelimiter, bAddCR);
				break;

			}
		};

	public:
		template < typename ... targs > inline void WriteString(gtl::internal::tformat_string<char, targs...> const& sv, targs&& ... args) requires (is_storing) { CheckArchiveStorable(); TWriteLine<char>(Format(sv, std::forward<targs>(args)... ), {}, false); }
		template < typename ... targs > inline void WriteString(gtl::internal::tformat_string<char8_t, targs...> const& sv, targs&& ... args) requires (is_storing) { CheckArchiveStorable(); TWriteLine<char8_t>(Format(sv, std::forward<targs>(args)...), {}, false); }
		template < typename ... targs > inline void WriteString(gtl::internal::tformat_string<char16_t, targs...> const& sv, targs&& ... args) requires (is_storing) { CheckArchiveStorable(); TWriteLine<char16_t>(Format(sv, std::forward<targs>(args) ...), {}, false); }
		template < typename ... targs > inline void WriteString(gtl::internal::tformat_string<char32_t, targs...> const& sv, targs&& ... args) requires (is_storing) { CheckArchiveStorable(); TWriteLine<char32_t>(Format(sv, std::forward<targs>(args) ...), {}, false); }
		template < typename ... targs > inline void WriteString(gtl::internal::tformat_string<wchar_t, targs...> const& sv, targs&& ... args) requires (is_storing) { CheckArchiveStorable(); TWriteLine<wchar_t>(Format(sv, std::forward<targs>(args)... ), {}, false); }
		template < typename ... targs > inline void WriteLine(gtl::internal::tformat_string<char, targs...> const& sv, targs&& ... args) requires (is_storing) { CheckArchiveStorable(); TWriteLine<char>(Format(sv, std::forward<targs>(args)... )); }
		template < typename ... targs > inline void WriteLine(gtl::internal::tformat_string<char8_t, targs...> const& sv, targs&& ... args) requires (is_storing) { CheckArchiveStorable(); TWriteLine<char8_t>(Format(sv, std::forward<targs>(args)... )); }
		template < typename ... targs > inline void WriteLine(gtl::internal::tformat_string<char16_t, targs...> const& sv, targs&& ... args) requires (is_storing) { CheckArchiveStorable(); TWriteLine<char16_t>(Format(sv, std::forward<targs>(args) ...)); }
		template < typename ... targs > inline void WriteLine(gtl::internal::tformat_string<char32_t, targs...> const& sv, targs&& ... args) requires (is_storing) { CheckArchiveStorable(); TWriteLine<char32_t>(Format(sv, std::forward<targs>(args) ...)); }
		template < typename ... targs > inline void WriteLine(gtl::internal::tformat_string<wchar_t, targs...> const& sv, targs&& ... args) requires (is_storing) { CheckArchiveStorable(); TWriteLine<wchar_t>(Format(sv, std::forward<targs>(args)... )); }

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
					throw std::ios_base::failure(GTL__FUNCSIG "Stream exception");
				if (size.u8 < 0xff)
					return size.u8;
			}
			if constexpr (nMinItemSize <= 2) {
				if (!(ar >> size.u16).m_streamInternal)
					throw std::ios_base::failure(GTL__FUNCSIG "Stream exception");
				if (size.u8 < 0xffff)
					return size.u16;
			}
			if constexpr (nMinItemSize <= 4) {
				if (!(ar >> size.u32).m_streamInternal)
					throw std::ios_base::failure(GTL__FUNCSIG "Stream exception");

				if (size.u32 < 0xffff'ffff)
					return size.u32;
				if constexpr (sizeof(size_t) == sizeof(size.u32)) {
					throw std::ios_base::failure(GTL__FUNCSIG "Stream Size is too big");
				}
			}
			if constexpr (sizeof(size_t) >= sizeof(size.u64)) {
				if (!(ar >> size.u64).m_streamInternal)
					throw std::ios_base::failure(GTL__FUNCSIG "Stream exception");
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
		// Generic Archive In / Out
		//
		//template < typename T > TArchive& operator << (TArchive& ar, const std::vector<T>& container);
		//template < typename T > TArchive& operator >> (TArchive& ar, std::vector<T>& container);
		//template < typename T > TArchive& operator & (TArchive& ar, std::vector<T>& container);

		////-------------------------
		//// Container ... std::vector, std::deque
		//template < typename T, concepts::container<T> tcontainer > requires (is_storing)
		//TArchive& StoreContainer(tcontainer const& container) {
		//	CheckArchiveStorable();
		//	auto count = std::size(container);
		//	StoreSize(count);
		//	if (count) {
		//		constexpr bool bSwapMightBeNeeded = (bSWAP_BYTE_ORDER and (sizeof(T) > 1) and (std::is_integral_v<T> or std::is_class_v<T>));
		//		if constexpr (std::is_trivially_copyable_v<T> and !bSwapMightBeNeeded) {
		//			Write((uint8_t const*)std::data(container), std::size(container)*sizeof(T));
		//		} else {
		//			for (auto const& item : container) {
		//				(*this) << item;
		//			}
		//		}
		//	}
		//	return *this;
		//};
		//template < typename T, concepts::container<T> tcontainer > requires (is_loading)
		//TArchive& LoadContainer(tcontainer& container) {
		//	CheckArchiveLoadable();
		//	container.clear();
		//	size_t size = LoadSize();
		//	if (size) {
		//		constexpr bool bSwapMightBeNeeded = (bSWAP_BYTE_ORDER and (sizeof(T) > 1) and (std::is_integral_v<T> or std::is_class_v<T>));
		//		if constexpr (requires (tcontainer container) { container.resize(size_t{}); }) {
		//			container.resize(size);
		//		}
		//		if constexpr (std::is_trivially_copyable_v<T> and !bSwapMightBeNeeded) {
		//			Read((uint8_t const*)std::data(container), std::size(container)*sizeof(T));
		//		} else {
		//			for (auto const& item : container) {
		//				(*this) << item;
		//			}
		//		}
		//		if constexpr (std::is_trivially_copyable_v<T>) {
		//			container.resize(size);
		//			if constexpr (bSWAP_BYTE_ORDER and std::is_integral_v<T> and (sizeof(T) > 1)) {
		//				if (ReadInts(container.data(), container.size()) != container.size())
		//					throw std::ios_base::failure(GTL__FUNCSIG "CANNOT Read Data");
		//			} else {
		//				auto len = container.size()*sizeof(T);
		//				if (Read(container.data(), container.size()*sizeof(T)) != len)
		//					throw std::ios_base::failure(GTL__FUNCSIG "CANNOT Read Data");
		//			}
		//		} else {
		//			if constexpr (requires (tcontainer container) { container.reserve(size); }) {
		//				container.reserve(size);
		//			}
		//			for (size_t i = 0; i < size; i++) {
		//				if constexpr (requires (tcontainer container, T a) { container.push_back(a); }) {
		//					container.push_back({});
		//					*this >> container.back();
		//				} else if constexpr (requires (tcontainer container, T a) { container.insert(a); }) {
		//					T a;
		//					*this >> a;
		//					container.insert(std::move(a));
		//				} else {
		//					static_assert(false, "CONTAINER must have push_back or insert ...");
		//				}
		//			}
		//		}
		//	}
		//	return *this;
		//};

		//template < typename T, concepts::container<T> tcontainer > requires (is_storing)
		//TArchive& operator << (tcontainer const& data) {
		//	return StoreContainer(data);
		//};
		//template < typename T, concepts::container<T> tcontainer > requires (is_loading)
		//TArchive& operator >> (tcontainer& data) {
		//	return LoadContainer(data);
		//};
		//template < typename T, concepts::container<T> tcontainer >
		//TArchive& operator & (CONTAINER& data) {
		//	if constexpr (is_storing && is_loading) {
		//		if (IsStoring())
		//			return StoreContainer(data);
		//		else
		//			return LoadContainer(data);
		//	} else if constexpr (is_storing) {
		//		return StoreContainer(data);
		//	} else if constexpr (is_loading) {
		//		return LoadContainer(data);
		//	} else {
		//		//static_assert(false, "what???");
		//		return *this;
		//	}
		//};
		//template < gtl::is__container CONTAINER > requires (is_storing)
		//TArchive& operator & (const CONTAINER& data) {
		//	if constexpr (is_storing && is_loading) {
		//		if (!IsStoring()) {
		//			throw std::ios_base::failure(GTL__FUNCSIG "NOT an storing archive!");
		//			return *this;
		//		}
		//	}
		//	return StoreContainer(data);
		//};

		////-----------------------------------------------------------------------------
		//// std::unique_ptr
		//template < typename T > requires (is_storing)
		//TArchive& operator << (const std::unique_ptr<T>& ptr) {
		//	CheckArchiveStorable();
		//	if constexpr (concepts::is_dynamic_serializable<T>) {
		//		ptr->DynamicSerializeOut(*this);
		//	} else {
		//		(*this) & *ptr;
		//	}
		//	return *this;
		//}
		//template < typename T > requires (is_loading)
		//TArchive& operator >> (std::unique_ptr<T>& ptr) {
		//	CheckArchiveLoadable;
		//	if constexpr (concepts::is_dynamic_serializable<T>) {
		//		ptr = T::DynamicSerializeIn(*this);
		//	} else {
		//		if (!ptr) {
		//			if constexpr (has__NewObject<T>) {
		//				ptr = T::NewObject();
		//			} else {
		//				ptr.reset(new T);
		//			}
		//		}
		//		(*this) & *ptr;
		//	}
		//	return *this;
		//}
		//template < typename T >
		//TArchive& operator & (std::unique_ptr<T>& ptr) {
		//	if constexpr (is_storing && is_loading) {
		//		if (IsStoring())
		//			return *this << ptr;
		//		else
		//			return *this >> ptr;
		//	} else if constexpr (is_storing) {
		//		return *this << ptr;
		//	} else if constexpr (is_loading) {
		//		return *this >> ptr;
		//	} else {
		//		//static_assert(false, "what???");
		//		return *this;
		//	}
		//}
		//template < typename T > requires (is_storing)
		//TArchive& operator & (const std::unique_ptr<T>& ptr) {
		//	CheckArchiveStorable();
		//	return *this << ptr;
		//}


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

		////-----------------------------------------------------------------------------
		//// gtl::internal::pair
		//template < typename T1, typename T2 > requires (is_storing)
		//TArchive& operator << (gtl::internal::pair<T1, T2> const& p) {
		//	CheckArchiveStorable();
		//	return *this << p.first << p.second;
		//}
		//template < typename T1, typename T2 > requires (is_loading)
		//TArchive& operator >> (gtl::internal::pair<T1, T2>& p) {
		//	CheckArchiveLoadable();
		//	return *this >> p.first >> p.second;
		//}
		//template < typename T1, typename T2 >
		//TArchive& operator & (gtl::internal::pair<T1, T2>& p) {
		//	return (*this) & p.first & p.second;
		//}
		//template < typename T1, typename T2 > requires (is_storing)
		//TArchive& operator & (gtl::internal::pair<T1, T2> const& p) {
		//	CheckArchiveStorable();
		//	return *this << (p);
		//}

	public:
		////-----------------------------------------------------------------------------
		//// String In / Out
		////
		//template < concepts::string_elem tchar > requires (is_storing)
		//TArchive& operator << (const std::basic_string<tchar>& str) {
		//	CheckArchiveStorable();

		//	auto& ar = *this;
		//	if (str.size() == 0) {
		//		ar << (std::uint8_t)0;
		//		return ar;
		//	}

		//	// Encoding
		//	if constexpr (std::is_same_v<tchar, char>) {
		//	} else if constexpr (std::is_same_v<tchar, wchar_t>) {
		//		// eCHAR_ENCODING::UNICODE (little / big)
		//		ar & (std::uint8_t)0xff;
		//		ar & (std::uint16_t)0xfffe;
		//	} else if constexpr (std::is_same_v<tchar, char8_t>) {
		//		ar & (std::uint8_t)0xff;
		//		ar & (std::uint16_t)0xfffd;
		//	} else {
		//		static_assert(false, "only MBCS, UNICODE(little endian), UTF-8 types are supported.");
		//		return ar;
		//	}

		//	// Size
		//	if (str.size() < 255) {
		//		// less than a byte
		//		ar & (std::uint8_t)str.size();
		//	} else if (str.size() < 0xfffd) {
		//		// less than ( a word - additional one for utf8 )
		//		ar & (std::uint8_t)0xff;
		//		ar & (std::uint16_t)str.size();
		//	} else if (str.size() < 0xffff'ffff) {
		//		// less than 32bit-unsigned
		//		ar & (std::uint8_t)0xff;
		//		ar & (std::uint16_t)0xffff;
		//		ar & (std::uint32_t)str.size();
		//	} else {
		//		// less than 64 bit-unsigned ...
		//		ar & (std::uint8_t)0xff;
		//		ar & (std::uint16_t)0xffff;
		//		ar & (std::uint32_t)0xffff'ffff;
		//		ar & (std::uint64_t)str.size();
		//	}

		//	// Text
		//	if (ar.IsSwapByteOrder()) {
		//		ar.WriteInts(str.data(), str.size());
		//	} else {
		//		std::streamsize nLen = str.size()*sizeof(tchar);
		//		ar.Write((const std::ostream::char_type*)str.data(), nLen);
		//	}
		//	return ar;
		//}
		//template < concepts::string_elem tchar > requires (is_loading, concepts::is_one_of<tchar, )
		//TArchive& operator >> (std::basic_string<tchar>& str) {
		//	CheckArchiveLoadable();

		//	auto& ar = *this;

		//	union {	// only on little-endian
		//		uint8_t size8;
		//		uint16_t size16;
		//		uint32_t size32;
		//		uint64_t size64 {0};
		//	} len;
		//	eCHAR_ENCODING eEncSource = eCHAR_ENCODING::NONE;

		//	// read length, encoding
		//	do {
		//		ar & len.size8;
		//		if (len.size8 < 0xff) {
		//			if (eEncSource == eCHAR_ENCODING::NONE)
		//				eEncSource = eCHAR_ENCODING::MBCS;
		//			break;
		//		}

		//		ar & len.size16;
		//		if (len.size16 < 0xfffd) {
		//			break;
		//		}

		//		if (eEncSource == eCHAR_ENCODING::NONE) {
		//			if (len.size16 == 0xfffe) {
		//				eEncSource = eCHAR_ENCODING::UNICODE_LITTLE_ENDIAN;
		//			} else if (len.size16 == 0xfffd) {
		//				eEncSource = eCHAR_ENCODING::UTF8;
		//			}
		//			len.size16 = 0;
		//			continue;
		//		}

		//		ar & len.size32;
		//		if (len.size32 == 0xffff'ffff) {
		//			ar & len.size64;
		//			break;
		//		}
		//		break;

		//	} while (true);

		//	// read contents
		//	auto ReadAndConvertString = [&](auto ch) {
		//		std::basic_string<decltype(ch)> strSource;
		//		strSource.resize(len.size64);
		//		if (ar.IsSwapByteOrder()) {
		//			ar.ReadInts(strSource.data(), strSource.size());
		//		} else {
		//			ar.Read(strSource.data(), strSource.size()*sizeof(ch));
		//		}
		//		ConvCodepage_Move(str, std::move(strSource), ar.GetCodepage());
		//	};


		//	switch (eEncSource) {
		//	case eCHAR_ENCODING::MBCS :						ReadAndConvertString((char)0); break;
		//	case eCHAR_ENCODING::UNICODE_LITTLE_ENDIAN :	ReadAndConvertString((wchar_t)0); break;
		//	case eCHAR_ENCODING::UTF8 :						ReadAndConvertString((char8_t)0); break;
		//		//case eCHAR_ENCODING::UNICODE_BIG_ENDIAN :		// to do : TEST. (NOT Tested yet)
		//		//	{
		//		//		std::basic_string<wchar_t> strSource;
		//		//		strSource.resize(len.size64);
		//		//		if constexpr (bSwapByteOrder) {
		//		//			ar.Read(strSource.data(), strSource.size()*sizeof(wchar_t));
		//		//		} else {
		//		//			ar.ReadInts(strSource.data(), strSource.size(), true);
		//		//		}
		//		//		ConvCodepage_Move(str, std::move(strSource), ar.GetCodepage());
		//		//	}
		//		//	break;
		//	}
		//	return *this;
		//}

		//template <
		//	typename tchar,
		//	ENABLE_IF(std::is_same_v<tchar, char> || std::is_same_v<tchar, wchar_t> || std::is_same_v<tchar, char8_t>)
		//>
		//TArchive& operator & (std::basic_string<tchar>& str) {
		//	if constexpr (is_storing && is_loading) {
		//		if (IsStoring())
		//			return *this << str;
		//		else
		//			return *this >> str;
		//	} else if constexpr (is_storing) {
		//		return *this << str;
		//	} else if constexpr (is_loading) {
		//		return *this >> str;
		//	} else {
		//		//static_assert(false, "what???");
		//		return *this;
		//	}
		//}
		//template <
		//	typename tchar,
		//	ENABLE_IF(std::is_same_v<tchar, char> || std::is_same_v<tchar, wchar_t> || std::is_same_v<tchar, char8_t>)
		//> requires (is_storing)
		//TArchive& operator & (const std::basic_string<tchar>& str) {
		//	CheckArchiveStorable();
		//	return *this << str;
		//}

		//template < typename STREAM >
		//using Archive = TArchive<false, STREAM>;

	protected:
		constexpr inline void CheckArchiveLoadable() {
			if constexpr (is_storing && is_loading) {
				if (!IsLoading()) {
					throw std::ios_base::failure(BSC__FUNCSIG "NOT an archive for loading!");
				}
			}
		}
		constexpr inline void CheckArchiveStorable() {
			if constexpr (is_storing && is_loading) {
				if (!IsStoring()) {
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

	// better use FileToContainer
	template < typename T = char, concepts::contiguous_type_container<T> TContainer = std::vector<T> >
		requires (std::is_trivial_v<T>)
	[[deprecated("better use FileToContainer()")]] std::optional<TContainer> FileToBuffer(std::filesystem::path const& path) {
		return FileToContainer<TContainer>(path);
	}

	/// @brief 
	/// @tparam TContainer 
	/// @param  
	/// @param path 
	/// @return 
	template < concepts::contiguous_container TContainer >
	bool ContainerToFile(TContainer const& buf, std::filesystem::path const& path) {
		static_assert(std::is_trivial_v<typename std::decay_t<decltype(buf)>::value_type>, "TContainer::value_type must be trivial");
		std::ofstream f(path, std::ios_base::binary);
		f.write((char const*)buf.data(), buf.size() * sizeof(typename TContainer::value_type));
		return (bool)f;
	}

#pragma pack(pop)
}	// namespace biscuit;

