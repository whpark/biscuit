module;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// MatHelper.ixx : cv::Mat, CDC,...
//
// PWH.
//
// 2023.07.17 from gtl/win_util/MatHelper.h
// 2024-10-17. biscuit.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

export module biscuit.qt.MatBitmapArchive;
import std;
import "biscuit/dependencies_opencv.hxx";
import biscuit;
import biscuit.opencv;
import biscuit.qt.utils;
import biscuit.qt.ProgressDlg;

using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::qt {

	bool SaveBitmapMatProgress(
		std::filesystem::path const& path,
		cv::Mat const& img,
		int nBPP,
		xSize2i const& pelsPerMeter,
		std::span<color_bgra_t const> palette = {},
		bool bNoPaletteLookup = false,
		bool bBottom2Top = false)
	{
		xProgressDlg dlgProgress(nullptr);
		dlgProgress.m_message = std::format(L"Saving : {}", path.wstring());

		dlgProgress.m_rThreadWorker = std::make_unique<std::jthread>(biscuit::SaveBitmapMat, path, img, nBPP, pelsPerMeter, palette, bNoPaletteLookup, bBottom2Top, dlgProgress.GetCallback());

		auto r = dlgProgress.exec();

		xWaitCursor wc;
		dlgProgress.m_rThreadWorker->join();

		bool bResult = (r == QDialog::Accepted);
		if (!bResult)
			std::filesystem::remove(path);

		return bResult;
	}

	std::optional<xBitmapMat> LoadBitmapMatProgress(std::filesystem::path const& path) {
		xBitmapMat result;
		xProgressDlg dlgProgress(nullptr);
		dlgProgress.m_message = std::format(L"Loading : {}", path.wstring());

		dlgProgress.m_rThreadWorker = std::make_unique<std::jthread>([&result, &path, &dlgProgress]() {
			if (auto r = biscuit::LoadBitmapMat(path, dlgProgress.GetCallback())) result = std::move(r.value());});

		auto ok = dlgProgress.exec();

		xWaitCursor wc;
		dlgProgress.m_rThreadWorker->join();

		if (ok != QDialog::Accepted or result.img.empty()) 
			return {};

		return result;
	}

	std::optional<xBitmapMat> LoadBitmapPixelArrayProgress(std::filesystem::path const& path) {
		xBitmapMat result;

		xProgressDlg dlgProgress(nullptr);
		dlgProgress.m_message = std::format(L"Loading : {}", path.wstring());

		dlgProgress.m_rThreadWorker = std::make_unique<std::jthread>([&result, &path, &dlgProgress]() {
			if (auto r = biscuit::LoadBitmapPixelArray(path, dlgProgress.GetCallback())) result = std::move(*r);
		});

		auto ok = dlgProgress.exec();

		xWaitCursor wc;
		dlgProgress.m_rThreadWorker->join();

		if (ok != QDialog::Accepted or result.img.empty()) 
			return {};

		return result;
	}

}

