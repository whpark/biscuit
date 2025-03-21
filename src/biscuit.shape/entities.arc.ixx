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

export module biscuit.shape.entities.arc;
import std;
import fmt;
import units;
import biscuit;
import biscuit.shape_basic;
import biscuit.shape.entities.shape;
import biscuit.shape.entities.circle;
import biscuit.shape.canvas;

using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::shape {

	class xArc : public xCircle {
	public:
		deg_t m_angle_start{};

	public:
		BSC__SHAPE_BASE_DEFINITION(xArc, xCircle, eSHAPE::arc_xy);
		BSC__SHAPE_ARCHIVE_MEMBER(xArc, xCircle, 1u, m_angle_start);

		//virtual point_t PointAt(double t) const override {};
		virtual std::optional<line_t> GetStartEndPoint() const override {
			auto c = units::math::cos(m_angle_start);
			auto s = units::math::sin(m_angle_start);
			auto pt0 = m_ptCenter + m_radius*point_t{c, s};
			c = units::math::cos(m_angle_start + m_angle_length);
			s = units::math::sin(m_angle_start + m_angle_length);
			auto pt1 = m_ptCenter + m_radius*point_t{c, s};
			return line_t{ pt0, pt1 };
		}
		virtual void FlipX() override { base_t::FlipX(); m_angle_start = AdjustAngle(180._deg - m_angle_start); }
		virtual void FlipY() override { base_t::FlipY(); m_angle_start = AdjustAngle(-m_angle_start); }
		virtual void FlipZ() override { base_t::FlipZ(); m_angle_start = AdjustAngle(180._deg - m_angle_start); }	// ????.....  성립 안되지만,
		virtual void Reverse() override {
			m_angle_start = m_angle_start+m_angle_length;
			m_angle_length = -m_angle_length;
		}
		virtual void Transform(ct_t const& ct, bool bRightHanded) override {
			// todo : ... upgrade?
			base_t::Transform(ct, bRightHanded);
			if (!bRightHanded)
				m_angle_start = -m_angle_start;
		}
		point_t At(rad_t t) const { return m_ptCenter + m_radius * point_t{units::math::cos(t), units::math::sin(t)}; };
		virtual bool UpdateBounds(rect_t& bounds) const override {
			bool bResult{};
			// todo : ... upgrade?
			rect_t rectMax(m_ptCenter, m_ptCenter);
			rectMax.pt0() -= point_t(m_radius, m_radius);
			rectMax.pt1() += point_t(m_radius, m_radius);
			if (bounds.Contains(rectMax))
				return bResult;
			auto start = m_angle_start;
			if (start < 0_deg)
				start += 360_deg;
			auto end = start + m_angle_length;
			bResult |= bounds.UpdateBounds(At(start));
			bResult |= bounds.UpdateBounds(At(end));
			if (start > end)
				std::swap(start, end);
			int count{};
			int iend = (int)end.value();
			for (int t = (int)std::floor((start/90.0_deg).value())*90+90; t <= iend; t += 90) {
				switch (t%360) {
				case 0 :		bResult |= bounds.UpdateBounds(m_ptCenter + point_t(m_radius, 0.)); break;
				case 90 :		bResult |= bounds.UpdateBounds(m_ptCenter + point_t(0., m_radius)); break;
				case 180 :		bResult |= bounds.UpdateBounds(m_ptCenter + point_t(-m_radius, 0.)); break;
				case 270 :		bResult |= bounds.UpdateBounds(m_ptCenter + point_t(0., -m_radius)); break;
				}
				if (count++ >=4)
					break;
			}
			return bResult;
		}
		virtual void Draw(ICanvas& canvas) const override {
			xShape::Draw(canvas);
			canvas.Arc(m_ptCenter, m_radius, m_angle_start, m_angle_length);
		}

		deg_t AdjustAngle(deg_t angle) const {
			angle = units::math::fmod(angle, 360._deg);
			if (angle < 0._deg)
				angle += 360._deg;
			return angle;
		}

		static xArc GetFromBulge(double bulge, point_t const& pt0, point_t const& pt1) {
			xArc arc;
			xPoint2d vecPerpendicular(-(pt0.y-pt1.y), (pt0.x-pt1.x));
			// Normalize
			{
				double d = vecPerpendicular.GetDistance();
				vecPerpendicular.x /= d;
				vecPerpendicular.y /= d;
			}
			xPoint2d ptCenterOfLine((pt0.x+pt1.x)/2., (pt0.y+pt1.y)/2.);
			double l = pt1.GetDistance(pt0)/2.;
			xPoint2d ptBulge;
			ptBulge.x = ptCenterOfLine.x + vecPerpendicular.x * (bulge * l);
			ptBulge.y = ptCenterOfLine.y + vecPerpendicular.y * (bulge * l);
			double h = ptBulge.GetDistance(ptCenterOfLine);
			arc.m_radius = (Square(l) + Square(h)) / (2 * h);

			arc.m_ptCenter.x = ptBulge.x + (arc.m_radius / h) * (ptCenterOfLine.x - ptBulge.x);
			arc.m_ptCenter.y = ptBulge.y + (arc.m_radius / h) * (ptCenterOfLine.y - ptBulge.y);
			arc.m_angle_start = rad_t(std::atan2(pt0.y - arc.m_ptCenter.y, pt0.x - arc.m_ptCenter.x));
			rad_t dT1 = rad_t(std::atan2(pt1.y - arc.m_ptCenter.y, pt1.x - arc.m_ptCenter.x));
			//arc.m_eDirection = (dBulge > 0) ? 1 : -1;
			//arc.m_dTLength = (dBulge > 0) ? fabs(dT1-arc.m_dT0) : -fabs(dT1-arc.m_dT0);
			if (bulge > 0) {
				while (dT1 < arc.m_angle_start)
					dT1 += rad_t(std::numbers::pi*2);
				arc.m_angle_length = dT1 - arc.m_angle_start;
			} else {
				while (dT1 > arc.m_angle_start)
					dT1 -= rad_t(std::numbers::pi*2);
				arc.m_angle_length = dT1 - arc.m_angle_start;
			}

			return arc;
		}

	};
}

BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(biscuit::shape::xArc);
