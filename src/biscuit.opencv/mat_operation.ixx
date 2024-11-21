module;

#include "biscuit/config.h"
#include "biscuit/macro.h"
#include "biscuit/dependencies_opencv.h"
#include "biscuit/dependencies_simdutf.h"

export module biscuit.opencv.mat_operation;
import std;
import biscuit;

namespace biscuit {
	constexpr std::array<uint8_t, 4> const s_bomMat = { 3, 'm', 'a', 't' };

	bool s_bGPUChecked{};
	bool s_bUseGPU{};

}

export namespace biscuit {

	//-----------------------------------------------------------------------------------------------------------------------------

	BSC__NODISCARD std::optional<cv::Mat> ReadMat(std::istream& is) {
		cv::Mat mat;
		// read sign
		{
			std::array<uint8_t, 4> buf;
			if (!is.read((char*)buf.data(), buf.size()))
				return {};
			if (!std::ranges::equal(buf, s_bomMat))
				return {};
		}

		auto ReadVar = [&is](auto& var) -> bool { return (bool)is.read((char*)&var, sizeof(var)); };
		int rows{}, cols{}, type{};
		if (!ReadVar(rows)) return {};
		if (!ReadVar(cols)) return {};
		if (!ReadVar(type)) return {};

		if ( (rows < 0) || (cols < 0) )
			return {};
		if ( (rows == 0) || (cols == 0) )
			return mat;

		mat = cv::Mat::zeros(rows, cols, type);
		if (mat.empty())
			return mat;

		auto step = mat.cols * mat.elemSize();
		for (int i = 0; i < rows; i++)
			is.read((char*)mat.ptr(i), step);

		return mat;
	}
	bool StoreMat(std::ostream& os, cv::Mat const& mat) {
		os.write((char const*)s_bomMat.data(), s_bomMat.size());
		os.write((char const*)&mat.rows, sizeof(mat.rows));
		os.write((char const*)&mat.cols, sizeof(mat.cols));
		int type = mat.type();
		os.write((char const*)&type, sizeof(type));
		auto step = mat.cols * mat.elemSize();
		for (int i = 0; i < mat.rows; i++)
			os.write((char*)mat.ptr(i), step);

		return true;
	}

	// cereal
	BSC__NODISCARD void StoreMatBase64(std::ostream& os, cv::Mat& mat) {
		os << mat.cols << mat.rows << mat.type();
		if (mat.cols and mat.rows) {
			std::string str;
			if (mat.isContinuous()) {
				str = ToBase64(std::span(mat.datastart, mat.dataend));
			} else {
				str.reserve((mat.cols * mat.rows * mat.elemSize() +1 ) * 8 / 6);
				size_t step = mat.cols*mat.elemSize();
				for (int y {}; y < mat.rows; y++) {
					auto const* p = mat.ptr(y);
					str += ToBase64(std::span(p, step));
				}
			}
			os << str;
		}
	};
	BSC__NODISCARD std::optional<cv::Mat> ReadMatBase64(std::istream& is) {
		int rows{}, cols{}, type{};
		is >> cols >> rows >> type;
		if (!rows or !cols) {
			return {};
		}
		std::string str;
		is >> str;
		cv::Mat mat = cv::Mat::zeros(rows, cols, type);
		auto [err, len] = simdutf::base64_to_binary(str.data(), str.size(), (char*)mat.data, simdutf::base64_default);
		if (err or len != mat.step*mat.rows) {
			return {};
		}
		return mat;
	}

	//-----------------------------------------------------------------------------------------------------------------------------

	BSC__NODISCARD bool IsMatEqual(cv::Mat const& a, cv::Mat const& b) {
		if (a.cols != b.cols or a.rows != b.rows or a.size() != b.size() or a.type() != b.type())
			return false;
		if (a.empty())
			return true;
		if (a.isContinuous() and b.isContinuous()) {
			return std::equal(a.datastart, a.dataend, b.datastart);
		} else {
			size_t step = a.cols * a.elemSize();
			for (int y{}; y < a.rows; y++) {
				auto const* pA = a.ptr(y);
				auto const* pB = b.ptr(y);
				if (!std::equal(pA, pA+step, pB))
					return false;
			}
		}
		return true;
	}


	//-----------------------------------------------------------------------------
	// Put Outlined Text at Center aligned
	void putTextCenterAligned(cv::InputOutputArray img, std::string const& str, xPoint2i org,
		int fontFace, double fontScale, cv::Scalar color,
		int thickness = 1, int lineType = 8,
		bool bottomLeftOrigin = false, bool bOutline = true)
	{
		xSize2i size{ cv::getTextSize(str, fontFace, fontScale, thickness, nullptr) };

		if (bOutline) {
			cv::Scalar crBkgnd;
			crBkgnd = cv::Scalar(255, 255, 255) - color;
			//if (crBkgnd == Scalar(0, 0, 0))
			//	crBkgnd = Scalar(1, 1, 1);
			int iShift = std::max(1, thickness / 2);
			cv::putText(img, str, org - size / 2 + xPoint2i(0, iShift), fontFace, fontScale, crBkgnd, thickness, lineType, bottomLeftOrigin);
			cv::putText(img, str, org - size / 2 + xPoint2i(iShift, 0), fontFace, fontScale, crBkgnd, thickness, lineType, bottomLeftOrigin);
			cv::putText(img, str, org - size / 2 + xPoint2i(-iShift, 0), fontFace, fontScale, crBkgnd, thickness, lineType, bottomLeftOrigin);
			cv::putText(img, str, org - size / 2 + xPoint2i(0, -iShift), fontFace, fontScale, crBkgnd, thickness, lineType, bottomLeftOrigin);
		}

		cv::putText(img, str, org - size / 2, fontFace, fontScale, color, thickness, lineType, bottomLeftOrigin);

	}

	//-----------------------------------------------------------------------------

	bool CheckGPU([[maybe_unused]] bool bUse) {
	#ifdef HAVE_CUDA
		if (s_bGPUChecked)
			return s_bUseGPU;

		if (bUse) {
			try {
				cv::Mat img(2, 2, CV_64F);
				cv::cuda::GpuMat mat;
				mat.upload(img);
				s_bUseGPU = true;
			}
			catch (cv::Exception&) {
				s_bUseGPU = false;
			}
		}
		else
			s_bUseGPU = false;

		s_bGPUChecked = true;

		return s_bUseGPU;
	#else
		return false;
	#endif
	}

	bool IsGPUEnabled() {
		CheckGPU(true);
		return s_bUseGPU;
	}


	bool ConvertColor(cv::Mat const& imgSrc, cv::Mat& imgDest, int eCode) {
		try {
			#ifdef HAVE_CUDA
			if (s_bUseGPU) {
				try {
					cv::cuda::GpuMat dst;
					cv::cuda::cvtColor(cv::cuda::GpuMat(imgSrc), dst, eCode);
					dst.download(imgDest);
					return true;
				} catch (...) {
					//TRACE((GTL__FUNCSIG " - Error\n").c_str());
				}
			}
			#endif
			cvtColor(imgSrc, imgDest, eCode);
		}
		catch (...) {
			return false;
		}
		return true;
	}

	bool ResizeImage(cv::Mat const& imgSrc, cv::Mat& imgDest, double dScale, int eInterpolation) {
		try {
			#ifdef HAVE_CUDA
			if (s_bUseGPU) {
				try {
					cv::cuda::GpuMat dst;
					cv::cuda::resize(cv::cuda::GpuMat(imgSrc), dst, cv::Size(), dScale, dScale, eInterpolation);
					dst.download(imgDest);
					return true;
				}
				catch (...) {
					//TRACE((GTL__FUNCSIG " - Error\n").c_str());
				}
			}
			#endif
			resize(imgSrc, imgDest, cv::Size(), dScale, dScale, eInterpolation);
		}
		catch (...) {
			return false;
		}
		return true;
	}

#ifdef HAVE_CUDA
	cv::cuda::TemplateMatching& GetCudaTemplateMatching(int mat_type, int method) {
		static std::mutex mtxTemplateMatching;
		static std::map<int, cv::Ptr<cv::cuda::TemplateMatching>> mapTemplateMatching;
		std::lock_guard lock(mtxTemplateMatching);
		auto& ref = mapTemplateMatching[mat_type];
		if (ref.empty())
			ref = cv::cuda::createTemplateMatching(mat_type, method);
		return *ref;
	}
#endif

	bool MatchTemplate(cv::Mat const& img, cv::Mat const& imgTempl, cv::Mat& matResult, int method) {
		try {
			#ifdef HAVE_CUDA
			if (s_bUseGPU) {
				try {
					cv::cuda::GpuMat mat;
					GetCudaTemplateMatching(img.type(), method).match(cv::cuda::GpuMat(img), cv::cuda::GpuMat(imgTempl), mat);
					mat.download(matResult);
					return true;
				}
				catch (...) {
					//TRACE((GTL__FUNCSIG " - Error\n").c_str());
				}
			}
			#endif
			cv::matchTemplate(img, imgTempl, matResult, method);
		}
		catch (...) {
			return false;
		}
		return true;
	}

	//struct sMatchTemplateOption {
	//	int method = cv::TemplateMatchModes::TM_SQDIFF_NORMED;
	//	double dScaleReduce = 0.0;
	//	double dScaleMagnify = 0.0;
	//	int eInterpolationReduce = cv::InterpolationFlags::INTER_LINEAR;
	//	int eInterpolationMagnify = cv::InterpolationFlags::INTER_LINEAR;
	//};
	struct sMatchTemplateResult {
		xPoint2d ptBest;
		double dRate{};
		double dMinMax{};
	};
	std::optional<sMatchTemplateResult> MatchTemplate(cv::Mat const& img, cv::Mat const& imgTempl, double threshold, cv::TemplateMatchModes method = cv::TemplateMatchModes::TM_CCOEFF_NORMED, double dScale = 0.0, int eScaleInterpolation = cv::InterpolationFlags::INTER_LINEAR) try {
		double dMin{}, dMax{};
		cv::Point ptMin, ptMax;
		sMatchTemplateResult result;

	#ifdef HAVE_CUDA
		bool bSuccessGPU{};
		if (s_bUseGPU) {
			try {
				cv::cuda::GpuMat imgG;
				cv::cuda::GpuMat imgGTempl;
				if (dScale == 0.0) {
					imgG.upload(img);
					imgGTempl.upload(imgTempl);
				}
				else {
					cv::cuda::resize(cv::cuda::GpuMat(imgG), imgG, cv::Size(), dScale, dScale, eInterpolation);
					cv::cuda::resize(cv::cuda::GpuMat(imgGTempl), imgGTempl, cv::Size(), dScale, dScale, eInterpolation);
				}
				cv::cuda::GpuMat matResult;
				GetCudaTemplateMatching(imgG.type(), method).match(imgG, imgGTempl, matResult);
				cv::cuda::minMaxLoc(matResult, &dMin, &dMax, &ptMin, &ptMax);
				bSuccessGPU = true;
			}
			catch (...) {
				//TRACE((GTL__FUNCSIG " - Error\n").c_str());
			}
		}
		if (!bSuccessGPU)
	#endif
		{
			cv::Mat matResult;
			cv::Mat imgG(img), imgGTempl(imgTempl);
			if (dScale != 0.0) {
				cv::resize(imgG, imgG, cv::Size(), dScale, dScale, eScaleInterpolation);
				cv::resize(imgGTempl, imgGTempl, cv::Size(), dScale, dScale, eScaleInterpolation);
			}
			cv::matchTemplate(imgG, imgGTempl, matResult, method);
			cv::minMaxLoc(matResult, &dMin, &dMax, &ptMin, &ptMax);
		}

		switch (method) {
		case cv::TM_SQDIFF :
		case cv::TM_SQDIFF_NORMED :
			result.ptBest = ptMin;
			break;
		case cv::TM_CCORR :
		case cv::TM_CCORR_NORMED :
		case cv::TM_CCOEFF :
		case cv::TM_CCOEFF_NORMED :
			result.ptBest = ptMax;
			break;
		}

		switch (method) {
		case cv::TM_SQDIFF :
			result.dMinMax = dMin;
			result.dRate = (dMax-dMin) / dMax;
			break;
		case cv::TM_SQDIFF_NORMED :
			result.dMinMax = dMin;
			result.dRate = (1.-dMin);
			break;
		case cv::TM_CCORR :
		case cv::TM_CCOEFF :
			result.dMinMax = dMax;
			result.dRate = dMax;
			break;
		case cv::TM_CCORR_NORMED :
		case cv::TM_CCOEFF_NORMED :
			result.dMinMax = dMax;
			result.dRate = dMax;
			break;
		}

		if (_isnan(result.dRate))
			result.dRate = 0;

		if (dScale == 0.0) {
			result.ptBest.x += imgTempl.cols/2;
			result.ptBest.y += imgTempl.rows/2;
		} else {
			result.ptBest.x = result.ptBest.x / dScale + imgTempl.cols/2;
			result.ptBest.y = result.ptBest.y / dScale + imgTempl.rows/2;
		}

		if (result.dRate < threshold)
			return {};
		return result;

	} catch (...) {
		return {};
	}

	//-----------------------------------------------------------------------------
	template < typename TRECT >
	bool CalcViewPosition(cv::Size const& sizeView, TRECT const& rectView, TRECT& rectImage/* out */, TRECT& rectDst/* out */)	// image size -> display rect and source rect
	{
		if (rectView.IxRectEmpty()) {
			rectImage.SetRectEmpty();
			rectDst.SetRectEmpty();
			return false;
		}
		auto wDest = rectView.Width();
		auto hDest = rectView.Height();

		if (wDest >= sizeView.width) {
			rectDst.left = rectView.left + (wDest - sizeView.width) / 2;
			rectDst.right = rectDst.left + sizeView.width;
			rectImage.left = 0;
			rectImage.right = sizeView.width;
		}
		else if (wDest < sizeView.width) {
			rectDst.left = rectView.left;
			rectDst.right = rectView.right;
			rectImage.left = (sizeView.width - wDest) / 2;
			rectImage.right = rectImage.left + wDest;
		}

		if (hDest >= sizeView.height) {
			rectDst.top = rectView.top + (hDest - sizeView.height) / 2;
			rectDst.bottom = rectDst.top + sizeView.height;
			rectImage.top = 0;
			rectImage.bottom = sizeView.height;
		}
		else if (hDest < sizeView.height) {
			rectDst.top = rectView.top;
			rectDst.bottom = rectView.bottom;
			rectImage.top = (sizeView.height - hDest) / 2;
			rectImage.bottom = rectImage.top + hDest;
		}

		return true;
	}

	bool MatToMatTransparent(cv::Mat const& imgSource, cv::Mat& imgTarget, cv::Mat const& matMask) try {
		//std::vector<Mat> mats;
		//split(imgSource, mats);
		//merge(mats, imgSource);
		subtract(imgSource, imgSource, imgSource, matMask);
		add(imgSource, imgTarget, imgSource, matMask);
		return true;
	}
	catch (...) {
		return false;
	}

	bool MatToMatTransparent(cv::Mat const& imgSource, cv::Mat& imgTarget, cv::Scalar const& crTransparent) {
		if (imgTarget.empty() || (imgSource.type() != imgTarget.type()) || (imgSource.size() != imgTarget.size()) ) {
			return false;
		}
		if ( (imgSource.channels() != 3) || (imgSource.depth() != CV_8U) )
			return false;

		cv::Vec3b cr;
		cr[0] = (uint8)crTransparent[0];
		cr[1] = (uint8)crTransparent[1];
		cr[2] = (uint8)crTransparent[2];

		for (int y = 0; y < imgSource.rows; y++) {
			const cv::Vec3b* ptr = imgSource.ptr<cv::Vec3b>(y);
			cv::Vec3b* ptr2 = imgTarget.ptr<cv::Vec3b>(y);
			for (int x = 0; x < imgSource.cols; x++) {
				if (ptr[x] != cr)
					ptr2[x] = ptr[x];
			}
		}

		return false;
	}

}

