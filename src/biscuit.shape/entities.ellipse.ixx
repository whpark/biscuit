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
// 2024-10-14. biscuit
//
///////////////////////////////////////////////////////////////////////////////

#include "biscuit/macro.h"
#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_glaze.h"
#include "biscuit/dependencies_units.h"
#include "biscuit/dependencies_cereal.h"

#include "shape_macro.h"

export module biscuit.shape.entities.ellipse;
import std;
import biscuit;
import biscuit.shape.shape;
import biscuit.shape.canvas;
import biscuit.shape.entities.circle;
import biscuit.shape.entities.arc;

#if 0
export namespace biscuit::shape {

	class xEllipse : public xArc {
	public:
		double m_radiusH{};
		deg_t m_angle_first_axis{};

		BSC__SHAPE_BASE(xEllipse, xArc, eSHAPE::ellipse_xy, 1u, m_radiusH, m_angle_first_axis);

		virtual std::optional<line_t> GetStartEndPoint() const override {
			xCoordTrans2d ct;
			ct.SetFromAngle2d(m_angle_first_axis, point_t{}, m_ptCenter);

			deg_t t0 = m_angle_start;
			deg_t t1 = m_angle_start + m_angle_length;
			auto radius1 = m_radius;
			auto radius2 = m_radiusH;
			point_t pt0 = ct(point_t{radius1*units::math::cos(t0), radius2*units::math::sin(t0)});
			point_t pt1 = ct(point_t{radius1*units::math::cos(t1), radius2*units::math::sin(t1)});

			return line_t{ pt0, pt1 };
		}
		virtual void FlipX() override { xArc::FlipX(); m_angle_first_axis = 180._deg - m_angle_first_axis; }
		virtual void FlipY() override { xArc::FlipY(); m_angle_first_axis = - m_angle_first_axis; }
		virtual void FlipZ() override { xArc::FlipZ(); m_angle_first_axis = 180._deg - m_angle_first_axis; }
		virtual void Transform(ct_t const& ct, bool bRightHanded) override {
			xArc::Transform(ct, bRightHanded);
			m_radiusH = ct.Trans(m_radiusH);
			if (!bRightHanded)
				m_angle_first_axis = -m_angle_first_axis;
		}
		virtual bool UpdateBounds(rect_t& bounds) const override {
			// todo : ... upgrade
			bool bResult{};
			bResult |= bounds.UpdateBounds(point_t(m_ptCenter.x-m_radius, m_ptCenter.y-m_radiusH, m_ptCenter.z));
			bResult |= bounds.UpdateBounds(point_t(m_ptCenter.x+m_radius, m_ptCenter.y+m_radiusH, m_ptCenter.z));
			return bResult;
		}
		virtual void Draw(ICanvas& canvas) const override {
			xShape::Draw(canvas);
			canvas.Ellipse(m_ptCenter, m_radius, m_radiusH, m_angle_first_axis, m_angle_start, m_angle_length);
		}
		//virtual void PrintOut(std::wostream& os) const override {
		//	xArc::PrintOut(os);
		//	fmt::print(os, L"\tradiusH:{}, angle_first_axis:{} deg\n", m_radiusH, (double)(deg_t)m_angle_first_axis);
		//}

		//virtual bool LoadFromCADJson(json_t& _j) override {
		//	xShape::LoadFromCADJson(_j);
		//	using namespace std::literals;
		//	gtl::bjson j(_j);

		//	m_ptCenter = PointFrom(j["basePoint"sv]);
		//	point_t firstAxis = PointFrom(j["secPoint"sv]);
		//	m_angle_first_axis = deg_t::atan2(firstAxis.y, firstAxis.x);
		//	double ratio = j["ratio"sv];
		//	m_radius = firstAxis.GetLength();
		//	m_radiusH = ratio * m_radius;

		//	rad_t t0 {(double)j["staparam"sv]};
		//	rad_t t1 {(double)j["endparam"sv]};
		//	bool bCCW = j["isccw"sv];

		//	m_angle_start = t0;
		//	m_angle_length = t1-t0;

		//	if (bCCW) {
		//		if (m_angle_length < 0_deg)
		//			m_angle_length += 360_deg;
		//	} else {
		//		if (m_angle_length > 0_deg)
		//			m_angle_length -= 360_deg;
		//	}

		//	return true;
		//}

	};

}

BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(biscuit::shape::xEllipse);
#endif

