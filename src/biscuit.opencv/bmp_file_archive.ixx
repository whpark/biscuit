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

export module biscuit.opencv.bmp_file_archive;
import std;
import biscuit;

namespace concepts = biscuit::concepts;

using namespace std::literals;
using namespace biscuit::literals;

namespace biscuit {
	// for performance reason, MatFromBitmapFile and MatFromBitmapFilePixelArray is duplicated

	constexpr bool bLoopUnrolling = true;
	constexpr bool bMultiThreaded = true;


	template < int nBPP, typename telement = cv::Vec3b, bool bLoopUnrolling = true, bool bMultiThreaded = true >
	bool MatFromBitmapFile(std::istream& f, cv::Mat& img, std::vector<telement> const& palette, callback_progress_t funcCallback) {

		static_assert((nBPP == 1) or (nBPP == 2) or (nBPP == 4) or (nBPP == 8) /*or (nBMP == 16)*/ or (nBPP == 24) or (nBPP == 32));
		if constexpr (nBPP > 8) {
			static_assert(nBPP / 8 == sizeof(telement));
		}
		if (sizeof(telement) != img.elemSize())
			return false;

		int width32 = (img.cols * nBPP + 31) / 32 * 4;
		int pixel_per_byte = (8/nBPP);
		int nColPixel = pixel_per_byte ? img.cols/ pixel_per_byte * pixel_per_byte : img.cols;

		using Func_UnPackSingleRow = std::function<void(int y, std::vector<uint8> const& line, telement* ptr, std::vector<telement> const& palette)>;
		Func_UnPackSingleRow UnPackSingleRow;

		using Func_UnpackLine = std::function<void()>;

		if constexpr (bLoopUnrolling) {
			if constexpr (nBPP == 1) {
				if (palette.size() < 2)
					return false;
				UnPackSingleRow = [img_cols = img.cols, &nColPixel](int y, std::vector<uint8> const& line, telement* ptr, std::vector<telement> const& palette) {
					int x{}, col{};
					for (; x < nColPixel; x += 8, col++) {
						ptr[x + 0] = palette[(line[col] >> 7) & 0b0000'0001];
						ptr[x + 1] = palette[(line[col] >> 6) & 0b0000'0001];
						ptr[x + 2] = palette[(line[col] >> 5) & 0b0000'0001];
						ptr[x + 3] = palette[(line[col] >> 4) & 0b0000'0001];
						ptr[x + 4] = palette[(line[col] >> 3) & 0b0000'0001];
						ptr[x + 5] = palette[(line[col] >> 2) & 0b0000'0001];
						ptr[x + 6] = palette[(line[col] >> 1) & 0b0000'0001];
						ptr[x + 7] = palette[(line[col] >> 0) & 0b0000'0001];
					}
					for (int shift{ 7 }; x < img_cols; x++, shift--) {
						ptr[x] = palette[(line[col] >> shift) & 0b0000'0001];
					}
				};
			}
			else if constexpr (nBPP == 2) {
				if (palette.size() < 4)
					return false;
				UnPackSingleRow = [img_cols = img.cols, &nColPixel](int y, std::vector<uint8> const& line, telement* ptr, std::vector<telement> const& palette) {
					int x{}, col{};
					for (; x < nColPixel; x += 4, col++) {
						ptr[x + 0] = palette[(line[col] >> 6) & 0b0000'0011];
						ptr[x + 1] = palette[(line[col] >> 4) & 0b0000'0011];
						ptr[x + 2] = palette[(line[col] >> 2) & 0b0000'0011];
						ptr[x + 3] = palette[(line[col] >> 0) & 0b0000'0011];
					}
					for (int shift{ 6 }; x < img_cols; x++, shift -= 2) {
						ptr[x] = palette[(line[col] >> shift) & 0b0000'0011];
					}
				};
			}
			else if constexpr (nBPP == 4) {
				if (palette.size() < 16)
					return false;
				UnPackSingleRow = [img_cols = img.cols, &nColPixel](int y, std::vector<uint8> const& line, telement* ptr, std::vector<telement> const& palette) {
					int x{}, col{};
					for (; x < nColPixel; x += 2, col++) {
						ptr[x + 0] = palette[(line[col] >> 4) & 0b0000'1111];
						ptr[x + 1] = palette[(line[col] >> 0) & 0b0000'1111];
					}
					if (x < img_cols)
						ptr[x] = palette[(line[col] >> 4) & 0b0000'1111];
					//x++;
				};
			}
			else if constexpr (nBPP == 8) {
				if (palette.size() < 256)
					return false;
				UnPackSingleRow = [img_cols = img.cols](int y, std::vector<uint8> const& line, telement* ptr, std::vector<telement> const& palette) {
					for (int x{}; x < img_cols; x++)
						ptr[x] = palette[line[x]];
				};
			}
			else if constexpr ((nBPP == 16) or (nBPP == 24) or (nBPP == 32)) {
				// nothing here.
			}
			else
				return false;
		}
		else {	// if constexpr (bLoopUnrolling)
			if ( (nBPP <= 8) and (palette.size() < (0x01 << nBPP)) )
				return false;

			if constexpr ( (nBPP == 1) or (nBPP == 2) or (nBPP == 4) ) {
				uint8 mask = (0x01 << nBPP) - 1;
				UnPackSingleRow = [img_cols = img.cols, &nBPP, &pixel_per_byte, mask](int y, std::vector<uint8> const& line, telement* ptr, std::vector<telement> const& palette) {
					for (int x{}; x < img_cols; x++) {
						int col = x / pixel_per_byte;
						int shift = 8 - (nBPP * ((x%pixel_per_byte)+1));
						ptr[x] = palette[(line[col] >> shift) & mask];
					}
				};
			}
			else if constexpr (nBPP == 8) {
				UnPackSingleRow = [img_cols = img.cols](int y, std::vector<uint8> const& line, telement* ptr, std::vector<telement> const& palette) {
					for (int x{}; x < img_cols; x++) {
						ptr[x] = palette[line[x]];
					}
				};
			}
			else if constexpr ((nBPP == 16) or (nBPP == 24) or (nBPP == 32)) {
				// nothing here.
			}
			else
				return false;
		}

		if constexpr (nBPP == 16) {
			UnPackSingleRow = [img_cols = img.cols](int y, std::vector<uint8> const& line, telement* ptr, std::vector<telement> const& palette) {
				//static_assert(sizeof(telement) == sizeof(uint16_t));
				for (int x{}, xc{}; x < img_cols; x++, xc += sizeof(telement)) {
					// todo: NOT SURE!
					ptr[x] = *(telement*)&line[xc];
				}
			};
		}
		if constexpr ( (nBPP == 24) or (nBPP == 32) ) {
			UnPackSingleRow = [img_cols = img.cols](int y, std::vector<uint8> const& line, telement* ptr, std::vector<telement> const& palette) {
				for (int x{}, xc{}; x < img_cols; x++, xc += sizeof(telement)) {
					if constexpr (sizeof(telement) >= sizeof(cv::Vec3b)) {
						ptr[x][0] = line[xc + 0];
						ptr[x][1] = line[xc + 1];
						ptr[x][2] = line[xc + 2];
					}
					if constexpr (sizeof(telement) >= sizeof(cv::Vec4b)) {
						ptr[x][3] = line[xc + 3];
					}
				}
			};
		}

		if constexpr (bMultiThreaded) {
			auto nCPUDetected = std::thread::hardware_concurrency();
			auto nThread = std::min((uint)img.rows, (nCPUDetected <= 0) ? 2 : nCPUDetected);

			struct BUFFER {
				std::atomic<int> y{-1};
				std::vector<uint8> line;
			};
			std::vector<BUFFER> buffers((size_t)nThread*2);
			for (auto& buf : buffers)
				buf.line.assign((size_t)width32, (uint8)0);

			std::condition_variable cv;
			std::mutex mtxQ;
			std::queue<BUFFER*> q;
			bool bError{};

			auto Reader = [&f, &buffers, img_rows = img.rows, &q, &mtxQ, &cv, &bError, &funcCallback]() {
				int y{};
				int iPercent{};
				for (; y < img_rows; y++) {
					auto index = y % buffers.size();
					auto& buf = buffers[index];
					int yOld = buf.y;
					while (yOld >= 0) {
						buf.y.wait(yOld);
						yOld = buf.y;
					}
					if (!f.read((char*)buf.line.data(), buf.line.size())) {
						bError = true;
						break;
					}
					buf.y = y;
					//buf.y.notify_one();
					{
						std::unique_lock lock(mtxQ);
						q.push(&buf);
					}
					cv.notify_one();

					if (funcCallback) {
						int iPercentNew = y * 100 / img_rows;
						if (iPercent != iPercentNew) {
							iPercent = iPercentNew;
							if (!funcCallback(iPercent, false, false)) {
								bError = true;
								break;
							}
						}
					}
				}
			};

			auto UnpackBuffer = [&q, &mtxQ, &cv, &UnPackSingleRow, &img, &palette](std::stop_token tk) {
				do {
					BUFFER* pBuf{};
					while (!pBuf) {
						std::unique_lock lock(mtxQ);
						if (q.empty()) {
							if (tk.stop_requested())
								return;
							cv.wait(lock);
						}
						else {
							pBuf = q.front();
							q.pop();
						}
					}

					UnPackSingleRow(pBuf->y, pBuf->line, img.ptr<telement>(pBuf->y), palette);
					pBuf->y = -1;
					pBuf->y.notify_one();

				} while (true);
			};

			std::thread reader{Reader};
			std::stop_source ss;
			std::vector<std::jthread> threads;
			threads.reserve(nThread);
			for (uint i{}; i < nThread; i++) {
				threads.emplace_back(UnpackBuffer, ss.get_token());
			}

			reader.join();
			ss.request_stop();
			cv.notify_all();

			return !bError;

		}
		else {
			std::vector<uint8> line((size_t)width32, 0);
			int iPercent{};
			for (int y{}; y < img.rows; y++) {
				if (!f.read((char*)line.data(), line.size()))
					return false;
				auto* ptr = img.ptr<telement>(y);

				UnPackSingleRow(y, line, ptr, palette);

				if (funcCallback) {
					int iPercentNew = y * 100 / img.rows;
					if (iPercent != iPercentNew) {
						iPercent = iPercentNew;
						if (!funcCallback(iPercent, false, false))
							return false;
					}
				}
			}
		}

		return true;
	}

	template < int nBPP, typename telement = cv::Vec3b >
	bool MatFromBitmapFilePixelArray(std::istream& f, cv::Mat& img, callback_progress_t funcCallback) {

		static_assert((nBPP == 1) or (nBPP == 2) or (nBPP == 4) or (nBPP == 8) /*or (nBMP == 16)*/ or (nBPP == 24) or (nBPP == 32));
		if constexpr (nBPP > 8) {
			static_assert(nBPP / 8 == sizeof(telement));
		}
		if (sizeof(telement) != img.elemSize())
			return false;

		int width32 = (img.cols * nBPP + 31) / 32 * 4;
		int pixel_per_byte = (8/nBPP);
		int nColPixel = pixel_per_byte ? img.cols/ pixel_per_byte * pixel_per_byte : img.cols;

		using Func_UnPackSingleRow = std::function<void(int y, std::vector<uint8> const& line, telement* ptr)>;
		Func_UnPackSingleRow UnPackSingleRow;

		using Func_UnpackLine = std::function<void()>;

		if constexpr (nBPP == 1) {
			UnPackSingleRow = [img_cols = img.cols, &nColPixel](int y, std::vector<uint8> const& line, telement* ptr) {
				int x{}, col{};
				for (; x < nColPixel; x += 8, col++) {
					ptr[x + 0] = (line[col] >> 7) & 0b0000'0001;
					ptr[x + 1] = (line[col] >> 6) & 0b0000'0001;
					ptr[x + 2] = (line[col] >> 5) & 0b0000'0001;
					ptr[x + 3] = (line[col] >> 4) & 0b0000'0001;
					ptr[x + 4] = (line[col] >> 3) & 0b0000'0001;
					ptr[x + 5] = (line[col] >> 2) & 0b0000'0001;
					ptr[x + 6] = (line[col] >> 1) & 0b0000'0001;
					ptr[x + 7] = (line[col] >> 0) & 0b0000'0001;
				}
				for (int shift{7}; x < img_cols; x++, shift--) {
					ptr[x] = (line[col] >> shift) & 0b0000'0001;
				}
				//x++;
			};
		}
		else if constexpr (nBPP == 2) {
			UnPackSingleRow = [img_cols = img.cols, &nColPixel](int y, std::vector<uint8> const& line, telement* ptr) {
				int x{}, col{};
				for (; x < nColPixel; x += 4, col++) {
					ptr[x + 0] = (line[col] >> 6) & 0b0000'0011;
					ptr[x + 1] = (line[col] >> 4) & 0b0000'0011;
					ptr[x + 2] = (line[col] >> 2) & 0b0000'0011;
					ptr[x + 3] = (line[col] >> 0) & 0b0000'0011;
				}
				for (int shift{ 6 }; x < img_cols; x++, shift -= 2) {
					ptr[x] = (line[col] >> shift) & 0b0000'0011;
				}
				//x++;
			};
		}
		else if constexpr (nBPP == 4) {
			UnPackSingleRow = [img_cols = img.cols, &nColPixel](int y, std::vector<uint8> const& line, telement* ptr) {
				int x{}, col{};
				for (; x < nColPixel; x += 2, col++) {
					ptr[x + 0] = (line[col] >> 4) & 0b0000'1111;
					ptr[x + 1] = (line[col] >> 0) & 0b0000'1111;
				}
				if (x < img_cols) {
					ptr[x] = (line[col] >> 4) & 0b0000'1111;
				}
				//x++;
			};
		}
		else if constexpr (nBPP == 8) {
			UnPackSingleRow = [img_cols = img.cols](int y, std::vector<uint8> const& line, telement* ptr) {
				for (int x{}; x < img_cols; x++) {
					ptr[x] = line[x];
				}
			};
		}
		else if constexpr ((nBPP == 16) or (nBPP == 24) or (nBPP == 32)) {
			// nothing here.
		}
		else {
			return false;
		}

		if constexpr (nBPP == 16) {
			UnPackSingleRow = [img_cols = img.cols](int y, std::vector<uint8> const& line, telement* ptr) {
				//static_assert(sizeof(telement) == sizeof(uint16_t));
				for (int x{}, xc{}; x < img_cols; x++, xc += sizeof(telement)) {
					// todo: NOT SURE!
					ptr[x] = *(telement*)&line[xc];
				}
			};
		}
		if constexpr ( (nBPP == 24) or (nBPP == 32) ) {
			UnPackSingleRow = [img_cols = img.cols](int y, std::vector<uint8> const& line, telement* ptr) {
				for (int x{}; x < img_cols; x++) {
					int xc = x * sizeof(telement);
					if constexpr (sizeof(telement) >= sizeof(cv::Vec3b)) {
						ptr[x][0] = line[xc + 0];
						ptr[x][1] = line[xc + 1];
						ptr[x][2] = line[xc + 2];
					}
					if constexpr (sizeof(telement) >= sizeof(cv::Vec4b)) {
						ptr[x][3] = line[xc + 3];
					}
				}
			};
		}

		auto nCPUDetected = std::thread::hardware_concurrency();
		auto nThread = std::min((uint)img.rows, (nCPUDetected <= 0) ? 2 : nCPUDetected);

		struct BUFFER {
			std::atomic<int> y{-1};
			std::vector<uint8> line;
		};
		std::vector<BUFFER> buffers((size_t)nThread*2);
		for (auto& buf : buffers)
			buf.line.assign((size_t)width32, (uint8)0);

		std::condition_variable cv;
		std::mutex mtxQ;
		std::queue<BUFFER*> q;
		bool bError{};

		auto Reader = [&f, &buffers, img_rows = img.rows, &q, &mtxQ, &cv, &bError, &funcCallback]() {
			int y{};
			int iPercent{};
			for (; y < img_rows; y++) {
				auto index = y % buffers.size();
				auto& buf = buffers[index];
				int yOld = buf.y;
				while (yOld >= 0) {
					buf.y.wait(yOld);
					yOld = buf.y;
				}
				if (!f.read((char*)buf.line.data(), buf.line.size())) {
					bError = true;
					break;
				}
				buf.y = y;
				//buf.y.notify_one();
				{
					std::unique_lock lock(mtxQ);
					q.push(&buf);
				}
				cv.notify_one();

				if (funcCallback) {
					int iPercentNew = y * 100 / img_rows;
					if (iPercent != iPercentNew) {
						iPercent = iPercentNew;
						if (!funcCallback(iPercent, false, false)) {
							bError = true;
							break;
						}
					}
				}
			}
		};

		auto UnpackBuffer = [&q, &mtxQ, &cv, &UnPackSingleRow, &img](std::stop_token tk) {
			do {
				BUFFER* pBuf{};
				while (!pBuf) {
					std::unique_lock lock(mtxQ);
					if (q.empty()) {
						if (tk.stop_requested())
							return;
						cv.wait(lock);
					}
					else {
						pBuf = q.front();
						q.pop();
					}
				}

				UnPackSingleRow(pBuf->y, pBuf->line, img.ptr<telement>(pBuf->y));
				pBuf->y = -1;
				pBuf->y.notify_one();

			} while (true);
		};

		std::thread reader{Reader};
		std::stop_source ss;
		std::vector<std::jthread> threads;
		threads.reserve(nThread);
		for (uint i{}; i < nThread; i++) {
			threads.emplace_back(UnpackBuffer, ss.get_token());
		}

		reader.join();
		ss.request_stop();
		cv.notify_all();

		return !bError;

	}

}	// namespace internal

export namespace biscuit {

	class xBitmapMat {
	public:
		cv::Mat img;
		sSize2i pelsPerMeter;
		bitmap_palette_t palette;
	};

	std::optional<xBitmapMat> LoadBitmapMat(std::istream& is, callback_progress_t funcCallback = {}) {
		bool bOK{};

		// Trigger notifying it's over.
		xFinalAction fa([&funcCallback, &bOK] {if (funcCallback) funcCallback(-1, true, !bOK); });

		if (!is)
			return {};

		try {
			auto header = LoadBitmapHeader(is);
			if (!header)
				return {};

			sBMPFileHeader const& fh = header->file;
			sBMPInfoHeader const& info = header->GetInfoHeader();

			if (info.compression or (info.planes != 1))
				return {};

			xBitmapMat result;
			auto& img = result.img;
			result.pelsPerMeter.width = info.XPelsPerMeter;
			result.pelsPerMeter.height = info.YPelsPerMeter;

			int cx = info.width;
			int cy = info.height;
			bool bFlipY{};
			if (cy < 0) {
				cy = -cy;
			}
			else {
				bFlipY = true;
			}
			if ((cx <= 0) or (cy <= 0) /*or ((uint64_t)cx * (uint64_t)cy >= 0xffff'ff00ull)*/)
				return {};

			// Load Palette
			auto& palette = result.palette;
			if (size_t nPalette = (fh.offsetData - sizeof(fh) - info.size) / sizeof(color_bgra_t); nPalette > 0) {
				switch (nPalette) {
				case 2: case 4: case 16: case 256:
					break;
				default:
					return {};
				}
				palette.assign(nPalette, {});
				is.read((char*)palette.data(), sizeof(color_bgra_t) * nPalette);
			}

			// pixel array
			if (!is.seekg(fh.offsetData))
				return {};

			// Check Palette - to reduce memory size
			bool bColor { info.nBPP > 8 };				// first assumption
			bool bAlphaChannel { info.nBPP >= 32 };		// first assumption
			if (info.nBPP <= 8) {
				if (palette.size()) {
					if (auto alpha0 = palette.front().a; alpha0 == 0 or alpha0 == 0xffu) {
						for (auto const& v : palette) {
							if (v.a != alpha0) {
								bAlphaChannel = true;
								break;
							}
						}
					}
					if (!bAlphaChannel) {
						for (auto const& v : palette) {
							if ((v.r != v.g) and (v.g != v.b)) {
								bColor = true;
								break;
							}
						}
					}
				}
				else {
					bColor = bAlphaChannel = false;
				}
			}

			if (bAlphaChannel) {
				img = cv::Mat::zeros(info.height, info.width, CV_8UC4);
				std::vector<cv::Vec4b> palette4(palette.size(), 0);
				for (size_t i{}; i < palette.size(); i++) {
					auto const& cr = palette[i];
					palette4[i] = cv::Vec4b(cr.b, cr.g, cr.r, cr.a);
				}
				bOK = MatFromBitmapFile<32, cv::Vec4b, bLoopUnrolling, bMultiThreaded>(is, img, palette4, funcCallback);
			}
			else if (bColor) {
				img = cv::Mat::zeros(cv::Size(cx, cy), CV_8UC3);
				std::vector<cv::Vec3b> palette3(palette.size(), 0);
				for (size_t i{}; i < palette.size(); i++) {
					auto const& cr = palette[i];
					palette3[i] = cv::Vec3b(cr.b, cr.g, cr.r);
				}
				bOK = MatFromBitmapFile<24, cv::Vec3b, bLoopUnrolling, bMultiThreaded>(is, img, palette3, funcCallback);
			}
			else {
				img = cv::Mat::zeros(cv::Size(cx, cy), CV_8UC1);
				std::vector<uint8> palette1(palette.size(), 0);
				for (size_t i{}; i < palette.size(); i++)
					palette1[i] = palette[i].r;
				switch (info.nBPP) {
				case 1: bOK = MatFromBitmapFile<1, uint8, bLoopUnrolling, bMultiThreaded>(is, img, palette1, funcCallback); break;
				case 2: bOK = MatFromBitmapFile<2, uint8, bLoopUnrolling, bMultiThreaded>(is, img, palette1, funcCallback); break;
				case 4: bOK = MatFromBitmapFile<4, uint8, bLoopUnrolling, bMultiThreaded>(is, img, palette1, funcCallback); break;
				case 8: bOK = MatFromBitmapFile<8, uint8, bLoopUnrolling, bMultiThreaded>(is, img, palette1, funcCallback); break;
				}
			}
			if (!bOK)
				img.release();

			if (!img.empty() and bFlipY) {
				cv::flip(img, img, 0);
			}
			return result;

		} catch (...) {
		}
		return {};
	}
	std::optional<xBitmapMat> LoadBitmapMat(const std::filesystem::path& path, callback_progress_t funcCallback = {}) {
		if (std::ifstream is{ path, std::ios::binary }; is)
			return LoadBitmapMat(is, funcCallback);
		return {};
	}

	std::optional<xBitmapMat> LoadBitmapPixelArray(std::istream& is, callback_progress_t funcCallback = {}) {
		bool bOK{};
		// Trigger notifying it's over.
		xFinalAction fa([&funcCallback, &bOK] {if (funcCallback) funcCallback(-1, true, !bOK); });

		if (!is)
			return {};

		try {
			auto header = LoadBitmapHeader(is);
			if (!header)
				return {};

			sBMPFileHeader const& fh = header->file;
			sBMPInfoHeader const& info = header->GetInfoHeader();

			if (info.compression or (info.planes != 1))
				return {};

			xBitmapMat result;
			auto& img = result.img;
			result.pelsPerMeter.width = info.XPelsPerMeter;
			result.pelsPerMeter.height = info.YPelsPerMeter;

			int cx = info.width;
			int cy = info.height;
			bool bFlipY{};
			if (cy < 0) {
				cy = -cy;
			}
			else {
				bFlipY = true;
			}
			if ((cx <= 0) or (cy <= 0) /*or ((uint64_t)cx * (uint64_t)cy >= 0xffff'ff00ull)*/)
				return {};

			// Load Palette
			auto& palette = result.palette;
			if (size_t nPalette = (fh.offsetData - sizeof(fh) - info.size) / sizeof(color_bgra_t); nPalette > 0) {
				palette.assign(nPalette, {});
				is.read((char*)palette.data(), sizeof(color_bgra_t) * nPalette);
			}

			// pixel array
			if (!is.seekg(fh.offsetData))
				return {};

			if (info.nBPP <= 8) {
				img = cv::Mat::zeros(cv::Size(cx, cy), CV_8UC1);
				switch (info.nBPP) {
				case 1: bOK = MatFromBitmapFilePixelArray<1, uint8>(is, img, funcCallback); break;
				case 2: bOK = MatFromBitmapFilePixelArray<2, uint8>(is, img, funcCallback); break;
				case 4: bOK = MatFromBitmapFilePixelArray<4, uint8>(is, img, funcCallback); break;
				case 8: bOK = MatFromBitmapFilePixelArray<8, uint8>(is, img, funcCallback); break;
				}
			//} else if (info.nBPP <= 16) {
			//	img = cv::Mat::zeros(cv::Size(cx, cy), CV_16UC1);
			//	bOK = MatFromBitmapFilePixelArray<16, uint16>(is, img, funcCallback);
			} else if (info.nBPP <= 24) {
				img = cv::Mat::zeros(cv::Size(cx, cy), CV_8UC3);
				bOK = MatFromBitmapFilePixelArray<24, cv::Vec3b>(is, img, funcCallback);
			} else if (info.nBPP <= 32) {
				img = cv::Mat::zeros(cv::Size(cx, cy), CV_8UC4);
				bOK = MatFromBitmapFilePixelArray<32, cv::Vec4b>(is, img, funcCallback);
			}

			if (!bOK)
				img.release();

			if (!img.empty() and bFlipY) {
				cv::flip(img, img, 0);
			}
			return result;
		}
		catch (...) {
		}
		return {};
	}
	std::optional<xBitmapMat> LoadBitmapPixelArray(const std::filesystem::path& path, callback_progress_t funcCallback = {}) {
		if (std::ifstream is{ path, std::ios::binary }; is)
			return LoadBitmapPixelArray(is, funcCallback);
		return {};
	}


}

