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
// 2024-10-14. biscuit.
//
///////////////////////////////////////////////////////////////////////////////

#include "biscuit/macro.h"
#include "biscuit/dependencies/glaze.h"

#include "shape_macro.h"

export module biscuit.shape.entities.line;
import std;
import "biscuit/dependencies/fmt.hxx";
import "biscuit/dependencies/cereal.hxx";
import "biscuit/dependencies/eigen.hxx";
import biscuit;
import biscuit.shape_basic;
import biscuit.shape.entities.shape;
import biscuit.shape.canvas;

export namespace biscuit::shape {

	class xLine : public xShape {
	public:
		point_t m_pt0, m_pt1;

		BSC__SHAPE_BASE_DEFINITION(xLine, xShape, eSHAPE::line);
		BSC__SHAPE_ARCHIVE_MEMBER(xLine, xShape, 1u, m_pt0, m_pt1);

		//virtual point_t PointAt(double t) const override { return lerp(m_pt0, m_pt1, t); }
		virtual std::optional<line_t> GetStartEndPoint() const override {
			return line_t{ m_pt0, m_pt1 };
		}
		virtual void FlipX() override { m_pt0.x = -m_pt0.x; m_pt1.x = -m_pt1.x; }
		virtual void FlipY() override { m_pt0.y = -m_pt0.y; m_pt1.y = -m_pt1.y; }
		virtual void FlipZ() override { m_pt0.z = -m_pt0.z; m_pt1.z = -m_pt1.z; }
		virtual void Reverse() override {
			std::swap(m_pt0, m_pt1);
		}
		virtual void Transform(ct_t const& ct, [[maybe_unused]] bool bRightHanded) override {
			m_pt0 = ct(m_pt0); m_pt1 = ct(m_pt1);
		};
		virtual bool UpdateBounds(rect_t& bounds) const override {
			bool bModified{};
			bModified |= bounds.UpdateBounds(m_pt0);
			bModified |= bounds.UpdateBounds(m_pt1);
			return bModified;
		};
		virtual void Draw(ICanvas& canvas) const override {
			xShape::Draw(canvas);
			canvas.Line(m_pt0, m_pt1);
		}
		//virtual void PrintOut(std::wostream& os) const override {
		//	xShape::PrintOut(os);
		//	fmt::print(os, L"\tpt0({},{},{}) - pt1({},{},{})\n", m_pt0.x, m_pt0.y, m_pt0.z, m_pt1.x, m_pt1.y, m_pt1.z);
		//}
		//virtual bool LoadFromCADJson(json_t& _j) override {
		//	xShape::LoadFromCADJson(_j);
		//	using namespace std::literals;
		//	gtl::bjson j(_j);
		//	m_pt0 = PointFrom(j["basePoint"sv]);
		//	m_pt1 = PointFrom(j["secPoint"sv]);
		//	return true;
		//}
	};

}

BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(biscuit::shape::xLine);

