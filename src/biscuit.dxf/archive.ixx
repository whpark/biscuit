module;

export module biscuit.dxf:archive;
import std;
import biscuit;

using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::dxf {


	//=============================================================================================================================
	// record
	enum class eDXF_FILE_TYPE { ascii, binary, binary_preR14 };
	using binary_t = std::vector<uint8>;
	using string_t = std::string;
	using group_code_t = int16;
	using record_value_t = std::variant<bool, int16, int32, int64, double, string_t, binary_t>;
	enum class eRECORD_VALUE : int8 { none = -1, boolean = 0, i16, i32, i64, dbl, str, hex_data };

	constexpr group_code_t const g_iMaxGroupCode = 1071;

	struct alignas(32) sRecord {	// alignas(32) - hoping for better cache performance (don't know if it works)
	public:
		group_code_t iGroupCode{};
		record_value_t value;

		eRECORD_VALUE GetRecordTypeFromGroupCode() const { return GetRecordTypeFromGroupCode(iGroupCode); }
		static eRECORD_VALUE GetRecordTypeFromGroupCode(group_code_t iGroupCode);
	};

	//=============================================================================================================================
	// dxf archive
	template < eDXF_FILE_TYPE eDXFFileType_ = eDXF_FILE_TYPE::ascii >
	class TDXFRecordIStream {
	public:
		using this_t = TDXFRecordIStream;
		constexpr static eDXF_FILE_TYPE eDXFFileType = eDXFFileType_;

	public:
		static std::vector<int> s_mapGroupCodeToType;
	public:
		std::istream& stream;

		TDXFRecordIStream(std::istream& stream) : stream(stream) {}

		std::expected<std::vector<sRecord>, std::string> ReadRecords() {
			std::vector<sRecord> records;
			// guess records count
			auto cur = stream.tellg();
			stream.seekg(0, std::ios::end);
			auto end = stream.tellg();
			stream.seekg(cur);
			constexpr static size_t const approx_size_per_record = eDXFFileType == eDXF_FILE_TYPE::ascii ? 16 : 8;
			auto approxLines = (end - cur) / approx_size_per_record;
			if (approxLines > 64)
				records.reserve(approxLines);
			// read records
			for (auto record = ReadRecord(); record and record->value.index() != std::variant_npos; record = ReadRecord()) {
				records.push_back(std::move(*record));

				// check EOF
				if (auto const& r = records.back();
					r.iGroupCode == 0
					and r.value.index() == std::to_underlying(eRECORD_VALUE::str)
					and std::get<string_t>(r.value) == "EOF"s) {
					return records;
				}
			}
			// check
			return std::unexpected("ReadRecords: unexpected record"s);
		}

		std::optional<sRecord> ReadRecord() {
			sRecord record;
			if (auto code = ReadGroupCode())
				record.iGroupCode = *code;
			else
				return std::nullopt;
			switch (record.GetRecordTypeFromGroupCode()) {
				using enum eRECORD_VALUE;
			case boolean:	if (auto v = ReadItem<bool>())		record.value = *v; else return std::nullopt; break;
			case i16:		if (auto v= ReadItem<int16>())		record.value = *v; else return std::nullopt; break;
			case i32:		if (auto v= ReadItem<int32>())		record.value = *v; else return std::nullopt; break;
			case i64:		if (auto v= ReadItem<int64>())		record.value = *v; else return std::nullopt; break;
			case dbl:		if (auto v= ReadItem<double>())		record.value = *v; else return std::nullopt; break;
			case str:		if (auto v= ReadItem<string_t>())	record.value = std::move(*v); else return std::nullopt; break;
			case hex_data:	if (auto v= ReadItem<binary_t>())	record.value = std::move(*v); else return std::nullopt; break;
			default: std::println("stream pos: {}", (int64_t)stream.tellg()); return std::nullopt;
			}
			return record;
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
			requires std::convertible_to<T, record_value_t>
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
	std::expected<std::vector<sRecord>, std::string> ReadRecords(std::istream& stream) {
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
						return std::unexpected("ReadRecords: failed to read first group code"s);
					stream.seekg(-(std::streamsize)sizeof(code), std::ios_base::cur);
					if (code == 0) {
						TDXFRecordIStream<eDXF_FILE_TYPE::binary> reader(stream);
						return reader.ReadRecords();
					}
					else {
						TDXFRecordIStream<eDXF_FILE_TYPE::binary_preR14> reader(stream);
						return reader.ReadRecords();
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
		TDXFRecordIStream<eDXF_FILE_TYPE::ascii> reader(stream);
		return reader.ReadRecords();
	}

	std::expected<std::vector<sRecord>, std::string> ReadRecords(std::filesystem::path const& path) {
		std::ifstream stream(path, std::ios_base::binary);
		if (!stream)
			return std::unexpected("ReadRecords: failed to open file"s);
		return ReadRecords(stream);
	}


	//================================================================================================================================
	// dxf archive (out) 
	template < eDXF_FILE_TYPE eDXFFileType_ = eDXF_FILE_TYPE::ascii >
	class TDXFRecordOStream {
	public:
		using this_t = TDXFRecordOStream;
		constexpr static eDXF_FILE_TYPE eDXFFileType = eDXFFileType_;

	public:
		std::ostream& stream;

		TDXFRecordOStream(std::ostream& stream) : stream(stream) {}

		bool WriteRecords(std::vector<sRecord> const& records) {
			for (auto const& record : records) {
				if (!WriteRecord(record))
					return false;
			}
			return true;
		}

		bool WriteRecord(sRecord const& record) {
			if (!WriteGroupCode(record.iGroupCode))
				return false;
			bool bResult{};
			std::visit([&](auto const& v) { bResult = WriteItem(v); }, record.value);
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
			requires std::convertible_to<T, record_value_t>
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

