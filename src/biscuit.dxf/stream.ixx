module;

export module biscuit.dxf:stream;
import std;
import biscuit;
import :group;

using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::dxf {

	//=============================================================================================================================
	// dxf archive
	enum class eDXF_FILE_TYPE { ascii, binary, binary_preR14 };

	template < eDXF_FILE_TYPE eDXFFileType_ = eDXF_FILE_TYPE::ascii >
	class TDXFGroupIStream {
	public:
		using this_t = TDXFGroupIStream;
		constexpr static eDXF_FILE_TYPE eDXFFileType = eDXFFileType_;

	public:
		static std::vector<int> s_mapGroupCodeToType;
	public:
		std::istream& stream;

		TDXFGroupIStream(std::istream& stream) : stream(stream) {}

		std::expected<std::vector<sGroup>, std::string> ReadGroups() {
			std::vector<sGroup> groups;
			// guess groups count
			auto cur = stream.tellg();
			stream.seekg(0, std::ios::end);
			auto end = stream.tellg();
			stream.seekg(cur);
			constexpr static size_t const approx_size_per_group = eDXFFileType == eDXF_FILE_TYPE::ascii ? 16 : 8;
			auto approxLines = (end - cur) / approx_size_per_group;
			if (approxLines > 64)
				groups.reserve(approxLines);
			// read groups
			for (auto group = ReadGroup(); group and group->value.index() != std::variant_npos; group = ReadGroup()) {
				groups.push_back(std::move(*group));

				// check EOF
				if (auto const& r = groups.back();
					r.iGroupCode == 0
					and r.value.index() == std::to_underlying(eGROUP_VALUE::str)
					and std::get<string_t>(r.value) == "EOF"s) {
					return groups;
				}
			}
			// check
			return std::unexpected("ReadGroups: unexpected group"s);
		}

		std::optional<sGroup> ReadGroup() {
			sGroup group;
			if (auto code = ReadGroupCode())
				group.iGroupCode = *code;
			else
				return std::nullopt;
			switch (group.GetValueTypeFromGroupCode()) {
				using enum eGROUP_VALUE;
			case boolean:	if (auto v = ReadItem<bool>())		group.value = *v; else return std::nullopt; break;
			case i16:		if (auto v= ReadItem<int16>())		group.value = *v; else return std::nullopt; break;
			case i32:		if (auto v= ReadItem<int32>())		group.value = *v; else return std::nullopt; break;
			case i64:		if (auto v= ReadItem<int64>())		group.value = *v; else return std::nullopt; break;
			case dbl:		if (auto v= ReadItem<double>())		group.value = *v; else return std::nullopt; break;
			case str:		if (auto v= ReadItem<string_t>())	group.value = std::move(*v); else return std::nullopt; break;
			case hex_data:	if (auto v= ReadItem<binary_t>())	group.value = std::move(*v); else return std::nullopt; break;
			default: std::println("stream pos: {}", (int64_t)stream.tellg()); return std::nullopt;
			}
			return group;
		}

		std::optional<group_code_t> ReadGroupCode() {
			if constexpr (eDXFFileType == eDXF_FILE_TYPE::binary_preR14) {
				uint8 code8{};
				if (!stream.read(reinterpret_cast<char*>(&code8), sizeof(code8)))
					return std::nullopt;
				if (code8 == 0xffu) {
					group_code_t code{};
					if (!stream.read(reinterpret_cast<char*>(&code), sizeof(code)))
						return std::nullopt;
					return code;
				}
				return (group_code_t)code8;
			}
			else {
				return ReadItem<group_code_t>();
			}
		}

	protected:

		template < typename T >
			requires std::convertible_to<T, group_value_t>
		std::optional<T> ReadItem() {
			T value{};
			if constexpr (eDXFFileType == eDXF_FILE_TYPE::ascii) {
				if constexpr (std::is_arithmetic_v<T>) {
					std::string str;
					if (!std::getline(stream, str))
						return std::nullopt;
					if constexpr (std::is_same_v<T, bool>)
						value = !!biscuit::tszto<int>(str, false);
					else
						value = biscuit::tszto<T>(str, 0);
				}
				else if constexpr (std::is_same_v<T, string_t>) {
					if (!std::getline(stream, value))
						return std::nullopt;
					if (value.ends_with('\r'))
						value.pop_back();
				}
				else if constexpr (std::is_same_v<T, binary_t>) {
					std::string str;
					if (!std::getline(stream, str))
						return std::nullopt;
					value = HexStringToBinary(str);
				}
			}
			else {
				if constexpr (std::is_same_v<T, bool>) {
					uint8 b{};
					if (!stream.read(reinterpret_cast<char*>(&b), sizeof(b)))
						return std::nullopt;
					value = !!b;
				}
				else if constexpr (std::is_arithmetic_v<T>) {
					if (!stream.read(reinterpret_cast<char*>(&value), sizeof(value)))
						return std::nullopt;
					if constexpr (std::is_integral_v<T> and std::endian::native == std::endian::big) {	// for floating points, no need to swap
						value = std::byteswap(value);
					}
				}
				else if constexpr (std::is_same_v<T, string_t>) {
					if (!std::getline(stream, value, '\0'))
						return std::nullopt;
				}
				else if constexpr (std::is_same_v<T, binary_t>) {
					if (auto str = ReadItem<std::string>())
						value = HexStringToBinary(*str);
				}
			}
			return value;
		}

		binary_t HexStringToBinary(string_t const& str) {
			binary_t value;
			value.reserve(str.size()/2);
			// input : hex_data: abcdef123456789
			for (auto sv = biscuit::TrimView(std::string_view(str)); !sv.empty(); ) {
				auto c = sv.front();
				if (!std::isxdigit(c)) {
					sv = sv.substr(1);
					continue;
				}
				if (sv.size() >= 2 and std::isxdigit(sv[1])) {
					value.push_back(tszto<uint8>(sv.substr(0, 2), 16));
					sv = sv.substr(2);
				}
				else {
					value.push_back(tszto<uint8>(sv.substr(0, 1), 16));
					sv = sv.substr(1);
				}
			}
			return value;
		}

	};

	//-----------------------------------------------------------------------------------------------------------------------------
	std::expected<std::vector<sGroup>, std::string> ReadGroups(std::istream& stream) {
		auto pos0 = stream.tellg();
		// file type
		{
			// check binary signature
			constexpr static std::array<char, 22> const sign{ "AutoCAD Binary DXF\r\n\x1a" };
			std::array<char, 22> buf{};
			if (stream.read(buf.data(), buf.size())) {
				if (std::equal(sign.begin(), sign.end(), buf.begin())) {
					group_code_t code{};
					if (!stream.read(reinterpret_cast<char*>(&code), sizeof(code)))
						return std::unexpected("ReadGroups: failed to read first group code"s);
					stream.seekg(-(std::streamsize)sizeof(code), std::ios_base::cur);
					if (code == 0) {
						TDXFGroupIStream<eDXF_FILE_TYPE::binary> reader(stream);
						return reader.ReadGroups();
					}
					else {
						TDXFGroupIStream<eDXF_FILE_TYPE::binary_preR14> reader(stream);
						return reader.ReadGroups();
					}
				}
				else {
					stream.seekg(pos0, std::ios_base::beg);
				}
			}
			else {
				stream.seekg(pos0, std::ios_base::beg);
			}
		}

		// ascii;
		TDXFGroupIStream<eDXF_FILE_TYPE::ascii> reader(stream);
		return reader.ReadGroups();
	}

	std::expected<std::vector<sGroup>, std::string> ReadGroups(std::filesystem::path const& path) {
		std::ifstream stream(path, std::ios_base::binary);
		if (!stream)
			return std::unexpected("ReadGroups: failed to open file"s);
		return ReadGroups(stream);
	}


	//================================================================================================================================
	// dxf archive (out) 
	template < eDXF_FILE_TYPE eDXFFileType_ = eDXF_FILE_TYPE::ascii >
	class TDXFGroupOStream {
	public:
		using this_t = TDXFGroupOStream;
		constexpr static eDXF_FILE_TYPE eDXFFileType = eDXFFileType_;

	public:
		std::ostream& stream;

		TDXFGroupOStream(std::ostream& stream) : stream(stream) {}

		bool WriteGroups(std::vector<sGroup> const& groups) {
			for (auto const& group : groups) {
				if (!WriteGroup(group))
					return false;
			}
			return true;
		}

		bool WriteGroup(sGroup const& group) {
			if (!WriteGroupCode(group.iGroupCode))
				return false;
			bool bResult{};
			std::visit([&](auto const& v) { bResult = WriteItem(v); }, group.value);
			return bResult;
		}

		bool WriteGroupCode(group_code_t code) {
			if constexpr (eDXFFileType == eDXF_FILE_TYPE::ascii) {
				auto str = std::format("{:3d}\r\n", code);
				return stream.write(str.data(), str.size());
			}
			else if constexpr (eDXFFileType == eDXF_FILE_TYPE::binary) {
				return WriteItem<group_code_t>(code);
			}
			else if constexpr (eDXFFileType == eDXF_FILE_TYPE::binary_preR14) {
				if (code < 255) {
					return WriteItem<int8>((int8)code);
				}
				else {
					return WriteItem<int8>(-1)
						and WriteItem<group_code_t>(code);
				}
			}
		}

	protected:
		template < typename T >
			requires std::convertible_to<T, group_value_t>
		bool WriteItem(T const& value) {
			if constexpr (eDXFFileType == eDXF_FILE_TYPE::ascii) {
				std::string str;
				if constexpr (std::is_same_v<T, bool>) { str = std::format("{:9d}\r\n", (int)(value ? 1 : 0)); }
				else if constexpr (std::is_same_v<T, int16>) { str = std::format("{:6d}\r\n", value); }
				else if constexpr (std::is_same_v<T, int32>) { str = std::format("{:9d}\r\n", value); }
				else if constexpr (std::is_same_v<T, int64>) { str = std::format("{}\r\n", value); }
				else if constexpr (std::is_same_v<T, double>) { str = std::format("{:f}\r\n", value); }
				else if constexpr (std::is_same_v<T, string_t>) { str = std::format("{}\r\n", value); }
				else if constexpr (std::is_same_v<T, binary_t>) {
					str.reserve(value.size()*2 + 2);
					for (auto v : value)
						str += std::format("{:02X}", v);
					str += "\r\n";
				}
				else {
					static_assert(false);
				}
				return stream.write(str.data(), str.size());
			}
			else if constexpr (eDXFFileType == eDXF_FILE_TYPE::binary or eDXFFileType == eDXF_FILE_TYPE::binary_preR14) {
				if constexpr (std::is_same_v<T, bool>) {
					uint8 b = value ? 1 : 0;
					return stream.write(reinterpret_cast<char*>(&b), sizeof(b));
				}
				else if constexpr (std::is_arithmetic_v<T>) {
					if constexpr (std::is_integral_v<T> and std::endian::native == std::endian::big) {	// for floating points, no need to swap
						T value2 = std::byteswap(value);
						return stream.write(reinterpret_cast<char*>(&value2), sizeof(value2));
					}
					else {
						return stream.write(reinterpret_cast<char*>(&value), sizeof(value));
					}
				}
				else if constexpr (std::is_same_v<T, string_t>) {
					static constexpr char const cNull = '\0';
					return stream.write(value.data(), value.size())
						&& stream.write(&cNull, sizeof(cNull));	// null-terminated string is not guaranteed
				}
				else if constexpr (std::is_same_v<T, binary_t>) {
					if (value.size() > 127)
						return false;
					string_t str;
					str.reserve(value.size()*2 + 2);
					for (auto v : value)
						str += std::format("{:02X}", v);
					WriteItem(str);
				}
				else {
					static_assert(false);
				}
			}
		}

	};


}	// namespace biscuit::dxf

