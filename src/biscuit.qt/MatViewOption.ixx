module;

export module biscuit.qt.MatView:Option;
import std;
import biscuit;

using namespace std::literals;

export namespace biscuit::qt::mat_view {

	enum class eZOOM : std::int8_t { none = -1, one2one, fit2window, fit2width, fit2height, mouse_wheel_locked, free };	// lock : 
	enum class eZOOM_IN : std::uint8_t { nearest, linear, bicubic, lanczos4/* EDSR, ESPCN, FSRCNN, LapSRN */};
	enum class eZOOM_OUT : std::uint8_t { nearest, area, };

	struct sOption {
		bool bZoomLock{false};				// if eZoom is NOT free, zoom is locked
		bool bPanningLock{true};			// image can be moved only eZoom == free
		bool bExtendedPanning{true};		// image can move out of screen
		bool bKeyboardNavigation{};			// page up/down, scroll down to next blank of image
		bool bDrawPixelValue{true};			// draw pixel value on image
		bool bPyrImageDown{true};			// build cache image for down sampling. (such as mipmap)
		double dPanningSpeed{2.0};			// Image Panning Speed. 1.0 is same with mouse move.
		int nScrollMargin{5};				// bExtendedPanning, px margin to scroll
		std::chrono::milliseconds tsScroll{250ms};	// Smooth Scroll. duration
		eZOOM_IN eZoomIn{eZOOM_IN::nearest};
		eZOOM_OUT eZoomOut{eZOOM_OUT::area};
		//cv::Vec3b crBackground{0, 0, 0};	// rgb	//{161, 114, 230}
		biscuit::color_bgra_t crBackground{0, 0, 0, 0};	// rgba	//{161, 114, 230}
		
	};

}	// namespace biscuit::qt::mat_view
