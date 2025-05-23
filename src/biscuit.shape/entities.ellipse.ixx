﻿module;

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
#include "biscuit/dependencies/glaze.h"

#include "shape_macro.h"

export module biscuit.shape.entities.ellipse;
import std;
import "biscuit/dependencies/fmt.hxx";
import "biscuit/dependencies/units.hxx";
import "biscuit/dependencies/cereal.hxx";
import "biscuit/dependencies/eigen.hxx";
import biscuit;
import biscuit.shape_basic;
import biscuit.shape.entities.shape;
import biscuit.shape.entities.circle;
import biscuit.shape.entities.arc;
import biscuit.shape.canvas;

using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::shape {

	class xEllipse : public xArc {
	public:
		point_t m_ptCenter;
		double m_radius{};
		deg_t m_angle_length{360_deg};	// 회전 방향.
		deg_t m_angle_start{};
		double m_radiusH{};
		deg_t m_angle_first_axis{};

		BSC__SHAPE_BASE_DEFINITION(xEllipse, xArc, eSHAPE::ellipse_xy);
		BSC__SHAPE_ARCHIVE_MEMBER(xEllipse, xArc, 1u, m_ptCenter, m_radius, m_angle_length, m_angle_start, m_radiusH, m_angle_first_axis);

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
		virtual void FlipX() override { base_t::FlipX(); m_angle_first_axis = 180._deg - m_angle_first_axis; }
		virtual void FlipY() override { base_t::FlipY(); m_angle_first_axis = - m_angle_first_axis; }
		virtual void FlipZ() override { base_t::FlipZ(); m_angle_first_axis = 180._deg - m_angle_first_axis; }
		virtual void Transform(ct_t const& ct, bool bRightHanded) override {
			base_t::Transform(ct, bRightHanded);
			m_radiusH *= ct.Scale2d();
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
	};

}

BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(biscuit::shape::xEllipse);

