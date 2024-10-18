module;

#ifdef _MSC_VER
#	define PUSH_ALIGN_AS(x) __pragma(pack(push, x))
#	define POP_ALIGN_AS() __pragma(pack(pop))
#	define ALIGNAS(x)
#else
#	define PUSH_ALIGN_AS(x)
#	define POP_ALIGN_AS
#	define ALIGNAS(x) alignas(x)
#endif

#include "biscuit/dependencies_opencv.h"

export module biscuit.bmp_file_header;
import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.coord;
import biscuit.color;
import biscuit.misc;

namespace concepts = biscuit::concepts;

using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit {
PUSH_ALIGN_AS(1)

	struct ALIGNAS(1) sBMPFileHeader {
		char sign[2]{ 'B', 'M' };
		uint32_t sizeFile{};
		uint16_t reserved1{};
		uint16_t reserved2{};
		uint32_t offsetData{};
	};
	static_assert(sizeof(sBMPFileHeader) == 14);

	struct ALIGNAS(1) sBMPInfoHeader {
		uint32_t	size;
		int32_t		width;
		int32_t		height;
		uint16_t	planes;
		uint16_t	nBPP;
		uint32_t	compression;
		uint32_t	sizeImage;
		int32_t		XPelsPerMeter;
		int32_t		YPelsPerMeter;
		uint32_t	nColorUsed;
		uint32_t	nColorImportant;

		struct ALIGNAS(1) cie_xyz_rgb {
			struct ALIGNAS(1) cie_xyz_t {
				using float_fix_point_2_30_t = int32_t;
				float_fix_point_2_30_t x, y, z;
			};
			cie_xyz_t r, g, b;
		};
	};
	static_assert(sizeof(sBMPInfoHeader) == 10*sizeof(uint32_t));

	struct ALIGNAS(1) sBMPInfoHeaderV4 : public sBMPInfoHeader {
		uint32_t	maskRed;
		uint32_t	maskGreen;
		uint32_t	maskBlue;
		uint32_t	maskAlpha;
		uint32_t	cstype;
		sBMPInfoHeader::cie_xyz_rgb	endpoints;
		uint32_t	gammaRed;
		uint32_t	gammaGreen;
		uint32_t	gammaBlue;
	};
	static_assert(sizeof(sBMPInfoHeaderV4) == sizeof(sBMPInfoHeader) + (8+9)*sizeof(int32_t));

	struct ALIGNAS(1) sBMPInfoHeaderV5 : public sBMPInfoHeaderV4 {
		uint32_t	intent;
		uint32_t	profileData;
		uint32_t	profileSize;
		uint32_t	reserved;
	};
	static_assert(sizeof(sBMPInfoHeaderV5) == sizeof(sBMPInfoHeaderV4) + (4)*sizeof(int32_t));

POP_ALIGN_AS()

	using vBitmapInfoHeader_t = std::variant<sBMPInfoHeader, sBMPInfoHeaderV4, sBMPInfoHeaderV5>;
	using bitmap_palette_t = std::vector<color_bgra_t>;
	struct sBMPHeader {
	public:
		sBMPFileHeader file{};
		vBitmapInfoHeader_t info{};

		sBMPInfoHeader& GetInfoHeader() {
			sBMPInfoHeader* pos {};
			std::visit([&](auto&& infoV) { pos = &infoV; }, info);
			if (!pos)
				throw std::runtime_error("Invalid BMP header");
			return *pos;
		};
		sBMPInfoHeader const& GetInfoHeader() const {
			sBMPInfoHeader const* pos {};
			std::visit([&](auto&& infoV) { pos = &infoV; }, info);
			if (!pos)
				throw std::runtime_error("Invalid BMP header");
			return *pos;
		};

	};

	std::optional<sBMPHeader> LoadBitmapHeader(std::istream& is) try {
		sBMPHeader header;
		if (!is.read((char*)&header.file, sizeof(header.file)))
			return {};
		uint32_t sizeHeader{};
		if (!is.read((char*)&sizeHeader, sizeof(sizeHeader)))
			return {};
		char* pos {};
		switch (sizeHeader) {
		case sizeof(sBMPInfoHeader)		: header.info.emplace<0, sBMPInfoHeader>({});	pos = (char*)&std::get<sBMPInfoHeader>(header.info); break;
		case sizeof(sBMPInfoHeaderV4)	: header.info.emplace<1, sBMPInfoHeaderV4>({});	pos = (char*)&std::get<sBMPInfoHeaderV4>(header.info); break;
		case sizeof(sBMPInfoHeaderV5)	: header.info.emplace<2, sBMPInfoHeaderV5>({});	pos = (char*)&std::get<sBMPInfoHeaderV5>(header.info); break;
		default:
			return {};
		}
		*(uint32_t*)pos = sizeHeader;
		if (is.read(pos + sizeof(sizeHeader), sizeHeader - sizeof(sizeHeader))) {
			return header;
		}
	}
	catch (...) {
		return {};
	}
	std::optional<sBMPHeader> LoadBitmapHeader(std::filesystem::path const& path) {
		if (std::ifstream f(path, std::ios_base::binary); f)
			return LoadBitmapHeader(f);
		return std::nullopt;
	}

}

