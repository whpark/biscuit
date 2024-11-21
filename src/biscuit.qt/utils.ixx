module;

#include <QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>

#include "biscuit/dependencies_opencv.h"
#include "biscuit/dependencies_glaze.h"

export module biscuit.qt.utils;
import std;
import biscuit;

using namespace std::literals;
using namespace biscuit::literals;

namespace concepts = biscuit::concepts;

namespace biscuit::qt {

	std::string const s_strKeyWindowPosition { "WindowPositions"s };
	struct sWindowPosOption {
		bool bVisible{true};
		bool bMaximized{};
		bool bMinimized{};

		GLZ_LOCAL_META(sWindowPosOption, bVisible, bMaximized, bMinimized);

		void operator << (QWidget& wnd) {
			bVisible = wnd.isVisible();
			bMaximized = wnd.isMaximized();
			bMinimized = wnd.isMinimized();
		}
		void operator >> (QWidget& wnd) {
			//if (bVisible)
			//	wnd.show();
			//else
			//	wnd.hide();
			if (bMaximized)
				wnd.showMaximized();
			//else if (bMinimized)
			//	wnd.showMinimized();
		}
	};
}

export namespace biscuit::qt {

	using string_view_t = std::u16string_view;

	auto ToCoord(QPoint pt) { return xPoint2i(pt.x(), pt.y()); }
	auto ToCoord(QPointF pt) { return xPoint2d(pt.x(), pt.y()); }
	auto ToCoord(QSize s) { return xSize2i(s.width(), s.height()); }
	auto ToCoord(QSizeF s) { return xSize2d(s.width(), s.height()); }
	auto ToCoordRect(QRect rect) { return xRect2i(rect.left(), rect.top(), rect.right(), rect.bottom()); }
	auto ToCoordRect(QRectF rect) { return xRect2d(rect.left(), rect.top(), rect.right(), rect.bottom()); }
	auto ToCoordBounds(QRect rect) { return xBounds2i(rect.left(), rect.top(), rect.right(), rect.bottom()); }
	auto ToCoordBounds(QRectF rect) { return xBounds2d(rect.left(), rect.top(), rect.right(), rect.bottom()); }

	auto ToQCoord(xPoint2i pt) { return QPoint (pt.x, pt.y); }
	auto ToQCoord(xPoint2d pt) { return QPointF(pt.x, pt.y); }
	auto ToQCoord(xSize2i s) { return QSize (s.width, s.height); }
	auto ToQCoord(xSize2d s) { return QSizeF(s.width, s.height); }
	auto ToQCoord(xRect2i const& rect) { return QRect (rect.x, rect.y, rect.width, rect.height); }
	auto ToQCoord(xRect2d const& rect) { return QRectF(rect.x, rect.y, rect.width, rect.height); }
	auto ToQCoord(xBounds2i const& bounds) { return QRect (bounds.l, bounds.t, bounds.Width(), bounds.Height()); }
	auto ToQCoord(xBounds2d const& bounds) { return QRectF(bounds.l, bounds.t, bounds.Width(), bounds.Height()); }

	QPoint Floor(QPointF pt) {
		return QPoint{ (int)pt.x(), (int)pt.y() };
	}
	QRect Floor(QRectF rect) {
		return QRect{ Floor(rect.topLeft()), Floor(rect.bottomRight()) };
	}

	//=================================================================================================================================
	// helper functions
	QString ToQString(std::string const& str) { return QString::fromStdString(str); }
	QString ToQString(std::wstring const& str) { return QString::fromStdWString(str); }
	QString ToQString(std::u8string const& str) { return QString::fromUtf8(str.c_str(), str.size()); }
	QString ToQString(std::u16string const& str) { return QString::fromStdU16String(str); }
	QString ToQString(std::u32string const& str) { return QString::fromStdU32String(str); }
	QString ToQString(std::filesystem::path const& path) { return ToQString(path.wstring()); }

	template < typename tcontainer, typename tstring = tcontainer::value_type >
	QStringList ToQStrings(tcontainer const& lst) {
		QStringList qlst;
		qlst.reserve(lst.size());
		for (auto const& str : lst)
			qlst.push_back(ToQString(str));
		return qlst;
	}

	template < typename T > requires
		requires (T t) { { ToString(t) } -> std::convertible_to<std::string>; }
	or  requires (T t) { { ToString(t) } -> std::convertible_to<std::wstring>; }
	QString ToQString(T const& t) {
		return ToQString(ToString(t));
	}

	std::u16string ToU16String(QString const& str) { return str.toStdU16String(); }
	std::string ToString(QString const& str) { return str.toStdString(); }
	std::wstring ToWString(QString const& str) { return str.toStdWString(); }
	string_view_t ToView(QString const& str) { return (string_view_t)str; }

	std::u16string ToU16String(QColor cr) { return ToU16String(cr.name()); }
	std::string ToString(QColor cr) { return ToString(cr.name()); }
	std::wstring ToWString(QColor cr) { return ToWString(cr.name()); }

	template < concepts::string_elem tchar_t >
	std::basic_string<tchar_t> ToTString(QString const& str) {
		if constexpr (sizeof(tchar_t) == sizeof(char)) {
			return ToString(str);
		}
		else if constexpr (std::is_same_v<tchar_t, wchar_t>) {
			return ToWString(str);
		}
		else if constexpr (std::is_same_v<tchar_t, char16_t>) {
			return str.toStdU16String();
		}
		else if constexpr (std::is_same_v<tchar_t, char32_t>) {
			return str.toStdU32String();
		}
		else {
			static_assert(false, "Not supported");
		}
	}

	//-------------------------------------------------------------------------
	// to Text, From Text
	template < concepts::coord::generic_coord T_COORD >
	QString ToQString(T_COORD const& coord, std::wstring_view svFMT = L"{}") {
		return ToQString(ToString(coord, svFMT));
	}
	template < concepts::coord::is_coord T_COORD >
	T_COORD FromString(QString const& str) {
		return biscuit::FromString<T_COORD>((string_view_t)str);
	}

	template < concepts::arithmetic value_t >
	value_t ToNumber(QString const& str, int base = 0, std::from_chars_result* result = {}) {
		return biscuit::ToNumber<value_t>((string_view_t)str, base, result);
	}

	//=============================================================================================================================
	/// @brief Wait Cursor
	class xWaitCursor {
	protected:
		//inline thread_local static int m_nCount;
	public:
		xWaitCursor() {
			//m_nCount++;	// 걸 때마다 호출. 처음 걸었어도 다른 곳에서 풀었을 수 있으므로, 무조건 호출 함.
			QApplication::setOverrideCursor(Qt::WaitCursor);
		}
		~xWaitCursor() {
			//if (--m_nCount == 0)	// 끄는거는, 0이 되었을 때만.
			QApplication::restoreOverrideCursor();
		}
	};

	//=============================================================================================================================
	/// @brief QCustomEventFilter
	/// @details filters out events in m_lstEventTypesToFilter
	class QCustomEventFilter  : public QObject {
		//Q_OBJECT
	public:
		using this_t = QCustomEventFilter;
		using base_t = QObject;

	public:
		std::vector<QEvent::Type> m_lstEventTypesToFilter;

		QCustomEventFilter(QObject* parent, std::initializer_list<QEvent::Type> lst) : QObject(parent), m_lstEventTypesToFilter(std::move(lst)) { }
		~QCustomEventFilter() {}

		bool eventFilter(QObject* obj, QEvent* event) override {
			if (auto iter = std::ranges::find(m_lstEventTypesToFilter, event->type());
				iter != m_lstEventTypesToFilter.end())
			{
				return true;
			}
			return QObject::eventFilter(obj, event);
		}

	};


	//=============================================================================================================================
	/// @brief cv::Mat::type() -> QImage::Format
	/// @param cv::Mat::type() (CV8UC1 ...)
	/// @return  QImage::Format
	QImage::Format GetImageFormatType(int cv_type) {
		static std::unordered_map<int, QImage::Format> const m {
			{ CV_8UC1,	QImage::Format_Grayscale8 },
			{ CV_8UC3,	QImage::Format_RGB888 },
			{ CV_8UC4,	QImage::Format_RGBA8888 },
			{ CV_16UC1,	QImage::Format_Grayscale16 },
			{ CV_16UC3,	QImage::Format_RGB16 },
			//{ CV_16UC4,	QImage::Format_RGB },
			//{ CV_16SC1,	QImage:: },
			//{ CV_16SC3,	QImage:: },
			//{ CV_16SC4,	QImage:: },
			//{ CV_32SC1,	QImage::{GL_LUMINANCE,	GL_LUMINANCE,		GL_INT				} },
			{ CV_32SC3,	QImage::Format_RGB32 },
			//{ CV_32SC4,	QImage:: },
			//{ CV_32FC1,	QImage:: },
			//{ CV_32FC3,	QImage:: },
			{ CV_32FC4,	QImage::Format_RGBA32FPx4 },
		};
		if (auto iter = m.find(cv_type); iter != m.end())
			return iter->second;
		return QImage::Format_Invalid;
	}

	//=============================================================================================================================
	// Window Position
	bool SaveWindowPosition(QSettings& reg, std::string const& strWindowName, QWidget* wnd) {
		if (!wnd)
			return false;

		// option (Maximized, Minimized, ...)
		sWindowPosOption option;
		option << *wnd;
	#if 1
		std::string str;
		glz::write_json(option, str);
		reg.setValue(std::format("{}/{}_misc", s_strKeyWindowPosition, strWindowName), ToQString(str));
	#else
		reg.setValue(std::format("{}/{}/Visible", s_strKeyWindowPosition, strWindowName), option.bVisible);
		reg.setValue(std::format("{}/{}/Maximized", s_strKeyWindowPosition, strWindowName), option.bMaximized);
		reg.setValue(std::format("{}/{}/Minimized", s_strKeyWindowPosition, strWindowName), option.bMinimized);
	#endif

		if (!option.bMaximized and !option.bMinimized) {
			// position
			auto rc = ToCoordBounds(wnd->geometry());

			reg.setValue(std::format("{}/{}_rect", s_strKeyWindowPosition, strWindowName), ToQString(rc.ToString<char16_t>()));
		}

		return true;
	}

	bool LoadWindowPosition(QSettings& reg, std::string const& strWindowName, QWidget* wnd) {
		if (!wnd)
			return false;

		// options
		sWindowPosOption option;
	#if 1
		auto misc = reg.value(std::format("{}/{}_misc", s_strKeyWindowPosition, strWindowName), "").toString().toStdString();
		if (auto err = glz::read_json(option, misc); !err) {
			option >> *wnd;
		}
	#else
		option.bVisible = reg.value(std::format("{}/{}/Visible", s_strKeyWindowPosition, strWindowName)).toBool();
		option.bMaximized = reg.value(std::format("{}/{}/Maximized", s_strKeyWindowPosition, strWindowName)).toBool();
		option.bMinimized = reg.value(std::format("{}/{}/Minimized", s_strKeyWindowPosition, strWindowName)).toBool();
		option >> *wnd;
	#endif

		if (!option.bMaximized and !option.bMaximized) {
			auto str = reg.value(std::format("{}/{}_rect", s_strKeyWindowPosition, strWindowName), "0,0,0,0").toString();
			xBounds2i rc;
			rc.FromString((string_view_t)str);

			//// High DPI
			//rc.pt0() = pWindow->FromDIP(wxPoint(rc.pt0()));
			//rc.pt1() = pWindow->FromDIP(wxPoint(rc.pt1()));
			std::vector<xPoint2i> pts{ {rc.l, rc.t}, {rc.l, rc.b}, {rc.r, rc.t}, {rc.r, rc.b} };

			// get display bounds
			std::vector<xRect2i> bounds;
			for (auto* screen: qApp->screens()) {
				bounds.emplace_back(screen->geometry());
			}

			// count vertex in the display
			int nPts{};
			for (auto const& rect : bounds) {
				for (auto const& pt : pts) {
					if (!rect.Contains(pt))
						continue;
					nPts++;
				}
				if (nPts >= 4)
					break;
			}

			xBounds2i rectAdjusted;
			if (nPts >= 3) {
				// if 3 more points are in the display rect, accept it.
				rectAdjusted = rc;
			}
			else {
				// if not, adjust rectangle.
				int maxArea{};
				for (auto const& bound : bounds) {
					auto u = rc;
					u &= bound;
					auto area = u.Width() * u.Height();
					if (maxArea < area) {
						maxArea = area;
						rectAdjusted = u;
					}
				}
			}
			if (!rectAdjusted.IsEmpty()) {
				wnd->setGeometry(ToQCoord(rectAdjusted));
			}
		}

		return true;
	}

}	// namespace biscuit::qt

