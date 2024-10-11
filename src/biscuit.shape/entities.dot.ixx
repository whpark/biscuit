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
// 2024-10-10. biscuit.
//
///////////////////////////////////////////////////////////////////////////////

#include "biscuit/macro.h"
#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_glaze.h"
#include "biscuit/dependencies_units.h"
#include "biscuit/dependencies_cereal.h"

export module biscuit.shape.entities.dot;
import std;
import biscuit;
import biscuit.shape.shape;
import biscuit.shape.canvas;

export namespace biscuit::shape {

	class xDot : public xShape {
	public:
		using base_t = xShape;
		using this_t = xDot;

	public:
		constexpr static inline uint32_t s_version{1u};
		point_t m_pt;

	public:
		virtual std::unique_ptr<xShape> clone() const override { return std::unique_ptr<this_t>(new this_t(*this)); }
		auto& base() { return static_cast<base_t&>(*this); }
		auto const& base() const { return static_cast<base_t const&>(*this); }
		auto operator <=> (this_t const&) const = default;

		template < typename archive >
		void serialize(archive& ar, unsigned int const file_version) {
			ar(base(), m_pt);
		}

		virtual bool Compare(xShape const& B_) const override {
			if (!base_t::Compare(B_))
				return false;
			this_t const& B = (this_t const&)B_;
			return (m_pt == B.m_pt);
		}
		virtual eSHAPE GetShapeType() const { return eSHAPE::dot; }

		//virtual point_t PointAt(double t) const override { return pt; };
		virtual std::optional<line_t> GetStartEndPoint() const override {
			return line_t{ m_pt, m_pt };
		}
		virtual void FlipX() override { m_pt.x = -m_pt.x; }
		virtual void FlipY() override { m_pt.y = -m_pt.y; }
		virtual void FlipZ() override { m_pt.z = -m_pt.z; }
		virtual void Reverse() override { }
		virtual void Transform(ct_t const& ct, bool bRightHanded) override {
			m_pt = ct(m_pt);
		};
		virtual bool UpdateBounds(rect_t& bounds) const override {
			return bounds.UpdateBounds(m_pt);
		};
		virtual void Draw(ICanvas& canvas) const override {
			xShape::Draw(canvas);
			canvas.MoveTo(m_pt);
			canvas.LineTo(m_pt);
		}
		//virtual void PrintOut(std::wostream& os) const override {
		//	xShape::PrintOut(os);
		//	fmt::print(os, L"\tpt({},{},{})\n", m_pt.x, m_pt.y, m_pt.z);
		//}

		//virtual bool LoadFromCADJson(json_t& _j) override {
		//	xShape::LoadFromCADJson(_j);
		//	using namespace std::literals;
		//	gtl::bjson j(_j);
		//	m_pt = PointFrom(j["basePoint"sv]);
		//	return true;
		//}

	};

}

export CEREAL_REGISTER_TYPE(biscuit::shape::xDot);
export CEREAL_REGISTER_POLYMORPHIC_RELATION(biscuit::shape::xShape, biscuit::shape::xDot);
export CEREAL_CLASS_VERSION(biscuit::shape::xDot, biscuit::shape::xDot::s_version);

