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

#include "shape_macro.h"

export module biscuit.shape.entities.dot;
import std;
import fmt;
import biscuit;
import biscuit.shape_basic;
import biscuit.shape.entities.shape;
import biscuit.shape.canvas;

export namespace biscuit::shape {

	class xDot : public xShape {
	public:
		point_t m_pt;

	public:
		BSC__SHAPE_BASE_DEFINITION(xDot, xShape, eSHAPE::dot);
		BSC__SHAPE_ARCHIVE_MEMBER(xDot, xShape, 1u, m_pt);

		//virtual point_t PointAt(double t) const override { return pt; };
		virtual std::optional<line_t> GetStartEndPoint() const override {
			return line_t{ m_pt, m_pt };
		}
		virtual void FlipX() override { m_pt.x = -m_pt.x; }
		virtual void FlipY() override { m_pt.y = -m_pt.y; }
		virtual void FlipZ() override { m_pt.z = -m_pt.z; }
		virtual void Reverse() override { }
		virtual void Transform(ct_t const& ct, [[maybe_unused]] bool bRightHanded) override {
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

BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(biscuit::shape::xDot);

