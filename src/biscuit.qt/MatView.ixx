﻿module;

#include <GL/glew.h>

#include <QOpenGLExtraFunctions>
#include <QWidget>
#include <QTimer>

#include <QEvent>
#include <QKeyEvent>
#include <QScrollBar>
#include <QMessageBox>
#include <QOpenGLExtraFunctions>
#include <QTimer>

#include "verdigris/wobjectcpp.h"
#include "verdigris/wobjectimpl.h"

#include "ui_MatView.h"

export module biscuit.qt.MatView;

import std;
import "biscuit/dependencies/fmt.hxx";
import "biscuit/dependencies/eigen.hxx";
import "biscuit/dependencies/opencv.hxx";
import "biscuit/dependencies/spdlog.hxx";
import biscuit;
import biscuit.opencv;
import biscuit.qt.utils;
import biscuit.qt.ui_data_exchange;
export import :Canvas;
export import :Option;
export import :SettingsDlg;

W_REGISTER_ARGTYPE(biscuit::qt::xMatViewCanvas*);
W_REGISTER_ARGTYPE(QMouseEvent*);
W_REGISTER_ARGTYPE(QWheelEvent*);

export namespace biscuit::qt {
	using namespace std::literals;

	class xMatView : public QWidget {
		W_OBJECT(xMatView);

	public:
		using this_t = xMatView;
		using base_t = QWidget;

		using string_t = std::wstring;
		using ct_t = biscuit::xCoordTrans2d;

	public:
		// ZOOM
		//using zoom_t = mat_view::zoom_t;
		//using zoom_in_t = mat_view::zoom_in_t;
		//using zoom_out_t = mat_view::zoom_out_t;

		using zoom_t = mat_view::eZOOM;
		using zoom_in_t = mat_view::eZOOM_IN;
		using zoom_out_t = mat_view::eZOOM_OUT;

		using option_t = mat_view::sOption;

		std::string m_strCookie;
		std::function<bool(bool bStore, std::string_view cookie, option_t&)> m_fnSyncSetting;

		struct S_SCROLL_GEOMETRY {
			xBounds2i rectClient, rectImageScreen, rectScrollRange;
		};

	protected:
		//// gl
		//std::unique_ptr<wxGLContext> m_context;
		////GLuint m_textureID{};

		struct {
			std::unique_ptr<QOpenGLExtraFunctions> gl{};
			GLuint shaderProgram{};
			GLuint VBO{}, VAO{};
			QOpenGLExtraFunctions& operator () () { return *gl; }
			QOpenGLExtraFunctions const& operator () () const { return *gl; }

			void Clear() {
				if (gl) {
					if (auto r = std::exchange(shaderProgram, 0)) {
						gl->glDeleteProgram(r);
					}
					if (auto r = std::exchange(VBO, 0)) {
						gl->glDeleteBuffers(1, &r);
					}
					if (auto r = std::exchange(VAO, 0)) {
						gl->glDeleteVertexArrays(1, &r);
					}
				}
				gl.reset();
			}
		} m_gl;

		cv::Mat m_img;	// original image
		cv::Mat m_palette;	// palette. CV_8UC1 or CV_8UC3, 1 row 256 cols
		mutable struct {
			std::mutex mtx;
			std::deque<cv::Mat> imgs;
			std::jthread threadPyramidMaker;
		} m_pyramid;

		// Mouse Action
	public:
		enum class eMOUSE_ACTION : uint8_t { none, pan, select, };
		enum class eWHEEL_ACTION : uint8_t { none, zoom, scroll, };
		auto& MouseAction() { return m_mapMouseAction; };
		auto& WheelAction() { return m_mapWheelAction; };
	protected:
		std::map<std::pair<Qt::MouseButton, Qt::KeyboardModifiers>, eMOUSE_ACTION> m_mapMouseAction{
			{ {Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier},		eMOUSE_ACTION::pan },
			{ {Qt::MouseButton::LeftButton, Qt::KeyboardModifier::ControlModifier},	eMOUSE_ACTION::pan },
			{ {Qt::MouseButton::LeftButton, Qt::KeyboardModifier::ShiftModifier},	eMOUSE_ACTION::select },
			{ {Qt::MouseButton::RightButton, Qt::KeyboardModifier::NoModifier},		eMOUSE_ACTION::select },
		};
		std::map<Qt::KeyboardModifiers, eWHEEL_ACTION> m_mapWheelAction{
			{ Qt::KeyboardModifier::NoModifier,			eWHEEL_ACTION::zoom },
			{ Qt::KeyboardModifier::ControlModifier,	eWHEEL_ACTION::scroll },
		};
	public:
		std::map<Qt::MouseButton, eMOUSE_ACTION> m_current_mouse_action;

		mutable struct {
			bool bInSelectionMode{};
			bool bRectSelected{};
			std::optional<xPoint2i> ptAnchor;			// Anchor Point in Screen Coordinate
			xPoint2i ptOffset0;			// offset backup
			xPoint2d ptSel0, ptSel1;	// Selection Point in Image Coordinate
			void Clear() {
				bInSelectionMode = {};
				bRectSelected = {};
				ptAnchor.reset();
				ptOffset0 = {};
				ptSel0 = ptSel1 = {};
			}
		} m_mouse;
		mutable struct {
			xPoint2d pt0, pt1;
			std::chrono::steady_clock::time_point t0, t1;
			QTimer timer;
			void Clear() {
				pt0 = pt1 = {};
				t0 = t1 = {};
				timer.stop();
			}
		} m_smooth_scroll;

		option_t m_option;
		zoom_t m_eZoom{zoom_t::fit2window};
		ct_t m_ctScreenFromImage;
		mutable bool m_bSkipSpinZoomEvent{};

		xMatViewCanvas* m_canvas{};

	public:
		xMatView(QWidget* parent = nullptr);
		~xMatView();

	public:
		cv::Mat GetImage() { return m_img; }
		cv::Mat const& GetImage() const { return m_img; }
		bool SetImage(cv::Mat const& img, bool bCenter = true, zoom_t eZoomMode = zoom_t::none, bool bCopy = false);
		void Reset();
		cv::Mat GetPalette() { return m_palette; }
		bool SetPalette(cv::Mat const& palette, bool bUpdateView);	// palette will be copied into m_palette
	protected:
		void ApplyPalette(cv::Mat const& imgSrc, cv::Mat& imgDst) {
			if (m_img.type() == CV_8UC1 and !m_palette.empty())
				cv::applyColorMap(imgSrc, imgDst, m_palette);
			else
				imgDst = imgSrc;
		}
	public:
		bool SetZoomMode(zoom_t eZoomMode, bool bCenter = true);
		std::optional<xRect2i> GetSelectionRect() const {
			if (!m_mouse.bRectSelected)
				return {};
			xRect2i rect(xPoint2i(biscuit::Floor(m_mouse.ptSel0)), xPoint2i(biscuit::Floor(m_mouse.ptSel1)));
			rect.Normalize();
			return rect;
		}
		std::optional<std::pair<xPoint2d, xPoint2d>> GetSelectionPoints() const {
			if (!m_mouse.bRectSelected)
				return {};
			return std::pair<xPoint2d, xPoint2d>{m_mouse.ptSel0, m_mouse.ptSel1};
		}
		void SetSelectionRect(xRect2i const& rect);
		void ClearSelectionRect();

		bool LoadOption() { return m_fnSyncSetting and m_fnSyncSetting(false, m_strCookie, m_option) and SetOption(m_option, false); }
		bool SaveOption() { return m_fnSyncSetting and m_fnSyncSetting(true, m_strCookie, m_option); }
		option_t const& GetOption() const { return m_option; }
		bool SetOption(option_t const& option, bool bStore = true);

		bool ShowToolBar(bool bShow);
		bool IsToolBarShown() const;

		//virtual void OnClose(wxCloseEvent& event) override;

		// ClientRect, ImageRect, ScrollRange
		S_SCROLL_GEOMETRY GetScrollGeometry();
		bool UpdateCT(bool bCenter = false, zoom_t eZoom = zoom_t::none);
		bool UpdateScrollBars();
		bool ZoomInOut(double step, xPoint2i ptAnchor, bool bCenter);
		bool SetZoom(double scale, xPoint2i ptAnchor, bool bCenter);
		bool ScrollTo(xPoint2d pt, std::chrono::milliseconds tsScroll = -1ms);
		bool Scroll(xPoint2d delta, std::chrono::milliseconds tsScroll = -1ms);
		void PurgeScroll(bool bUpdate = true);
		bool KeyboardNavigate(int key, bool ctrl = false, bool alt = false, bool shift = false);

		bool SigMousePressed(xMatViewCanvas* canvas, QMouseEvent* event) W_SIGNAL(SigMousePressed, canvas, event);
		bool SigMouseReleased(xMatViewCanvas* canvas, QMouseEvent* event) W_SIGNAL(SigMouseReleased, canvas, event);
		bool SigMouseDoubleClicked(xMatViewCanvas* canvas, QMouseEvent* event) W_SIGNAL(SigMouseDoubleClicked, canvas, event);
		bool SigMouseMoved(xMatViewCanvas* canvas, QMouseEvent* event) W_SIGNAL(SigMouseMoved, canvas, event);
		bool SigMouseWheelMoved(xMatViewCanvas* canvas, QWheelEvent* event) W_SIGNAL(SigMouseWheelMoved, canvas, event);

	protected:
		void BuildPyramid();
		void StopPyramidMaker();
		xRect2i GetViewRect();
		void InitializeGL(xMatViewCanvas* canvas);
		void PaintGL(xMatViewCanvas* canvas);
		bool PutMatAsTexture(GLuint textureID, cv::Mat const& img, int width, xBounds2i const& rect, xRect2i const& rectClient);

	protected:
		virtual void keyPressEvent(QKeyEvent *event) override;
		void OnView_mousePressEvent(xMatViewCanvas* canvas, QMouseEvent *event);
		void OnView_mouseReleaseEvent(xMatViewCanvas* canvas, QMouseEvent *event);
		void OnView_mouseDoubleClickEvent(xMatViewCanvas* canvas, QMouseEvent *event);
		void OnView_mouseMoveEvent(xMatViewCanvas* canvas, QMouseEvent *event);
		void OnView_wheelEvent(xMatViewCanvas* canvas, QWheelEvent* event);

	protected:
		// slots
		void OnCmbZoomMode_currentIndexChanged(int index);
		void OnSpinZoom_valueChanged(double value);
		void OnBtnZoomIn_clicked();
		void OnBtnZoomOut_clicked();
		void OnBtnZoomFit_clicked();
		void OnBtnCountColor_clicked();	// NOT Connected with button
		void OnBtnSettings_clicked();
		void OnSmoothScroll_timeout();
		void OnView_resized();
		void OnSbHorz_valueChanged(int value);
		//void OnSbHorz_sliderMoved(int value);
		void OnSbVert_valueChanged(int value);
		//void OnSbVert_sliderMoved(int value);

	private:
		std::unique_ptr<Ui::MatViewClass> ui;
	};

	W_OBJECT_IMPL(xMatView);

	constexpr double const dZoomLevels[] = {
		1./8192, 1./4096, 1./2048, 1./1024,
		1./512, 1./256, 1./128, 1./64, 1./32, 1./16, 1./8, 1./4., 1./2.,
		3./4, 1, 1.25, 1.5, 1.75, 2, 2.5, 3, 4, 5, 6, 7, 8, 9, 10,
		12.5, 15, 17.5, 20, 25, 30, 35, 40, 45, 50, 60, 70, 80, 90, 100,
		125, 150, 175, 200, 250, 300, 350, 400, 450, 500,
		600, 700, 800, 900, 1'000,
		//1250, 1500, 1750, 2000, 2500, 3000, 3500, 4000, 4500, 5000,
		//6000, 7000, 8000, 9000, 10000,
		//12500, 15000, 17500, 20000, 25000, 30000, 35000, 40000, 45000, 50000,
		//60000, 70000, 80000, 90000, 100000,
		//125000, 150000, 175000, 200000, 250000, 300000, 350000, 400000, 450000, 500000,
		//600000, 700000, 800000, 900000, 1000000,
		//1250000, 1500000, 1750000, 2000000, 2500000, 3000000, 3500000, 4000000, 4500000, 5000000,
		//6000000, 7000000, 8000000, 9000000, 10000000,
		//12500000, 15000000, 17500000, 20000000, 25000000, 30000000, 35000000, 40000000, 45000000, 50000000,
		//60000000, 70000000, 80000000, 90000000
	};

	xMatView::xMatView(QWidget* parent) : QWidget(parent), ui(std::make_unique<Ui::MatViewClass>()) {
		ui->setupUi(this);

		m_canvas = new biscuit::qt::xMatViewCanvas(ui->frame);
		m_canvas->setObjectName("canvas");
		m_canvas->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
		ui->gridLayout->addWidget(m_canvas, 0, 0, 1, 1);

		ui->cmbZoomMode->setCurrentIndex(std::to_underlying(m_eZoom));

		// openGL object
		//ui->canvas = std::make_unique<xMatViewCanvas>(this);
		if (auto* canvas = m_canvas) {
			canvas->m_fnInitializeGL		= [this](auto* p) { this->InitializeGL(p); };
			canvas->m_fnPaintGL				= [this](auto* p) { this->PaintGL(p); };
			canvas->m_fnMousePress			= [this](auto* p, auto* e) { this->OnView_mousePressEvent(p, e); };
			canvas->m_fnMouseRelease		= [this](auto* p, auto* e) { this->OnView_mouseReleaseEvent(p, e); };
			canvas->m_fnMouseDoubleClick	= [this](auto* p, auto* e) { this->OnView_mouseDoubleClickEvent(p, e); };
			canvas->m_fnMouseMove			= [this](auto* p, auto* e) { this->OnView_mouseMoveEvent(p, e); };
			canvas->m_fnWheel				= [this](auto* p, auto* e) { this->OnView_wheelEvent(p, e); };
			canvas->setMouseTracking(true);
		}
		//ui->canvas->setObjectName("canvas");
		//QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		//sizePolicy.setHorizontalStretch(0);
		//sizePolicy.setVerticalStretch(0);
		//sizePolicy.setHeightForWidth(false);
		//ui->canvas->setSizePolicy(sizePolicy);
		//ui->gridLayout->addWidget(ui->canvas.get(), 0, 0, 1, 1);

		if (m_fnSyncSetting) {
			m_fnSyncSetting(false, m_strCookie, m_option);
		}

		connect(ui->cmbZoomMode, &QComboBox::currentIndexChanged, this, &this_t::OnCmbZoomMode_currentIndexChanged);
		connect(ui->spinZoom, &QDoubleSpinBox::valueChanged, this, &this_t::OnSpinZoom_valueChanged);
		connect(ui->btnZoomIn, &QPushButton::clicked, this, &this_t::OnBtnZoomIn_clicked);
		connect(ui->btnZoomOut, &QPushButton::clicked, this, &this_t::OnBtnZoomOut_clicked);
		connect(ui->btnZoomFit, &QPushButton::clicked, this, &this_t::OnBtnZoomFit_clicked);
		connect(ui->btnSettings, &QPushButton::clicked, this, &this_t::OnBtnSettings_clicked);
		connect(ui->sbHorz, &QScrollBar::valueChanged, this, &this_t::OnSbHorz_valueChanged);
		connect(ui->sbHorz, &QScrollBar::sliderMoved, this, &this_t::OnSbHorz_valueChanged);
		connect(ui->sbVert, &QScrollBar::valueChanged, this, &this_t::OnSbVert_valueChanged);
		connect(ui->sbVert, &QScrollBar::sliderMoved, this, &this_t::OnSbVert_valueChanged);
		connect(&m_smooth_scroll.timer, &QTimer::timeout, this, &this_t::OnSmoothScroll_timeout);
		connect(m_canvas, &QOpenGLWidget::resized, this, &this_t::OnView_resized);
		//connect(ui->btnCountColor, &QPushButton::clicked, this, &this_t::OnBtnCountColor_clicked);
	}

	xMatView::~xMatView() {
		m_gl.Clear();
	}

	bool xMatView::SetImage(cv::Mat const& img, bool bCenter, zoom_t eZoomMode, bool bCopy) {
		xWaitCursor wc;

		StopPyramidMaker();

		// original image
		if (bCopy)
			img.copyTo(m_img);
		else
			m_img = img;

		BuildPyramid();

		// check (opengl) texture format
		auto [eColorType, eFormat, ePixelType] = GetGLImageFormatType(m_img.type());
		if (eColorType == 0)
			return false;

		m_mouse.Clear();
		m_smooth_scroll.Clear();

		//if (img.empty() and !m_img.isContinuous()) {
		//	return false;
		//}

		if (eZoomMode != zoom_t::none) {
			ui->cmbZoomMode->setCurrentIndex(std::to_underlying(eZoomMode));
		}
		UpdateCT(bCenter, eZoomMode);
		UpdateScrollBars();

		if (m_canvas)
			m_canvas->update();
		return true;
	}

	void xMatView::Reset() {
		m_mouse.Clear();
		m_smooth_scroll.Clear();
		m_ctScreenFromImage = {};
		StopPyramidMaker();
		m_img.release();
		m_pyramid.imgs.clear();

	}

	bool xMatView::SetPalette(cv::Mat const& palette, bool bUpdateView) {
		bool bCopied{};
		if (true
			&& (palette.type() == CV_8UC1 || palette.type() == CV_8UC3)
			&& palette.size() == cv::Size(1, 256)
			)
		{
			palette.copyTo(m_palette);
			bCopied = true;
		}
		else {
			m_palette.release();
		}

		if (bUpdateView and m_canvas)
			m_canvas->update();

		return bCopied;
	}

	bool xMatView::SetZoomMode(zoom_t eZoomMode, bool bCenter) {
		ui->cmbZoomMode->setCurrentIndex(std::to_underlying(eZoomMode));
		m_eZoom = eZoomMode;
		UpdateCT(bCenter, eZoomMode);
		UpdateScrollBars();
		if (m_canvas)
			m_canvas->update();
		return true;
	}

	void xMatView::SetSelectionRect(xRect2i const& rect) {
		m_mouse.bRectSelected = true;
		m_mouse.ptSel0 = rect.pt0();
		m_mouse.ptSel1 = rect.pt1();
		if (m_canvas)
			m_canvas->update();
	}
	void xMatView::ClearSelectionRect() {
		m_mouse.bRectSelected = false;
	}

	bool xMatView::SetOption(option_t const& option, bool bStore) {
		if (&m_option != &option)
			m_option = option;

		BuildPyramid();

		UpdateCT(false, zoom_t::none);
		UpdateScrollBars();
		if (m_canvas)
			m_canvas->update();

		if (bStore && m_fnSyncSetting) {
			return m_fnSyncSetting(true, m_strCookie, m_option);
		}
		return true;
	}

	bool xMatView::ShowToolBar(bool bShow) {
		if (!ui->toolbar)
			return false;
		if (bShow)
			ui->toolbar->show();
		else
			ui->toolbar->hide();
		return true;
	}

	bool xMatView::IsToolBarShown() const {
		if (!ui->toolbar)
			return false;
		return ui->toolbar->isVisible();
	}


	bool xMatView::UpdateCT(bool bCenter, zoom_t eZoom) {
		if (m_img.empty())
			return false;
		if (eZoom == zoom_t::none)
			eZoom = m_eZoom;
		ui->cmbZoomMode->setCurrentIndex(std::to_underlying(eZoom));

		xBounds2i rectClient = GetViewRect();
		xSize2i sizeClient = rectClient.size();

		// scale
		double dScale{-1.0};
		switch (eZoom) {
		case zoom_t::one2one:		dScale = 1.0; break;
		case zoom_t::fit2window:	dScale = std::min((double)sizeClient.width / m_img.cols, (double)sizeClient.height / m_img.rows); break;
		case zoom_t::fit2width:		dScale = (double)sizeClient.width / m_img.cols; break;
		case zoom_t::fit2height:	dScale = (double)sizeClient.height / m_img.rows; break;
		//case free:			dScale = m_ctScreenFromImage.m_scale; break;
		}
		if (dScale > 0)
			m_ctScreenFromImage.m_scale = dScale;

		// constraints. make image put on the center of the screen
		if ( bCenter or IsOneOf(eZoom, zoom_t::fit2window, zoom_t::fit2width, zoom_t::fit2height) ) {
			ct_t ct2 = m_ctScreenFromImage;
			ct2.m_origin = xPoint2d(m_img.size())/2.;
			ct2.m_offset = xBounds2d(rectClient).CenterPoint();

			xPoint2d ptOrigin = {};
			xPoint2i ptLT = ct2(ptOrigin);

			if (bCenter or eZoom == zoom_t::fit2window) {
				m_ctScreenFromImage.m_origin = {};
				m_ctScreenFromImage.m_offset = ptLT;
			}
			else if (eZoom == zoom_t::fit2width) {
				m_ctScreenFromImage.m_origin.x = 0;
				m_ctScreenFromImage.m_offset.x = ptLT.x;
			}
			else if (eZoom == zoom_t::fit2height) {
				m_ctScreenFromImage.m_origin.y = 0;
				m_ctScreenFromImage.m_offset.y = ptLT.y;
			}
		}

		// panning constraints.
		xPoint2d pt0 = m_ctScreenFromImage(xPoint2d(0,0));
		xPoint2d pt1 = m_ctScreenFromImage(xPoint2d(m_img.cols, m_img.rows));
		xBounds2i rectImageScreen(pt0, pt1);
		rectImageScreen.Normalize();
		if (m_option.bExtendedPanning) {
			int marginX {std::min(m_option.nScrollMargin, rectImageScreen.Width())};
			int marginY {std::min(m_option.nScrollMargin, rectImageScreen.Height())};
			// make any part of image stays inside the screen
			if (rectImageScreen.r < rectClient.l) {
				m_ctScreenFromImage.m_offset.x += rectClient.l - rectImageScreen.r + marginX;
			}
			else if (rectImageScreen.l > rectClient.r) {
				m_ctScreenFromImage.m_offset.x += rectClient.r - rectImageScreen.l - marginX;
			}
			if (rectImageScreen.b < rectClient.t) {
				m_ctScreenFromImage.m_offset.y += rectClient.t - rectImageScreen.b + marginY;
			}
			else if (rectImageScreen.t > rectClient.b) {
				m_ctScreenFromImage.m_offset.y += rectClient.b - rectImageScreen.t - marginY;
			}
		} else {
			// default panning. make image stays inside the screen
			if (rectImageScreen.Width() <= rectClient.Width()) {
				auto pt = m_ctScreenFromImage(xPoint2d(m_img.cols/2, 0));
				m_ctScreenFromImage.m_offset.x += rectClient.CenterPoint().x - pt.x;
			}
			if (rectImageScreen.Width() > rectClient.Width()) {
				if (rectImageScreen.l > rectClient.l)
					m_ctScreenFromImage.m_offset.x += rectClient.l - rectImageScreen.l;
				else if (rectImageScreen.r < rectClient.r)
					m_ctScreenFromImage.m_offset.x += rectClient.r - rectImageScreen.r;
			}
			if (rectImageScreen.Height() <= rectClient.Height()) {
				auto pt = m_ctScreenFromImage(xPoint2d(0, m_img.rows/2));
				m_ctScreenFromImage.m_offset.y += rectClient.Height() / 2 - pt.y;
			}
			if (rectImageScreen.Height() > rectClient.Height()) {
				if (rectImageScreen.t > rectClient.t)
					m_ctScreenFromImage.m_offset.y += rectClient.t - rectImageScreen.t;
				else if (rectImageScreen.b < rectClient.b)
					m_ctScreenFromImage.m_offset.y += rectClient.b - rectImageScreen.b;
			}
		}

		return true;
	}

	xMatView::S_SCROLL_GEOMETRY xMatView::GetScrollGeometry() {
		xBounds2i rectClient{GetViewRect()};
		auto pt0 = m_ctScreenFromImage(xPoint2d(0,0));
		auto pt1 = m_ctScreenFromImage(xPoint2d(m_img.cols, m_img.rows));
		xBounds2i rectImageScreen(pt0, pt1);
		rectImageScreen.Normalize();
		xBounds2i rectScrollRange(rectClient);
		if (m_option.bExtendedPanning) {
			rectScrollRange.pt0() -= rectImageScreen.size();
			rectScrollRange.pt1() += rectClient.size();
			int nScrollMarginX = std::min(m_option.nScrollMargin, rectImageScreen.Width());
			int nScrollMarginY = std::min(m_option.nScrollMargin, rectImageScreen.Height());
			rectScrollRange.Deflate(nScrollMarginX, nScrollMarginY);
		} else {
			rectScrollRange |= rectImageScreen;
		}
		return {rectClient, rectImageScreen, rectScrollRange};
	}

	bool xMatView::UpdateScrollBars() {
		if (m_img.empty())
			return false;
		auto [rectClient, rectImageScreen, rectScrollRange] = GetScrollGeometry();

		if (QScrollBar* sb = ui->sbHorz; sb and sb->isVisible()) {
			int p = m_option.bExtendedPanning
				? rectScrollRange.Width() - (m_ctScreenFromImage.m_offset.x + std::max(0, rectImageScreen.Width() - m_option.nScrollMargin) + rectClient.Width())
				: -m_ctScreenFromImage.m_offset.x;
			if (rectScrollRange.Width() > 0) {
				p = std::clamp(p, 0, rectScrollRange.Width());
				sb->setRange(0, std::max(0, rectScrollRange.Width() - rectClient.Width()));
				sb->setPageStep(rectClient.Width());
				sb->setSliderPosition(p);
			}
		}

		if (QScrollBar* sb = ui->sbVert; sb and sb->isVisible()) {
			int p = m_option.bExtendedPanning
				? rectScrollRange.Height() - (m_ctScreenFromImage.m_offset.y + std::max(0, rectImageScreen.Height() - m_option.nScrollMargin) + rectClient.Height())
				: -m_ctScreenFromImage.m_offset.y;
			if (rectScrollRange.Height() > 0) {
				p = std::clamp(p, 0, rectScrollRange.Height());
				sb->setRange(0, std::max(0, rectScrollRange.Height()-rectClient.Height()));
				sb->setPageStep(rectClient.Height());
				sb->setSliderPosition(p);
			}
		}

		if (m_ctScreenFromImage.m_scale != ui->spinZoom->value()*1.e-2) {
			m_bSkipSpinZoomEvent = true;
			ui->spinZoom->setValue(m_ctScreenFromImage.m_scale*1.e2);
			m_bSkipSpinZoomEvent = false;
		}

		return true;
	}

	bool xMatView::ZoomInOut(double step, xPoint2i ptAnchor, bool bCenter) {
		auto scale = m_ctScreenFromImage.m_scale;
		if (step > 0) {
			for (auto dZoom : dZoomLevels) {
				if (dZoom > scale) {
					scale = dZoom;
					break;
				}
			}
		}
		else {
			for (auto pos = std::rbegin(dZoomLevels); pos != std::rend(dZoomLevels); pos++) {
				auto dZoom = *pos;
				if (dZoom < scale) {
					scale = dZoom;
					break;
				}
			}
		}
		return SetZoom(scale, ptAnchor, bCenter);
	}

	bool xMatView::SetZoom(double scale, xPoint2i ptAnchor, bool bCenter) {
		// Backup Image Position
		auto ctImageFromScreen = m_ctScreenFromImage.GetInverse();
		if (!ctImageFromScreen)
			return false;
		xPoint2d ptImage = (*ctImageFromScreen)(xPoint2d(ptAnchor));
		// Get Scroll Amount
		if (m_ctScreenFromImage.m_scale == scale)
			return true;
		else
			m_ctScreenFromImage.m_scale = scale;

		if (m_img.empty())
			return false;

		auto rectClient = GetViewRect();
		auto dMinZoom = std::min(rectClient.width/4. / m_img.cols, rectClient.height/4. / m_img.rows);
		dMinZoom = std::min(dMinZoom, 0.5);
		m_ctScreenFromImage.m_scale = std::clamp(m_ctScreenFromImage.m_scale, dMinZoom, 1.e3);
		m_ctScreenFromImage.m_offset += ptAnchor - m_ctScreenFromImage(ptImage);
		// Anchor point
		auto eZoom = m_eZoom;
		if (eZoom != zoom_t::mouse_wheel_locked)
			eZoom = zoom_t::free;
		ui->cmbZoomMode->setCurrentIndex(std::to_underlying(eZoom));
		//OnCmbZoomMode_currentIndexChanged(std::to_underlying(eZoom));
		UpdateCT(bCenter);
		UpdateScrollBars();
		if (m_canvas)
			m_canvas->update();
		return true;
	}

	bool xMatView::ScrollTo(xPoint2d pt, std::chrono::milliseconds tsScroll) {
		if (tsScroll == 0ms) {
			m_smooth_scroll.timer.stop();	// 이미 동작하고 있는 Scroll 중지
			m_ctScreenFromImage.m_offset = pt;
			UpdateCT(false);
			UpdateScrollBars();
			if (m_canvas)
				m_canvas->update();
		}
		else {
			m_smooth_scroll.pt0 = m_ctScreenFromImage.m_offset;
			m_smooth_scroll.pt1 = pt;
			m_smooth_scroll.t0 = std::chrono::steady_clock::now();
			if (tsScroll < 0ms)
				tsScroll = m_option.tsScroll;
			m_smooth_scroll.t1 = m_smooth_scroll.t0 + tsScroll;
			m_smooth_scroll.timer.start(10ms);
		}
		return true;
	}

	bool xMatView::Scroll(xPoint2d delta, std::chrono::milliseconds tsScroll) {
		return ScrollTo(m_ctScreenFromImage.m_offset + delta, tsScroll);
	}

	void xMatView::PurgeScroll(bool bUpdate) {
		if (!m_smooth_scroll.timer.isActive())
			return;
		m_smooth_scroll.timer.stop();
		m_ctScreenFromImage.m_offset = m_smooth_scroll.pt1;
		m_smooth_scroll.Clear();
		if (bUpdate) {
			UpdateCT(false);
			UpdateScrollBars();
			if (m_canvas)
				m_canvas->update();
		}
	}

	bool xMatView::KeyboardNavigate(int key, bool ctrl, bool alt, bool shift) {
		if (ctrl or alt or m_img.empty())
			return false;

		// if scroll is not finished, use target scroll position
		PurgeScroll();

		auto rCT = m_ctScreenFromImage.GetInverse();
		if (!rCT)
			return false;
		auto& ctS2I = *rCT;
		xRect2i rectClient{GetViewRect()};
		double scale = ctS2I.Scale2d();

		switch (key) {
		case Qt::Key_Left:
		case Qt::Key_Right:
		case Qt::Key_Up:
		case Qt::Key_Down:
		case Qt::Key_PageUp:
		case Qt::Key_PageDown:
			{
				auto ptShift = ctS2I(xPoint2d{100., 100.}) - ctS2I(xPoint2d{0., 0.});
				auto ptShiftPage = ctS2I(rectClient.pt1()) - ctS2I(rectClient.pt0());
				ptShiftPage *= 0.9;

				auto tsScroll = -1ms;

				xPoint2d delta;
				if (key == Qt::Key_Left)			{ delta.x += ptShift.x;	tsScroll = 0ms; }
				else if (key == Qt::Key_Right)		{ delta.x -= ptShift.x;	tsScroll = 0ms; }
				else if (key == Qt::Key_Up)			{ delta.y += ptShift.y;	tsScroll = 0ms; }
				else if (key == Qt::Key_Down)		{ delta.y -= ptShift.y;	tsScroll = 0ms; }
				else if (key == Qt::Key_PageUp)		{ delta.y += ptShiftPage.y; }
				else if (key == Qt::Key_PageDown)	{ delta.y -= ptShiftPage.y; }

				Scroll(delta, 0ms);
			}
			break;

		case Qt::Key_Space :
			{
				int heightMin { (int) (scale * rectClient.height * 5.0 / 10) };
				int deltaDefaultScreen{ rectClient.height * 95 / 100 };
				double deltaImage { scale * deltaDefaultScreen };
				int heightMax { (int) ((scale * rectClient.height) * 9.5 / 10) };
				xPoint2i ptImgBottom = ctS2I(xPoint2i{rectClient.x, rectClient.pt1().y});
				xPoint2i ptImgTop = ctS2I(rectClient.pt0());
				auto y0 = std::min(ptImgBottom.y + heightMax, m_img.rows - 1);

				auto CheckIfBlank = [&](int y) -> bool {
					constexpr static int margin = 5;
					cv::Rect rc(margin, y, m_img.cols-2*margin, 1);
					if (!IsROI_Valid(rc, m_img.size()))
						return false;
					auto imgRow = m_img(rc);
					if (imgRow.empty())
						return true;
					cv::Scalar mean, stddev;
					cv::meanStdDev(imgRow, mean, stddev);
					for (int i{}, nChannel = std::min(3, imgRow.channels()); i < nChannel; i++) {
						constexpr auto threshold = 4.0;
						if (stddev[i] > threshold)
							return false;
					}
					return true;
				};

				{
					if (CheckIfBlank(ptImgBottom.y)) {
						auto yE = m_img.rows;
						for (auto y = ptImgBottom.y; y < yE; y++) {
							if (!CheckIfBlank(y)) {
								deltaImage = y-ptImgTop.y;
								break;
							}
						}
					}
					else {
						deltaImage = heightMax;
						auto y2 = ptImgTop.y + heightMin*1/2;
						auto y = ptImgBottom.y;
						for (; y >= y2; y--) {
							if (CheckIfBlank(y)) {
								deltaImage = y - ptImgTop.y;
								break;
							}
						}
						if (y < y2) {	// not found
							auto yE = ptImgBottom.y + heightMax;
							for (; y < yE; y++) {
								if (CheckIfBlank(y)) {
									deltaImage = y - ptImgBottom.y;
									break;
								}
							}
						}
					}
				}

				deltaImage *= m_ctScreenFromImage.m_scale;
				Scroll({0., -deltaImage});
			}
			break;

		case Qt::Key_Home :
			ScrollTo({});
			break;

		case Qt::Key_End :
			{
				auto pt0 = m_ctScreenFromImage(xPoint2d{0.0, (double)0.0});
				auto pt1 = m_ctScreenFromImage(xPoint2d{0.0, (double)m_img.rows});
				auto p = (pt0.y - pt1.y) - rectClient.height;
				ScrollTo({0., p});
			}
			break;

		default:
			return false;
		}
		return true;
	}

	void xMatView::BuildPyramid() {
		// Build Pyramid Image for down sampling { cv::InterpolationFlags::INTER_AREA }
		StopPyramidMaker();
		m_pyramid.imgs.clear();
		if (m_img.empty())
			return;
		cv::Mat img;
		ApplyPalette(m_img, img);
		m_pyramid.imgs.push_front(img);
		const uint minArea = 1'000 * 1'000;
		if (m_option.bPyrImageDown and m_option.eZoomOut == zoom_out_t::area and ((uint64_t)img.cols * img.rows) > minArea) {
			m_pyramid.threadPyramidMaker = std::jthread([this](std::stop_token stop) {
				cv::Mat imgPyr = m_pyramid.imgs[0];
				while (!stop.stop_requested() and ((uint64_t)imgPyr.cols * imgPyr.rows) > minArea) {
					cv::pyrDown(imgPyr, imgPyr);
					{
						std::unique_lock lock{ m_pyramid.mtx };
						m_pyramid.imgs.emplace_front(imgPyr);
					}
				}
			});
		}
	}

	void xMatView::StopPyramidMaker() {
		if (m_pyramid.threadPyramidMaker.joinable()) {
			m_pyramid.threadPyramidMaker.request_stop();
			m_pyramid.threadPyramidMaker.join();
		}
	}

	xRect2i xMatView::GetViewRect() {
		if (!m_canvas)
			return {};
		xRect2i rect = ToCoordRect(m_canvas->rect());
		rect.pt() = {};
		for (auto r = devicePixelRatio(); auto& v : rect.arr())
			v *= r;
		return rect;
	}

	void xMatView::keyPressEvent(QKeyEvent* event) {
		// Print Char Pressed
		auto l = std::source_location::current();
		//OutputDebugStringW(std::format(L"{}: {}\n", biscuit::ConvertString<wchar_t>(l.function_name()), event->key()).c_str());
		auto bControl = event->modifiers() & Qt::ControlModifier;
		auto bAlt =	event->modifiers() & Qt::AltModifier;
		auto bShift = event->modifiers() & Qt::ShiftModifier;
		auto code = event->key();

		if ( (code == Qt::Key_Escape)
			and (m_mouse.bRectSelected or m_mouse.bInSelectionMode)
			)
		{
			m_mouse.bRectSelected = m_mouse.bInSelectionMode = false;
			if (m_canvas)
				m_canvas->update();
			event->accept();
			return ;
		}

		if ( bControl and bAlt and !bShift and (code == 'M') ) {
			auto show = !ui->toolbar->isVisible();
			ShowToolBar(show);
			event->accept();
			return ;
		}

		if (m_option.bKeyboardNavigation
			and KeyboardNavigate(code, bControl, bAlt, bShift))
		{
			event->accept();
			return;
		}

		event->ignore();
	}

	void xMatView::OnView_mousePressEvent(xMatViewCanvas* canvas, QMouseEvent* event) {
		if (!canvas)
			return;

		if (emit SigMousePressed(canvas, event)) {
			event->accept();
			return;
		}

		auto btn = event->button();
		auto iter = m_mapMouseAction.find({btn, event->modifiers()});
		if (iter == m_mapMouseAction.end()) {
			return;
		}
		auto action = iter->second;
		xPoint2i ptView = ToCoord(event->pos() * devicePixelRatio());
		switch (action) {
		case eMOUSE_ACTION::pan:
			{
				if (m_option.bPanningLock and (m_eZoom == zoom_t::fit2window))
					return;
				if (mouseGrabber())
					return;
				event->accept();
				canvas->grabMouse();
				m_mouse.ptAnchor = ptView;
				m_mouse.ptOffset0 = m_ctScreenFromImage.m_offset;
				m_current_mouse_action[btn] = action;
			}
			break;
		case eMOUSE_ACTION::select:
			{
				auto ctI = m_ctScreenFromImage.GetInverse();
				if (!ctI)
					return;
				event->accept();
				m_mouse.bRectSelected = false;
				m_mouse.bInSelectionMode = true;
				auto pt = (*ctI)(ptView);
				m_mouse.ptSel0.x = std::clamp<int>(pt.x, 0, m_img.cols);
				m_mouse.ptSel0.y = std::clamp<int>(pt.y, 0, m_img.rows);
				m_mouse.ptSel1 = m_mouse.ptSel0;
				m_current_mouse_action[btn] = action;
				canvas->update();
			}
			break;
		}
	}

	void xMatView::OnView_mouseReleaseEvent(xMatViewCanvas* canvas, QMouseEvent* event) {
		if (!canvas)
			return;

		if (emit SigMouseReleased(canvas, event)) {
			event->accept();
			return;
		}

		auto btn = event->button();
		auto action = m_current_mouse_action[btn];
		m_current_mouse_action[btn] = eMOUSE_ACTION::none;
		switch (action) {
		case eMOUSE_ACTION::pan:
			{
				if (!mouseGrabber())
					return;
				event->accept();
				canvas->releaseMouse();
				m_mouse.ptAnchor.reset();
			}
			break;
		case eMOUSE_ACTION::select:
			if (m_mouse.bInSelectionMode) {
				auto ctI = m_ctScreenFromImage.GetInverse();
				if (!ctI)
					return;
				event->accept();
				m_mouse.bInSelectionMode = false;
				m_mouse.bRectSelected = true;
				xPoint2i ptView = ToCoord(event->pos() * devicePixelRatio());
				auto pt = (*ctI)(ptView);
				m_mouse.ptSel1.x = std::clamp<int>(pt.x, 0, m_img.cols);
				m_mouse.ptSel1.y = std::clamp<int>(pt.y, 0, m_img.rows);
			}
			break;
		}
	}

	void xMatView::OnView_mouseDoubleClickEvent(xMatViewCanvas* canvas, QMouseEvent* event) {
		if (emit SigMouseDoubleClicked(canvas, event)) {
			event->accept();
			return;
		}
	}

	void xMatView::OnView_mouseMoveEvent(xMatViewCanvas* canvas, QMouseEvent* event) {
		static std::locale l("en_US.UTF-8");
		if (!canvas)
			return;

		if (emit SigMouseMoved(canvas, event)) {
			event->accept();
			return;
		}

		event->accept();
		xPoint2d ptView = ToCoord(event->pos()*devicePixelRatio());
		if (m_mouse.ptAnchor) {
			if (!m_option.bPanningLock) {
				switch (m_eZoom) {
				case zoom_t::one2one: break;
				case zoom_t::mouse_wheel_locked: break;
				case zoom_t::free: break;
				default :
					m_eZoom = zoom_t::free;
					ui->cmbZoomMode->setCurrentIndex(std::to_underlying(m_eZoom));
					break;
				}
			}
			auto dPanningSpeed = m_mouse.bInSelectionMode ? 1.0 : m_option.dPanningSpeed;
			auto ptOffset = (ptView - *m_mouse.ptAnchor) * dPanningSpeed;
			if (m_eZoom == zoom_t::fit2width)
				ptOffset.x = 0;
			if (m_eZoom == zoom_t::fit2height)
				ptOffset.y = 0;
			m_ctScreenFromImage.m_offset = m_mouse.ptOffset0 + ptOffset;
			UpdateCT();
			UpdateScrollBars();
			canvas->update();
		}

		auto ctI = m_ctScreenFromImage.GetInverse();

		// Selection Mode
		if (m_mouse.bInSelectionMode) {
			auto pt = ctI ? (*ctI)(ptView) : ptView;
			m_mouse.ptSel1.x = std::clamp<int>(pt.x, 0, m_img.cols);
			m_mouse.ptSel1.y = std::clamp<int>(pt.y, 0, m_img.rows);
			canvas->update();
		}

		// status
		{
			xPoint2i ptImage = biscuit::Floor((*ctI)(xPoint2d(ptView)));
			std::wstring status;

			// Current Position
			int nx{}, ny{};
			{
				for (auto v = m_img.cols; v; v/= 10, nx++);
				nx = nx*4/3;
				for (auto v = m_img.rows; v; v/= 10, ny++);
				ny = ny*4/3;
				// print ptImage.x and ptImage.y with thousand comma separated
				if (!m_img.empty())
					status += fmt::format(L"(w{} h{}) ",
										  AddThousandCommaW((int)m_img.cols),
										  AddThousandCommaW((int)m_img.rows));
				status += fmt::format(L"[x{:>{}} y{:>{}}]",
									  AddThousandCommaW((int)ptImage.x), nx,
									  AddThousandCommaW((int)ptImage.y), ny);
			}

			// image value
			{
				int n = m_img.channels();
				if (xRect2i(0, 0, m_img.cols, m_img.rows).Contains(ptImage)) {
					int depth = m_img.depth();
					auto cr = GetMatValue(m_img.ptr(ptImage.y), depth, n, ptImage.y, ptImage.x);
					auto strValue = std::format(L" [{:3}", cr[0]);
					for (int i{1}; i < n; i++)
						strValue += std::format(L",{:3}", cr[i]);
					status += strValue + L"]";
				}
				else {
					auto strValue = std::format(L" [{:3}", ' ');
					for (int i{1}; i < n; i++)
						strValue += std::format(L" {:3}", ' ');
					status += strValue + L"]";
				}
			}

			// Selection
			if (m_mouse.bInSelectionMode or m_mouse.bRectSelected) {
				xSize2i size = m_mouse.ptSel1 - m_mouse.ptSel0;
				status += fmt::format(L" (x{0} y{1} w{2} h{3})",
					AddThousandCommaW((int)m_mouse.ptSel0.x), AddThousandCommaW((int)m_mouse.ptSel0.y),
					AddThousandCommaW(std::abs(size.width)), AddThousandCommaW(std::abs(size.height)));
			}


			if (auto str = ToQString(status); str != ui->edtInfo->text()) {
				ui->edtInfo->setText(str);
				ui->edtInfo->setSelection(0, 0);
			}
		}
	}

	void xMatView::OnView_wheelEvent(xMatViewCanvas* canvas, QWheelEvent* event) {
		if (!canvas)
			return;
		auto iter = m_mapWheelAction.find(event->modifiers());
		if (iter == m_mapWheelAction.end()) {
			return;
		}
		auto action = iter->second;
		switch (action) {
		case eWHEEL_ACTION::zoom:
			{
		if ((m_eZoom == zoom_t::mouse_wheel_locked) or (m_option.bZoomLock and m_eZoom != zoom_t::free)) {
			return;
		}
		event->accept();
		ZoomInOut(event->angleDelta().y(), ToCoord(event->position()*devicePixelRatio()), false);
			}
			break;
		case eWHEEL_ACTION::scroll:
			{
				if ((m_eZoom == zoom_t::mouse_wheel_locked) or (m_option.bPanningLock and m_eZoom != zoom_t::free)) {
					return;
				}
				event->accept();
				auto pt = xPoint2d(event->angleDelta().x(), event->angleDelta().y());
				Scroll(pt);
			}
			break;
		}
	}

	void xMatView::OnCmbZoomMode_currentIndexChanged(int index) {
		//static int count{};
		//OutputDebugStringA(std::format("{} : index : {}, count {}\n", std::source_location{}.function_name(), index, count++).c_str());
		{
			zoom_t cur = (zoom_t)index;
			if (m_eZoom == cur and IsOneOf(m_eZoom, zoom_t::fit2window, zoom_t::one2one, zoom_t::mouse_wheel_locked, zoom_t::free))
				return;
			m_eZoom = cur;
		}

		//// Scroll Bar Visibility
		//bool bHorz{true}, bVert{true};
		//m_canvas->AlwaysShowScrollbars(bHorz, bVert);

		UpdateCT(true);
		UpdateScrollBars();
		if (m_canvas)
			m_canvas->update();
	}

	void xMatView::OnSpinZoom_valueChanged(double value) {
		auto scale = value * 1.e-2;
		static auto const dMinZoom = dZoomLevels[0];
		static auto const dMaxZoom = dZoomLevels[std::size(dZoomLevels)-1];
		scale = std::clamp<double>(scale, dMinZoom, dMaxZoom);
		if (m_bSkipSpinZoomEvent)
			return;
		if ( m_option.bZoomLock and IsNoneOf(m_eZoom, zoom_t::free, zoom_t::mouse_wheel_locked) )
			return;
		if (m_ctScreenFromImage.m_scale == scale)
			return;
		auto rect{GetViewRect()};
		auto ctI = m_ctScreenFromImage.GetInverse();
		if (!ctI)
			return;
		auto ptImage = (*ctI)(rect.CenterPoint());
		m_ctScreenFromImage.m_scale = scale;
		auto pt2 = m_ctScreenFromImage(ptImage);
		m_ctScreenFromImage.m_offset += rect.CenterPoint() - m_ctScreenFromImage(ptImage);
		UpdateScrollBars();
		if (m_canvas)
			m_canvas->update();
	}

	void xMatView::OnBtnZoomIn_clicked() {
		//if ( m_option.bZoomLock and IsNoneOf(m_eZoom, zoom_t::free, zoom_t::mouse_wheel_locked) )
		//	return;
		ZoomInOut(100, GetViewRect().CenterPoint(), false);
	}

	void xMatView::OnBtnZoomOut_clicked() {
		//if ( m_option.bZoomLock and IsNoneOf(m_eZoom, zoom_t::free, zoom_t::mouse_wheel_locked) )
		//	return;
		ZoomInOut(-100, GetViewRect().CenterPoint(), false);
	}

	void xMatView::OnBtnZoomFit_clicked() {
		//if ( m_option.bZoomLock and IsNoneOf(m_eZoom, zoom_t::free, zoom_t::mouse_wheel_locked) )
		//	return;
		UpdateCT(true, zoom_t::fit2window);
		UpdateScrollBars();
		if (m_canvas)
			m_canvas->update();
	}

	void xMatView::OnBtnCountColor_clicked() {
		if (m_img.empty() or m_img.channels() != 1)
			return;

		int channels[] = {0};
		int histSize[] = {256};
		cv::MatND matHist;
		float sranges[] = { 0, 256 };
		const float* ranges[] = { sranges };
		cv::calcHist(&m_img, 1, channels, cv::Mat(), matHist, 1, histSize, ranges);
		std::wstring str;
		for (int i{}; i < matHist.rows; i++) {
			if (matHist.at<float>(i, 0) > 0) {
				// add locale en_US.utf8
				str += fmt::format(L"Value {} : ", i);
				str += fmt::format(std::locale(".UTF-8"), L"{:L}", (int)matHist.at<float>(i, 0));
				str += L"\n";
			}
		}
		QMessageBox::information(this, "Color Count", ToQString(str));
	}

	void xMatView::OnBtnSettings_clicked() {
		xMatViewSettingsDlg dlg(m_option, this);
		if (dlg.exec() != QDialog::Accepted)
			return;
		SetOption(dlg.m_option);
	}

	void xMatView::OnSmoothScroll_timeout() {
		auto t = std::chrono::steady_clock::now();
		if (m_smooth_scroll.t0 < t and t < m_smooth_scroll.t1) {
			auto& ss = m_smooth_scroll;

			double len = (ss.t1-ss.t0).count();
			double pos = (t-ss.t0).count();
			double s = ::sin(pos / len * std::numbers::pi/2.);
			m_ctScreenFromImage.m_offset = ss.pt0 + (ss.pt1-ss.pt0) * s;
		}
		if (t >= m_smooth_scroll.t1) {
			m_smooth_scroll.timer.stop();
			m_ctScreenFromImage.m_offset = m_smooth_scroll.pt1;
		}
		UpdateCT(false);
		UpdateScrollBars();
		if (m_canvas)
			m_canvas->update();
	}

	void xMatView::OnView_resized() {
		UpdateCT();
		UpdateScrollBars();
	}

	void xMatView::OnSbHorz_valueChanged(int pos) {
		auto [rectClient, rectImageScreen, rectScrollRange] = GetScrollGeometry();
		int range = rectScrollRange.Width();
		int page =  rectClient.Width();
		pos = std::clamp(pos, 0, range);
		m_ctScreenFromImage.m_offset.x = - pos;
		if (m_option.bExtendedPanning)
			m_ctScreenFromImage.m_offset.x += rectScrollRange.Width() - std::max(0, rectImageScreen.Width() - m_option.nScrollMargin) - rectClient.Width();
		UpdateScrollBars();
		if (m_canvas)
			m_canvas->update();
	}
	void xMatView::OnSbVert_valueChanged(int pos) {
		auto [rectClient, rectImageScreen, rectScrollRange] = GetScrollGeometry();
		int range = rectScrollRange.Height();
		int page =  rectClient.Height();
		pos = std::clamp(pos, 0, range);
		m_ctScreenFromImage.m_offset.y = - pos;
		if (m_option.bExtendedPanning)
			m_ctScreenFromImage.m_offset.y += rectScrollRange.Height() - std::max(0, rectImageScreen.Height() - m_option.nScrollMargin) - rectClient.Height();
		UpdateScrollBars();
		if (m_canvas)
			m_canvas->update();
	}

	void xMatView::InitializeGL(xMatViewCanvas* canvas) {
		GLenum err = glewInit();
		if (err != GLEW_OK) {
			const GLubyte* msg = glewGetErrorString(err);
			QString str((char const*)msg);
			QMessageBox::critical(this, "openGL", str);
			return;
			//throw std::exception(reinterpret_cast<const char*>(msg));
		}

		// OpenGL Version in number
		int nMajor = 0;
		glGetIntegerv(GL_MAJOR_VERSION, &nMajor);
		if (nMajor < 3) {
			QMessageBox::critical(this, "openGL", "OpenGL 3.0 or higher is required.");
			return;
		}

		//QString strVersion = QString::fromUtf8((char const*)glGetString(GL_VERSION));
		// openGL Version Major

		// Vertex Shader Source
		static char const* vertexShaderSource = 
R"(
	#version 330 core
	layout(location = 0) in vec2 inPosition;
	layout(location = 1) in vec2 inTexCoord;
	out vec2 TexCoord;
	void main() {
		gl_Position = vec4(inPosition, 0.0, 1.0);
		TexCoord = inTexCoord;
	}
)";

		// Fragment Shader Source
		static char const* fragmentShaderSource =
R"(
	#version 330 core
	in vec2 TexCoord;
	out vec4 FragColor;
	uniform sampler2D textureSampler;
	void main() {
		FragColor = texture(textureSampler, TexCoord);
	}
)";

		// todo: TEMP
		if (!m_gl.gl)
			m_gl.gl = std::make_unique<QOpenGLExtraFunctions>(canvas->context());
		if (!m_gl.shaderProgram) {

			m_gl().glGenVertexArrays(1, &m_gl.VAO);
			m_gl().glBindVertexArray(m_gl.VAO);

			// Create Vertex Buffer Object (VBO)
			m_gl().glGenBuffers(1, &m_gl.VBO);
			m_gl().glBindBuffer(GL_ARRAY_BUFFER, m_gl.VBO);

			// Define vertices and texture coordinates for a quad
			float vertices[] = {
				// Position    // Texture Coordinates
				0.0f, 1.0f,  0.0f, 0.0f,
				1.0f, 0.0f,  1.0f, 0.0f,
				1.0f, 1.0f,  1.0f, 1.0f,
				0.0f, 1.0f,  0.0f, 1.0f,
			};
			m_gl().glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			// Create and compile the vertex shader
			auto vertexShader = m_gl().glCreateShader(GL_VERTEX_SHADER);
			m_gl().glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
			m_gl().glCompileShader(vertexShader);

			// Create and compile the fragment shader
			GLuint fragmentShader = m_gl().glCreateShader(GL_FRAGMENT_SHADER);
			m_gl().glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
			m_gl().glCompileShader(fragmentShader);

			// Create and link the shader program
			m_gl.shaderProgram = m_gl().glCreateProgram();
			m_gl().glAttachShader(m_gl.shaderProgram, vertexShader);
			m_gl().glAttachShader(m_gl.shaderProgram, fragmentShader);
		}
		if (m_gl.shaderProgram) {
			m_gl().glLinkProgram(m_gl.shaderProgram);
			m_gl().glUseProgram(m_gl.shaderProgram);

			m_gl().glUniform1i(m_gl().glGetUniformLocation(m_gl.shaderProgram, "textureSampler"), 0);
		}

		// Specify vertex attribute data
		m_gl().glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		m_gl().glEnableVertexAttribArray(0);
		m_gl().glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		m_gl().glEnableVertexAttribArray(1);

		//// get max bitmap size
		//GLint maxTexSize;
		//glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
		//OutputDebugStringA((std::format("GL_MAX_TEXTURE_SIZE: {}\n", maxTexSize)).c_str());

		//// get opengl Version
		//QString strVersion = QString::fromUtf8((char const*)glGetString(GL_VERSION));
		//QString strVendor = QString::fromUtf8((char const*)glGetString(GL_VENDOR));
		//QString strRenderer = QString::fromUtf8((char const*)glGetString(GL_RENDERER));
		//QString strGLSL = QString::fromUtf8((char const*)glGetString(GL_SHADING_LANGUAGE_VERSION));
		//OutputDebugStringA((std::format("OpenGL Version: {}\n", strVersion.toStdString())).c_str());

	}

	bool xMatView::PutMatAsTexture(GLuint textureID, cv::Mat const& img, int width, xBounds2i const& rect, xRect2i const& rectClient) {
		if (!textureID or img.empty() or !img.isContinuous())
			return false;

		if (!m_gl.gl or !m_gl.shaderProgram or !m_gl.VAO or !m_gl.VBO) {
			return biscuit::PutMatAsTexture(textureID, img, width, rect);
		}

		if (rectClient.width <= 0 or rectClient.height <= 0)
			return false;

		glBindTexture(GL_TEXTURE_2D, textureID);
		if (img.step%4)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		else
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		// Create the texture
		auto [eColorType, eFormat, ePixelType] = GetGLImageFormatType(img.type());
		glTexImage2D(GL_TEXTURE_2D, 0, eColorType, img.cols, img.rows, 0, eFormat, ePixelType, img.ptr());

		m_gl().glUniform1i(m_gl().glGetUniformLocation(m_gl.shaderProgram, "textureSampler"), 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		// patch
		m_gl().glBindVertexArray(m_gl.VAO);
		m_gl().glBindBuffer(GL_ARRAY_BUFFER, m_gl.VBO);

		TBounds<float, 2, false> rc((float)rect.l/rectClient.width, (float)rect.t/rectClient.height, (float)rect.r/rectClient.width, (float)rect.b/rectClient.height);
		for (auto& v : rc.arr()) {
			v = v*2 - 1;
		}

		// Define vertices and texture coordinates for a quad
		auto r = (float)width/img.cols;
		float vertices[] = {
			// Position		// Texture Coordinates
			//-r, -1.0f,		rc.right, rc.top,
			//r, -1.0f,		rc.left, rc.top,
			//r, 1.0f,		rc.left, rc.bottom,
			//-r, 1.0f,		rc.right, rc.bottom,
			rc.l, -rc.t, 0, 0,
			rc.r, -rc.t, r, 0,
			rc.r, -rc.b, r, 1,
			rc.l, -rc.b, 0, 1,
		};

		m_gl().glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		m_gl().glBindVertexArray(0);

		//glBegin(GL_QUADS);
		//glTexCoord2f(0.f, 0.f);	glVertex2i(rect.left,   rect.top);
		//glTexCoord2f(0.f, 1.f);	glVertex2i(rect.left,   rect.bottom);
		//glTexCoord2f(r, 1.f);	glVertex2i(rect.right,  rect.bottom);
		//glTexCoord2f(r, 0.f);	glVertex2i(rect.right,  rect.top);
		//glEnd();

		return true;
	}


	void xMatView::PaintGL(xMatViewCanvas* canvas) {
		//auto t0 = std::chrono::steady_clock::now();

		if (!canvas)
			return;

		//================
		// openGL
		//canvas->makeCurrent();

		// Client Rect
		xRect2i rectClient;
		rectClient = GetViewRect();
		xSize2i const sizeView = rectClient.size();
		glViewport(0, 0, sizeView.width, sizeView.height);

		glMatrixMode(GL_PROJECTION);     // Make a simple 2D projection on the entire window
		glLoadIdentity();
		glOrtho(0.0, sizeView.width, sizeView.height, 0.0, 0.0, 100.0);

		glMatrixMode(GL_MODELVIEW);    // Set the matrix mode to object modeling
		cv::Scalar cr(m_option.crBackground.b, m_option.crBackground.g, m_option.crBackground.r);
		glClearColor(cr[0]/255.f, cr[1]/255.f, cr[2]/255.f, 1.0f);
		glClearDepth(0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the window

		auto* context = canvas->context();
		if (!context)
			return;
		auto* surface = canvas->context()->surface();

		xFinalAction faSwapBuffer{[&]{
			//OutputDebugStringA("Flush\n");
			glFlush();
			context->swapBuffers(surface);
		}};

		//event.Skip();
		if (!canvas or !context)
			return;

		//// Show zoom Scale
		//base_t::m_spinCtrlZoom->SetValue(m_ctScreenFromImage.m_scale);

		if (rectClient.IsEmpty())
			return;

		//==================
		// Get Image - ROI
		auto const& ct = m_ctScreenFromImage;
		auto rctI = ct.GetInverse();
		if (!rctI) return;
		auto& ctI = *rctI;

		// Position
		xBounds2i rectImage;
		rectImage.pt0() = ctI(xPoint2d(rectClient.pt0()));
		rectImage.pt1() = ctI(xPoint2d(rectClient.pt1()));
		rectImage.Normalize();
		rectImage.Inflate(1, 1);
		rectImage &= xBounds2i{ 0, 0, m_img.cols, m_img.rows };
		if (rectImage.IsEmpty())
			return;
		cv::Rect roi(rectImage);
		xBounds2i rectTarget;
		rectTarget.pt0() = ct(rectImage.pt0());
		rectTarget.pt1() = ct(xPoint2d(rectImage.pt1()));
		rectTarget.Normalize();
		if (rectTarget.r == rectTarget.l)
			rectTarget.r = rectTarget.l+1;
		if (rectTarget.b == rectTarget.t)
			rectTarget.b = rectTarget.t+1;
		if (!IsROI_Valid(roi, m_img.size()))
			return;

		// img (roi)
		cv::Rect rcTarget(cv::Point2i{}, cv::Size2i(rectTarget.Width(), rectTarget.Height()));
		cv::Rect rcTargetC(rcTarget);	// 4 byte align
		if (rcTarget.width*m_img.elemSize() % 4)
			rcTargetC.width = AdjustAlign32(rcTargetC.width);
		// check target image size
		if ((uint64_t)rcTargetC.width * rcTargetC.height > 1ull *1024*1024*1024)
			return;

		int imgType = (m_img.type() == CV_8UC1 and !m_palette.empty()) ? m_palette.type() : m_img.type();
		cv::Mat img(rcTargetC.size(), imgType);
		//img = m_option.crBackground;
		int eInterpolation = cv::INTER_LINEAR;
		try {
			if (ct.m_scale < 1.) {
				static std::unordered_map<zoom_out_t, cv::InterpolationFlags> const mapInterpolation = {
					{zoom_out_t::nearest, cv::InterpolationFlags::INTER_NEAREST}, {zoom_out_t::area, cv::InterpolationFlags::INTER_AREA},
				};
				if (auto pos = mapInterpolation.find(m_option.eZoomOut); pos != mapInterpolation.end())
					eInterpolation = pos->second;

				// resize from pyramid image
				cv::Size2d size {ct.m_scale*m_img.cols, ct.m_scale*m_img.rows};
				cv::Mat imgPyr;
				//{
				//	auto t = std::chrono::steady_clock::now();
				//	OutputDebugStringA(std::format("=======================\n1 {}\n", std::chrono::duration_cast<std::chrono::milliseconds>(t-t0)).c_str());
				//	t0 = t;
				//}
				{
					std::unique_lock lock{m_pyramid.mtx};
					for (auto const& img : m_pyramid.imgs) {
						if (img.cols < size.width)
							continue;
						imgPyr = img;
						break;
					}
				}
				//{
				//	auto t = std::chrono::steady_clock::now();
				//	OutputDebugStringA(std::format("=======================\n2 {}\n", std::chrono::duration_cast<std::chrono::milliseconds>(t-t0)).c_str());
				//	t0 = t;
				//}
				if (!imgPyr.empty()) {

					xWaitCursor wc;

					double scaleP = m_img.cols < m_img.rows ? (double)imgPyr.rows / m_img.rows : (double)imgPyr.cols / m_img.cols;
					double scale = imgPyr.cols < imgPyr.rows ? (double)size.height / imgPyr.rows : (double)size.width / imgPyr.cols;
					cv::Rect roiP(roi);
					roiP.x *= scaleP;
					roiP.y *= scaleP;
					roiP.width *= scaleP;
					roiP.height *= scaleP;
					roiP = GetSafeROI(roiP, imgPyr.size());
					if (!roiP.empty()) {
						//cv::resize(imgPyr(roiP), img(rcTarget), rcTarget.size(), 0., 0., eInterpolation);
						cv::Mat imgSrc(imgPyr(roiP));
						cv::Mat imgDest;
					#ifdef HAVE_CUDA
						if (IsGPUEnabled()) {
							try {
								cv::cuda::GpuMat dst;
								cv::cuda::resize(cv::cuda::GpuMat(imgSrc), dst, rcTarget.size(), 0., 0., eInterpolation);
								dst.download(imgDest);
							}
							catch (...) {
								//TRACE((GTL__FUNCSIG " - Error\n").c_str());
							}
						}
					#endif
						//#endif
						if (imgDest.empty())
							cv::resize(imgSrc, img(rcTarget), rcTarget.size(), 0., 0., eInterpolation);
						else {
							if (imgDest.size() == rcTarget.size())
								imgDest.copyTo(img(rcTarget));
						}
					}
				}
				//{
				//	auto t = std::chrono::steady_clock::now();
				//	OutputDebugStringA(std::format("=======================\n3 {}\n", std::chrono::duration_cast<std::chrono::milliseconds>(t-t0)).c_str());
				//	t0 = t;
				//}
			}
			else if (ct.m_scale > 1.) {
				static std::unordered_map<zoom_in_t, cv::InterpolationFlags> const mapInterpolation = {
					{zoom_in_t::nearest, cv::InterpolationFlags::INTER_NEAREST}, {zoom_in_t::linear, cv::InterpolationFlags::INTER_LINEAR},
					{zoom_in_t::bicubic, cv::InterpolationFlags::INTER_CUBIC}, {zoom_in_t::lanczos4, cv::InterpolationFlags::INTER_LANCZOS4},
				};
				if (auto pos = mapInterpolation.find(m_option.eZoomIn); pos != mapInterpolation.end())
					eInterpolation = pos->second;
				// ApplyColorMap
				cv::Mat imgC;
				ApplyPalette(m_img(roi), imgC);
				cv::resize(imgC, img(rcTarget), rcTarget.size(), 0., 0., eInterpolation);
			}
			else {
				cv::Mat imgC;
				ApplyPalette(m_img(roi), imgC);
				imgC.copyTo(img(rcTarget));
			}
		} catch (std::exception& ) {
			//OutputDebugStringA(std::format("cv::{}.......\n", e.what()).c_str());
		} catch (...) {
			//OutputDebugStringA("cv::.......\n");
		}

		if (!img.empty()) {
			if (m_option.bDrawPixelValue) {
				auto ctCanvas = m_ctScreenFromImage;
				ctCanvas.m_offset -= m_ctScreenFromImage(roi.tl());
				DrawPixelValue(img, m_img, roi, ctCanvas, 8*devicePixelRatio());
			}

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glEnable(GL_TEXTURE_2D);
			GLuint textures[2]{};
			glGenTextures(std::size(textures), textures);
			if (!textures[0]) {
				//OutputDebugStringA("glGenTextures failed\n");
				return;
			}
			xFinalAction finalAction([&] {
				glDisable(GL_TEXTURE_2D);
				glDeleteTextures(std::size(textures), textures);
			});

			// Use the shader program
			if (m_gl.shaderProgram) {
				m_gl().glUseProgram(m_gl.shaderProgram);
			}

			PutMatAsTexture(textures[0], img, rcTarget.width, rectTarget, rectClient);

			// Draw Selection Rect
			if (m_mouse.bInSelectionMode or m_mouse.bRectSelected) {
				xBounds2i rect;
				rect.pt0() = m_ctScreenFromImage(m_mouse.ptSel0);
				rect.pt1() = m_ctScreenFromImage(m_mouse.ptSel1);
				rect.Normalize();
				rect &= rectClient;
				if (!rect.IsEmpty()) {
					cv::Mat rectangle(16, 16, CV_8UC4);
					rectangle = cv::Scalar(255, 255, 127, 128);
					PutMatAsTexture(textures[1], rectangle, rectangle.cols, rect, rectClient);
				}
			}
		}

	}

} // namespace biscuit::qt
