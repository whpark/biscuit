﻿module;

//////////////////////////////////////////////////////////////////////
//
// canvas.h:
//
// PWH
// 2017.07.20
// 2021.05.28 from QCanvas -> canvas
// 2024-10-10. biscuit
//
///////////////////////////////////////////////////////////////////////////////

#include "biscuit/macro.h"
#include "biscuit/dependencies/glaze.h"

#include "tinysplinecxx.h"

export module biscuit.shape.canvas;
import std;
import "biscuit/dependencies/fmt.hxx";
import "biscuit/dependencies/units.hxx";
import "biscuit/dependencies/cereal.hxx";
import "biscuit/dependencies/opencv.hxx";
import "biscuit/dependencies/eigen.hxx";
import biscuit;
import biscuit.shape_basic;
import biscuit.shape.entities.shape;
//import biscuit.shape.entities;

export namespace biscuit::shape {
//#pragma warning(push)
//#pragma warning(disable: 4275)

	//class xShape;
	class xText;
	class xMText;

	class ICanvas;
	void Canvas_Spline(ICanvas& canvas, int degree, std::span<point_t const> pts, std::span<double const> knots, bool bLoop);

	//=============================================================================================================================
	// ICanvas : Interface of Canvas
	class ICanvas {
	public:
		xCoordTransChain m_ct, m_ctI;
		point_t m_ptLast{};

	public:
		/// @brief for laser on/off
		double m_min_jump_length = 0.0;

		/// @brief target resolution
		double m_target_interpolation_inverval{1.0};

	public:
		ICanvas() = default;
		ICanvas(ICanvas const& B) {
			*this = B;
		}
		ICanvas(ICanvas &&) = default;
		ICanvas(ICoordTrans const& ct) {
			SetCT(ct);
		}
		virtual ~ICanvas() {}
		ICanvas& operator = (ICanvas const& B) = default;
		ICanvas& operator = (ICanvas &&) = default;

		virtual void Init() {
			m_ptLast = point_t::All(std::nan(""));
			m_min_jump_length = 0.0;
			m_target_interpolation_inverval = 1.0;
		}

		void SetCT(ICoordTrans const& ct) {
			if (auto* pCT = dynamic_cast<xCoordTransChain const*>(&ct); pCT) {
				m_ct = *pCT;
				if (auto ctI = m_ct.GetInverse())
					m_ctI = std::move(*(xCoordTransChain*)ctI.release());
				else
					m_ctI.Init();
			} else {
				m_ct.Init();
				m_ct *= ct;
				m_ctI.Init();
				if (auto ctI = ct.GetInverse())
					m_ctI *= *ctI;
			}
		}

		// Scratching
		point_t Trans(point_t const& pt) const { return m_ct(pt); }
		point_t TransI(point_t const& pt) const { return m_ctI(pt); }

		virtual void MoveTo_Target(point_t const& ptTargetSystem) = 0;
		virtual void LineTo_Target(point_t const& ptTargetSystem) = 0;

		virtual void PreDraw(xShape const&) = 0;
		//virtual void Draw(xShape const&);

		void MoveTo(point_t pt) {
			pt = Trans(pt);
			if ( pt.IsAllValid() and (m_min_jump_length != 0.0) and (m_ptLast.GetDistance(pt) < m_min_jump_length) )
				return;
			MoveTo_Target(pt);
			m_ptLast = pt;
		}
		void LineTo(point_t pt) {
			pt = Trans(pt);
			LineTo_Target(pt);
			m_ptLast = pt;
		}

		virtual void Dot(point_t const& pt) {
			MoveTo(pt);
			LineTo(pt);
		}
		virtual void Line(point_t const& pt0, point_t const& pt1) {
			MoveTo(pt0);
			LineTo(pt1);
		}
		virtual void Arc(point_t const& ptCenter, double radius, rad_t t0, rad_t tLength) {
			int n = Round(std::abs(rad_t(tLength).value() * radius / m_target_interpolation_inverval));
			rad_t t1 = t0+tLength;
			MoveTo(radius * point_t{units::math::cos(t0), units::math::sin(t0), .0}+ptCenter);
			for (int i = 1; i <= n; i++) {
				rad_t t { t0 + (t1-t0)*(double)i/n };
				constexpr static auto m2pi = std::numbers::pi*2;
				double c = radius * units::math::cos(t);
				double s = radius * units::math::sin(t);
				xPoint2d pt(ptCenter.x + c, ptCenter.y+s);
				LineTo(pt);
			}
		}
		virtual void Ellipse(point_t const& ptCenter, double radius1, double radius2, rad_t tFirstAxis, rad_t t0, rad_t tLength) {
			xCoordTrans2d ct;
			//ct.Init(1.0, (rad_t)tFirstAxis, point_t{}, ptCenter);
			ct.SetFromAngle2d(tFirstAxis, point_t{}, ptCenter);

			rad_t t1 = t0 + tLength;
			MoveTo(ct(point_t{radius1*units::math::cos(t0), radius2*units::math::sin(t0)}));
			int n = Round(rad_t(tLength).value() * std::max(radius1, radius2) / m_target_interpolation_inverval);
			for (int i = 0; i <= n; i++) {
				rad_t t { t0 + (t0-t1) * (double)i / n };
				constexpr static auto m2pi = std::numbers::pi*2;
				double c = units::math::cos(t);
				double s = units::math::sin(t);
				LineTo(ct(point_t{radius1*c, radius2*s}));
			}
		}
		virtual void Spline(int degree, std::span<point_t const> pts, std::span<double const> knots, bool bLoop) {
			Canvas_Spline(*this, degree, pts, knots, bLoop);
		}
		virtual void Text(xText const& /*text*/) {
		}
		virtual void Text(xMText const& /*text*/) {
		}

		virtual std::optional<xBounds2d> GetClippingRect() {
			return std::nullopt;
		}

	//public:
	//	virtual rad_t CalcArcInterval(double radius, double target_resolution) {
	//		auto scale = m_ct(point_t::All(0)).Distance(m_ct(point_t::All(1.)));
	//		return rad_t(target_resolution / (radius*scale*std::numbers::pi));
	//	}
	};

	//=============================================================================================================================
	// IQDeviceScanner : Interface of Canvas
	class ICanvasScanner : public ICanvas {
		friend class ICanvas;

	public:
		ICanvasScanner() {
		}
		virtual ~ICanvasScanner() {
		}

	public:
		//virtual void PreDraw(xShape const& shape) {}

		virtual void PreDraw(xShape const&) override {}

		virtual void MoveTo_Target(point_t const& ) override {
			// Scanner Jump To
		}
		virtual void LineTo_Target(point_t const& ) override {
			// Scanner Write To
		}
	protected:
	};


	//=============================================================================================================================
	// ICanvas : Interface of Canvas
	class xCanvasMat : public ICanvas {
	public:
		cv::Mat& m_img;

		cv::Scalar m_color{};
		double m_line_width {1.};

		// line width
		int m_line_type = cv::LINE_8;

	public:
		xCanvasMat(cv::Mat& img, ICoordTrans const& ct) : m_img(img), ICanvas(ct) { }
		virtual ~xCanvasMat() { }

		virtual void PreDraw(xShape const& shape) override {
			m_color = ColorS(shape.m_color);
		}

		// Primative, returns End Position.
		virtual void MoveTo_Target(point_t const& ) override {}
		virtual void LineTo_Target(point_t const& pt) override {
			cv::line(m_img, m_ptLast, pt, m_color, (int)m_line_width, m_line_type);
			//cv::line(m_img, cv::Point(m_ptLast.x, m_ptLast.y), cv::Point(pt.x, pt.y), m_color, (int)m_line_width, m_line_type);
			//cv::line(m_img, cv::Point2d(m_ptLast.x, m_ptLast.y), cv::Point2d(pt.x, pt.y), m_color, (int)m_line_width, m_line_type);
		}

		//inline auto ColorS() { return ColorS(cr_); }
		inline cv::Scalar ColorS(color_t cr) { return cv::Scalar(cr.b, cr.g, cr.r); }

		//// additional...
		//virtual void MoveRelTo(const point_t& pt) { MoveTo(m_ptLast+pt); }
		//virtual void LineRelTo(const point_t& pt, bool bShowDirection = false) { LineTo(m_ptLast + pt, bShowDirection); }
		//virtual void ArcRelTo(const point_t& ptCenter, deg_t dTLength) { ArcTo(m_ptLast + ptCenter, dTLength); }
	
		std::optional<xBounds2d> GetClippingRect() override {
			xBounds2d rc;
			rc.pt0() = m_ctI(xPoint2d(0.0, 0.0));
			rc.pt1() = m_ctI(xPoint2d(m_img.cols, m_img.rows));
			rc.Normalize();
			return rc;
		}
	};


	//=============================================================================================================================
	// ICanvas : Interface of Canvas
	template < bool round_down_line = true, bool round_down_arc = true >
	class xCanvasMat_RoundDown : public xCanvasMat {
	public:
		using base_t = xCanvasMat;
	public:
		using base_t::base_t;

		// Primative, returns End Position.
		virtual void LineTo_Target(point_t const& pt) override {
			if constexpr (round_down_line) {
				cv::line(m_img, cv::Point((int)m_ptLast.x, (int)m_ptLast.y), cv::Point((int)pt.x, (int)pt.y), m_color, (int)m_line_width, m_line_type);
			} else {
				xCanvasMat::LineTo_Target(pt);
			}
		}

		virtual void Arc(point_t const& ptCenter, double radius, deg_t t0, deg_t tLength) override {
			if constexpr (round_down_arc) {
				point_t ptC = Trans(ptCenter);
				xPoint3i ptCi((int)ptC.x, (int)ptC.y, (int)ptC.z);	// RoundDrop
				ptC = TransI(ptCi);
				ICanvas::Arc(ptC, radius, t0, tLength);
			} else {
				ICanvas::Arc(ptCenter, radius, t0, tLength);
			}
		}
	};

}	// namespace biscuit::shape

namespace biscuit::shape {
	void Canvas_Spline(ICanvas& canvas, int degree, std::span<point_t const> pts, std::span<double const> knots, bool bLoop) {
		if (pts.size() < 2)
			return;

		if (auto rectClipping = canvas.GetClippingRect()) {
			// todo: has bugs.
			auto rc = *rectClipping;
			rc.Inflate(rc.Width(), rc.Height());
			bool bIn{false};
			for (auto const& pt : pts) {
				if (rc.Contains(xPoint2d(pt))) {
					bIn = true;
					break;
				}
			}
			if (!bIn)
				return;
		}

		double len{};
		for (size_t i{1}; i < pts.size(); i++) {
			len += pts[i].GetDistance(pts[i-1]);
		}

		double scale = canvas.m_ct.Scale2d();
		double step = canvas.m_target_interpolation_inverval / len / scale;
		//double step = 0.001;

		constexpr static int const dim = 2;
		tinyspline::BSpline bspline(pts.size(), dim, degree);
		std::vector<tinyspline::real> points((pts.size()+bLoop)*dim);
		auto* pos = points.data();
		if constexpr (dim == 2) {
			for (size_t i{}; i < pts.size(); i++) {
				*pos++ += pts[i].x;
				*pos++ += pts[i].y;
			}
			//if (bLoop) {
			//	*pos++ += pts[0].x;
			//	*pos++ += pts[0].y;
			//}
		} else if constexpr (dim == 3) {
			for (size_t i{}; i < pts.size(); i++) {
				*pos++ += pts[i].x;
				*pos++ += pts[i].y;
				*pos++ += pts[i].z;
			}
			//if (bLoop) {
			//	*pos++ += pts[0].x;
			//	*pos++ += pts[0].y;
			//	*pos++ += pts[0].z;
			//}
		} else {
			// todo : error.
			return;
		}
		bspline.setControlPoints(points);
		//std::vector<double> knotsv{knots.begin(), knots.end()};
		//bspline.setKnots(knotsv);

		//auto cr = dynamic_cast<xCanvasMat&>(canvas).m_color;
		//dynamic_cast<xCanvasMat&>(canvas).m_color = cv::Scalar(0, 0, 255);
		//canvas.MoveTo(pts.front());
		//for (auto const& pt : pts) {
		//	canvas.LineTo(pt);
		//}
		//dynamic_cast<xCanvasMat&>(canvas).m_color = cr;

		auto ResultToPoint = [](auto const& result) {
			return point_t{ result[0], result[1], result.size() >= 3 ? result[2] : 0.0 };
		};
		auto pt0 = bspline.eval(0.0).result();
		point_t pt = ResultToPoint(pt0);
		canvas.MoveTo(pt);
		for (double t{ step }; t < 1.0; t += step) {
			auto pt1 = ResultToPoint(bspline.eval(t).result());
			canvas.LineTo(pt1);
		}
		auto ptL = ResultToPoint(bspline.eval(1.0).result());
		canvas.LineTo(ptL);

	}

}

