//////////////////////////////////////////////////////////////////////
//
// shape_others.h:
//
// PWH
// 2017.07.20
// 2021.05.27
// 2021.08.11 Compare
// 2023-06-09 분리
//
///////////////////////////////////////////////////////////////////////////////

#include "biscuit/macro.h"
#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_glaze.h"
#include "biscuit/dependencies_units.h"
#include "biscuit/dependencies_cereal.h"

export module biscuit.shape.entities.line;
import std;
import biscuit;
import biscuit.shape.shape;
import biscuit.shape.canvas;

export namespace biscuit::shape {

	class xLine : public xShape {
	public:
		using base_t = xShape;
		using this_t = xLine;

	public:
		constexpr static inline uint32_t s_version{1u};
		point_t m_pt0, m_pt1;

		virtual bool Compare(xShape const& B_) const override {
			if (!base_t::Compare(B_))
				return false;
			this_t const& B = (this_t const&)B_;
			return (m_pt0 == B.m_pt0) and (m_pt1 == B.m_pt1);
		}
		virtual eSHAPE GetShapeType() const { return eSHAPE::line; }

		//virtual point_t PointAt(double t) const override { return lerp(m_pt0, m_pt1, t); }
		virtual std::optional<std::pair<point_t, point_t>> GetStartEndPoint() const override {
			return std::pair{ m_pt0, m_pt1 };
		}
		virtual void FlipX() override { m_pt0.x = -m_pt0.x; m_pt1.x = -m_pt1.x; }
		virtual void FlipY() override { m_pt0.y = -m_pt0.y; m_pt1.y = -m_pt1.y; }
		virtual void FlipZ() override { m_pt0.z = -m_pt0.z; m_pt1.z = -m_pt1.z; }
		virtual void Reverse() override {
			std::swap(m_pt0, m_pt1);
		}
		virtual void Transform(ct_t const& ct, bool bRightHanded) override {
			m_pt0 = ct(m_pt0); m_pt1 = ct(m_pt1);
		};
		virtual bool UpdateBounds(rect_t& rectBoundary) const override {
			bool bModified{};
			bModified |= rectBoundary.UpdateBounds(m_pt0);
			bModified |= rectBoundary.UpdateBounds(m_pt1);
			return bModified;
		};
		virtual void Draw(ICanvas& canvas) const override {
			xShape::Draw(canvas);
			canvas.Line(m_pt0, m_pt1);
		}
		virtual void PrintOut(std::wostream& os) const override {
			xShape::PrintOut(os);
			fmt::print(os, L"\tpt0({},{},{}) - pt1({},{},{})\n", m_pt0.x, m_pt0.y, m_pt0.z, m_pt1.x, m_pt1.y, m_pt1.z);
		}

		GTL__DYNAMIC_VIRTUAL_DERIVED(xLine);
		auto operator <=> (xLine const&) const = default;

		template < typename archive >
		friend void serialize(archive& ar, xLine& var, unsigned int const file_version) {
			boost::serialization::base_object<xShape>(var);
			ar & var;
		}
		template < typename archive >
		friend archive& operator & (archive& ar, xLine& var) {
			ar & boost::serialization::base_object<xShape>(var);
			return ar & var.m_pt0 & var.m_pt1;
		}

		virtual bool LoadFromCADJson(json_t& _j) override {
			xShape::LoadFromCADJson(_j);
			using namespace std::literals;
			gtl::bjson j(_j);
			m_pt0 = PointFrom(j["basePoint"sv]);
			m_pt1 = PointFrom(j["secPoint"sv]);
			return true;
		}
	};


#pragma pack(pop)
}

export CEREAL_REGISTER_TYPE(biscuit::shape::xLine);
export CEREAL_REGISTER_POLYMORPHIC_RELATION(biscuit::shape::xShape, biscuit::shape::xLine);
export CEREAL_CLASS_VERSION(biscuit::shape::xLine, biscuit::shape::xLine::s_version);
