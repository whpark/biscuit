module;

//////////////////////////////////////////////////////////////////////
//
// entities.hatch.ixx
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
#include "biscuit/dependencies_glaze.h"

#include "shape_macro.h"

export module biscuit.shape.entities.hatch;
import std;
import "biscuit/dependencies_fmt.hxx";
import "biscuit/dependencies_cereal.hxx";
import "biscuit/dependencies_eigen.hxx";
import biscuit;
import biscuit.shape_basic;
import biscuit.shape.entities.shape;
import biscuit.shape.entities.polyline;
import biscuit.shape.canvas;

export namespace biscuit::shape {

	class xHatch : public xShape {
	public:
		string_t m_name;
		bool m_bSolid{};
		bool m_bAssociative{};
		int m_nLoops{};
		int m_hstyle{};
		int m_hpattern{};
		bool m_bDouble{};
		deg_t m_angle{};
		double m_scale{};
		int m_deflines{};
		std::vector<xPolyline> m_boundaries;

		BSC__SHAPE_BASE_DEFINITION(xHatch, xShape, eSHAPE::hatch);
		BSC__SHAPE_ARCHIVE_MEMBER(xHatch, xShape, 1u, m_name, m_bSolid, m_bAssociative, m_nLoops, m_hstyle, m_hpattern, m_bDouble, m_angle, m_scale, m_deflines, m_boundaries);

		//virtual point_t PointAt(double t) const override {};
		virtual std::optional<line_t> GetStartEndPoint() const override { return {}; }
		virtual void FlipX() override { for (auto& b : m_boundaries) b.FlipX(); }
		virtual void FlipY() override { for (auto& b : m_boundaries) b.FlipY(); }
		virtual void FlipZ() override { for (auto& b : m_boundaries) b.FlipZ(); }
		virtual void Reverse() override { }
		virtual void Transform(ct_t const&, bool) override { };
		virtual bool UpdateBounds(rect_t& bounds) const override {
			bool b{};
			for (auto const& bound : m_boundaries) {
				b |= bound.UpdateBounds(bounds);
			}
			return b;
		}
		virtual void Draw(ICanvas& canvas) const override {
			xShape::Draw(canvas);
			// todo : draw hatch
		}
	};

}

BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(biscuit::shape::xHatch);

