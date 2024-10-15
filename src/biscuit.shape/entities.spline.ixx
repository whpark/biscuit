module;

//////////////////////////////////////////////////////////////////////
//
// shape_others.h:
//
// PWH
// 2017.07.20
// 2021.05.27
// 2021.08.11 Compare
// 2023-06-09 분리
// 2024-10-14. biscuit.
//
///////////////////////////////////////////////////////////////////////////////

#include "biscuit/macro.h"
#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_glaze.h"
#include "biscuit/dependencies_units.h"
#include "biscuit/dependencies_cereal.h"

#include "shape_macro.h"

export module biscuit.shape.entities.spline;
import std;
import biscuit;
import biscuit.shape.shape;
import biscuit.shape.canvas;

export namespace biscuit::shape {

	class xSpline : public xShape {
	public:
		enum class eFLAG { closed = 1, periodic = 2, rational = 4, planar = 8, linear = planar|16 };
		int m_flags{};
		point_t m_ptNormal;
		point_t m_vStart, m_vEnd;
		int m_degree{};

		std::vector<double> m_knots;
		std::vector<point_t> m_ptsControl;
		std::vector<point_t> m_ptsFit;

		double m_toleranceKnot{0.0000001};
		double m_toleranceControlPoint{0.0000001};
		double m_toleranceFitPoint{0.0000001};

		BSC__SHAPE_BASE_DEFINITION(xSpline, xShape, eSHAPE::spline);
		BSC__SHAPE_ARCHIVE_MEMBER(xSpline, xShape, 1u, m_flags, m_ptNormal, m_vStart, m_vEnd, m_degree, m_knots, m_ptsControl, m_toleranceKnot, m_toleranceControlPoint, m_toleranceFitPoint);

		//virtual point_t PointAt(double t) const override {};
		virtual std::optional<line_t> GetStartEndPoint() const override {
			if (m_ptsControl.empty())
				return {};
			return line_t{ m_ptsControl.front(), m_ptsControl.back() };
		}
		virtual void FlipX() override { for (auto& pt : m_ptsControl) pt.x = -pt.x; for (auto& pt : m_ptsFit) pt.x = -pt.x; m_ptNormal.x = -m_ptNormal.x; m_vStart.x = -m_vStart.x; m_vEnd.x = -m_vEnd.x; }
		virtual void FlipY() override { for (auto& pt : m_ptsControl) pt.y = -pt.y; for (auto& pt : m_ptsFit) pt.y = -pt.y; m_ptNormal.y = -m_ptNormal.y; m_vStart.y = -m_vStart.y; m_vEnd.y = -m_vEnd.y; }
		virtual void FlipZ() override { for (auto& pt : m_ptsControl) pt.z = -pt.z; for (auto& pt : m_ptsFit) pt.z = -pt.z; m_ptNormal.z = -m_ptNormal.z; m_vStart.z = -m_vStart.z; m_vEnd.z = -m_vEnd.z; }
		virtual void Reverse() override {
			std::swap(m_vStart, m_vEnd);
			std::ranges::reverse(m_knots);
			std::ranges::reverse(m_ptsControl);
			std::ranges::reverse(m_ptsFit);
		}
		virtual void Transform(ct_t const& ct, [[maybe_unused]] bool bRightHanded) override {
			m_ptNormal = ct(m_ptNormal);
			m_vStart = ct(m_vStart);
			m_vEnd = ct(m_vEnd);
			for (auto& pt : m_ptsControl)
				pt = ct(pt);
		}
		virtual bool UpdateBounds(rect_t& rect) const override {
			bool b{};
			for (auto& pt : m_ptsControl)
				b = rect.UpdateBounds(pt);
			return b;
		};
		virtual void Draw(ICanvas& canvas) const override {
			xShape::Draw(canvas);
			canvas.Spline(m_degree, m_ptsControl, m_knots, false);
		}
		//virtual void PrintOut(std::wostream& os) const override {
		//	xShape::PrintOut(os);
		//	fmt::print(os, L"\tflags:{}, degree:{}\n", m_flags, m_degree);
		//	fmt::print(os, L"\tknot ");
		//	for (auto knot : m_knots)
		//		fmt::print(os, L"{}, ", knot);
		//	fmt::print(os, L"\n\tcontrol points ");
		//	for (auto pt : m_ptsControl)
		//		fmt::print(os, L"({},{},{}), ", pt.x, pt.y, pt.z);
		//	fmt::print(os, L"\n");
		//}

		//virtual bool LoadFromCADJson(json_t& _j) override {
		//	xShape::LoadFromCADJson(_j);
		//	using namespace std::literals;
		//	gtl::bjson j(_j);

		//	m_ptNormal = PointFrom(j["normalVec"sv]);
		//	m_vStart = PointFrom(j["tgStart"sv]);
		//	m_vEnd = PointFrom(j["tgEnd"sv]);
		//	m_flags = j["flags"sv];
		//	m_degree = j["degree"sv];

		//	m_toleranceKnot = j["tolknot"sv];
		//	m_toleranceControlPoint = j["tolcontrol"sv];
		//	m_toleranceFitPoint = j["tolfit"sv];

		//	for (auto& v : j["knotslist"sv].json().as_array()) {
		//		m_knots.push_back(v.as_double());
		//	}
		//	for (auto& v : j["controllist"sv].json().as_array()) {
		//		m_ptsControl.push_back(PointFrom(v));
		//	}
		//	for (auto& v : j["fitlist"sv].json().as_array()) {
		//		m_ptsFit.push_back(PointFrom(v));
		//	}

		//	return true;
		//}

	};

}

BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(biscuit::shape::xSpline);

