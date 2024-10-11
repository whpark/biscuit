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
#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_glaze.h"
#include "biscuit/dependencies_units.h"
#include "biscuit/dependencies_cereal.h"

export module biscuit.shape.entities.circle;
import std;
import biscuit;
import biscuit.shape.shape;
import biscuit.shape.canvas;

export namespace biscuit::shape {

	class xCircle : public xShape {
	public:
		using base_t = xShape;
		using this_t = xCircle;

	public:
		constexpr static inline uint32_t s_version{1u};
		point_t m_ptCenter;
		double m_radius{};
		deg_t m_angle_length{360_deg};	// 회전 방향.

	public:
		virtual std::unique_ptr<xShape> clone() const override { return std::unique_ptr<this_t>(new this_t(*this)); }
		auto& base() { return static_cast<base_t&>(*this); }
		auto const& base() const { return static_cast<base_t const&>(*this); }
		auto operator <=> (this_t const&) const = default;
		template < typename archive >
		void serialize(archive& ar, unsigned int const file_version) {
			ar(base(), m_ptCenter, m_radius, m_angle_length);
		}
		virtual bool Compare(xShape const& B_) const override {
			if (!base_t::Compare(B_))
				return false;
			this_t const& B = (this_t const&)B_;
			return *this == B;
		}
		virtual eSHAPE GetShapeType() const { return eSHAPE::circle_xy; }

		//virtual point_t PointAt(double t) const override {};
		virtual std::optional<line_t> GetStartEndPoint() const override {
			auto pt = m_ptCenter+point_t{m_radius,0,0};
			return line_t{ pt, pt };
		}
		virtual void FlipX() override { m_ptCenter.x = -m_ptCenter.x; m_angle_length = -m_angle_length; }
		virtual void FlipY() override { m_ptCenter.y = -m_ptCenter.y; m_angle_length = -m_angle_length; }
		virtual void FlipZ() override { m_ptCenter.z = -m_ptCenter.z; m_angle_length = -m_angle_length; }
		virtual void Reverse() override {
			m_angle_length = -m_angle_length;
		}
		virtual void Transform(ct_t const& ct, bool bRightHanded) override {
			m_ptCenter = ct(m_ptCenter); m_radius = ct.Trans(m_radius);
			if (!bRightHanded)
				m_angle_length = -m_angle_length;
		}
		virtual bool UpdateBounds(rect_t& rectBoundary) const override {
			bool bResult{};
			bResult |= rectBoundary.UpdateBounds(point_t(m_ptCenter.x-m_radius, m_ptCenter.y-m_radius, m_ptCenter.z));
			bResult |= rectBoundary.UpdateBounds(point_t(m_ptCenter.x+m_radius, m_ptCenter.y+m_radius, m_ptCenter.z));
			return bResult;
		};
		virtual void Draw(ICanvas& canvas) const override {
			xShape::Draw(canvas);
			canvas.Arc(m_ptCenter, m_radius, 0._deg, m_angle_length);
		}
		//virtual void PrintOut(std::wostream& os) const override {
		//	xShape::PrintOut(os);
		//	fmt::print(os, L"\tcenter({},{},{}), r {}\n", m_ptCenter.x, m_ptCenter.y, m_ptCenter.z, m_radius);
		//}

		//virtual bool LoadFromCADJson(json_t& _j) override {
		//	xShape::LoadFromCADJson(_j);
		//	using namespace std::literals;
		//	gtl::bjson j(_j);

		//	m_ptCenter = PointFrom(j["basePoint"sv]);
		//	m_radius = j["radious"sv];
		//	return true;
		//}
	};

}

export CEREAL_REGISTER_TYPE(biscuit::shape::xCircle);
export CEREAL_REGISTER_POLYMORPHIC_RELATION(biscuit::shape::xShape, biscuit::shape::xCircle);
export CEREAL_CLASS_VERSION(biscuit::shape::xCircle, biscuit::shape::xCircle::s_version);

