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
#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_glaze.h"
#include "biscuit/dependencies_units.h"
#include "biscuit/dependencies_cereal.h"

export module biscuit.shape.entities.ellipse;
import std;
import biscuit;
import biscuit.shape.shape;
import biscuit.shape.canvas;
import biscuit.shape.entities.arc;

export namespace biscuit::shape {

	class xEllipse : public xArc {
	public:
		using base_t = xArc;
		using this_t = xEllipse;

	public:
		constexpr static inline uint32_t s_version{1u};
		double m_radiusH{};
		deg_t m_angle_first_axis{};

		virtual bool Compare(xShape const& B_) const override {
			if (!base_t::Compare(B_))
				return false;
			this_t const& B = (this_t const&)B_;
			return (m_radiusH == B.m_radiusH) and (m_angle_first_axis == B.m_angle_first_axis);
		}
		virtual eSHAPE GetShapeType() const { return eSHAPE::ellipse_xy; }

		//virtual point_t PointAt(double t) const override {};
		virtual std::optional<std::pair<point_t, point_t>> GetStartEndPoint() const override {
			xCoordTrans2d ct;
			ct.Init(1.0, (rad_t)m_angle_first_axis, point_t{}, m_ptCenter);

			deg_t t0 = m_angle_start;
			deg_t t1 = m_angle_start + m_angle_length;
			auto radius1 = m_radius;
			auto radius2 = m_radiusH;
			point_t pt0 = ct(point_t{radius1*units::math::cos(t0), radius2*units::math::sin(t0)});
			point_t pt1 = ct(point_t{radius1*units::math::cos(t1), radius2*units::math::sin(t1)});

			return std::pair{ pt0, pt1 };
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
		virtual bool UpdateBounds(rect_t& rectBoundary) const override {
			// todo : ... upgrade
			bool bResult{};
			bResult |= rectBoundary.UpdateBounds(point_t(m_ptCenter.x-m_radius, m_ptCenter.y-m_radiusH, m_ptCenter.z));
			bResult |= rectBoundary.UpdateBounds(point_t(m_ptCenter.x+m_radius, m_ptCenter.y+m_radiusH, m_ptCenter.z));
			return bResult;
		}
		virtual void Draw(ICanvas& canvas) const override {
			xShape::Draw(canvas);
			canvas.Ellipse(m_ptCenter, m_radius, m_radiusH, m_angle_first_axis, m_angle_start, m_angle_length);
		}
		virtual void PrintOut(std::wostream& os) const override {
			xArc::PrintOut(os);
			fmt::print(os, L"\tradiusH:{}, angle_first_axis:{} deg\n", m_radiusH, (double)(deg_t)m_angle_first_axis);
		}

		GTL__DYNAMIC_VIRTUAL_DERIVED(xEllipse);
		//GTL__REFLECTION_VIRTUAL_DERIVED(xEllipse, xArc);
		//GTL__REFLECTION_MEMBERS(m_radiusH, m_angle_first_axis);
		auto operator <=> (xEllipse const&) const = default;

		template < typename archive >
		friend void serialize(archive& ar, xEllipse& var, unsigned int const file_version) {
			boost::serialization::base_object<xArc>(var);
			ar & var;
		}
		template < typename archive >
		friend archive& operator & (archive& ar, xEllipse& var) {
			ar & boost::serialization::base_object<xArc>(var);
			return ar & var.m_radiusH & (double&)var.m_angle_first_axis;
		}

		virtual bool LoadFromCADJson(json_t& _j) override {
			xShape::LoadFromCADJson(_j);
			using namespace std::literals;
			gtl::bjson j(_j);

			m_ptCenter = PointFrom(j["basePoint"sv]);
			point_t firstAxis = PointFrom(j["secPoint"sv]);
			m_angle_first_axis = deg_t::atan2(firstAxis.y, firstAxis.x);
			double ratio = j["ratio"sv];
			m_radius = firstAxis.GetLength();
			m_radiusH = ratio * m_radius;

			rad_t t0 {(double)j["staparam"sv]};
			rad_t t1 {(double)j["endparam"sv]};
			bool bCCW = j["isccw"sv];

			m_angle_start = t0;
			m_angle_length = t1-t0;

			if (bCCW) {
				if (m_angle_length < 0_deg)
					m_angle_length += 360_deg;
			} else {
				if (m_angle_length > 0_deg)
					m_angle_length -= 360_deg;
			}

			return true;
		}

	};

}

export CEREAL_REGISTER_TYPE(biscuit::shape::xEllipse);
export CEREAL_REGISTER_POLYMORPHIC_RELATION(biscuit::shape::xShape, biscuit::shape::xEllipse);
export CEREAL_CLASS_VERSION(biscuit::shape::xEllipse, biscuit::shape::xEllipse::s_version);
