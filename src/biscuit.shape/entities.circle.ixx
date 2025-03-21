module;

//////////////////////////////////////////////////////////////////////
//
// entities.circle.ixx:
//
// PWH
// 2017.07.20
// 2021.05.27
// 2021.08.11 Compare
// 2023-06-09 분리
// 2024-10-10. biscuit
//
///////////////////////////////////////////////////////////////////////////////

#include "biscuit/macro.h"
//#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_glaze.h"
#include "biscuit/dependencies_units.h"
#include "biscuit/dependencies_cereal.h"

#include "shape_macro.h"

export module biscuit.shape.entities.circle;
import std;
import fmt;
import units;
import biscuit;
import biscuit.shape_basic;
import biscuit.shape.entities.shape;
import biscuit.shape.canvas;

using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::shape {

	class xCircle : public xShape {
	public:
		point_t m_ptCenter;
		double m_radius{};
		deg_t m_angle_length{360_deg};	// 회전 방향.

	public:
		BSC__SHAPE_BASE_DEFINITION(xCircle, xShape, eSHAPE::circle_xy);
		BSC__SHAPE_ARCHIVE_MEMBER(xCircle, xShape, 1u, m_ptCenter, m_radius, m_angle_length);

		virtual std::optional<line_t> GetStartEndPoint() const override {
			auto pt = m_ptCenter + point_t(m_radius,0,0);
			return line_t{ pt, pt };
		}
		virtual void FlipX() override { m_ptCenter.x = -m_ptCenter.x; m_angle_length = -m_angle_length; }
		virtual void FlipY() override { m_ptCenter.y = -m_ptCenter.y; m_angle_length = -m_angle_length; }
		virtual void FlipZ() override { m_ptCenter.z = -m_ptCenter.z; m_angle_length = -m_angle_length; }
		virtual void Reverse() override {
			m_angle_length = -m_angle_length;
		}
		virtual void Transform(ct_t const& ct, bool bRightHanded) override {
			m_ptCenter = ct(m_ptCenter); m_radius *= ct.Scale2d();
			if (!bRightHanded)
				m_angle_length = -m_angle_length;
		}
		virtual bool UpdateBounds(rect_t& bounds) const override {
			bool bResult = false;
			bResult |= bounds.UpdateBounds(point_t(m_ptCenter.x-m_radius, m_ptCenter.y-m_radius, m_ptCenter.z));
			bResult |= bounds.UpdateBounds(point_t(m_ptCenter.x+m_radius, m_ptCenter.y+m_radius, m_ptCenter.z));
			return bResult;
		};
		virtual void Draw(ICanvas& canvas) const override {
			xShape::Draw(canvas);
			canvas.Arc(m_ptCenter, m_radius, 0._deg, m_angle_length);
		}
	};
}	// namespace biscuit::shape

BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(biscuit::shape::xCircle);

