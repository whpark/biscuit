module;

//////////////////////////////////////////////////////////////////////
//
// entities.insert.ixx
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

export module biscuit.shape.entities.insert;
import std;
import "biscuit/dependencies/fmt.hxx";
import "biscuit/dependencies/cereal.hxx";
import "biscuit/dependencies/eigen.hxx";
import biscuit;
import biscuit.shape_basic;
import biscuit.shape.entities.shape;

export namespace biscuit::shape {

	class xInsert : public xShape {
	public:
		string_t m_name;	// block name
		point_t m_pt;
		double m_xscale{1};
		double m_yscale{1};
		double m_zscale{1};
		deg_t m_angle{};
		int m_nCol{1};
		int m_nRow{1};
		double m_spacingCol{};
		double m_spacingRow{};

		BSC__SHAPE_BASE_DEFINITION(xInsert, xShape, eSHAPE::insert);
		BSC__SHAPE_ARCHIVE_MEMBER(xInsert, xShape, 1u, m_name, m_pt, m_xscale, m_yscale, m_zscale, m_angle, m_nCol, m_nRow, m_spacingCol, m_spacingRow);

		virtual std::optional<line_t> GetStartEndPoint() const override { return {}; }
		virtual void FlipX() override {}
		virtual void FlipY() override {}
		virtual void FlipZ() override {}
		virtual void Reverse() override {}
		virtual void Transform(ct_t const&, bool /*= ct.IsRightHanded()*/) override {};
		virtual bool UpdateBounds(rect_t&) const override { return false; };
		virtual void Draw(ICanvas&) const override {
			//xShape::Draw(canvas);
		};
	};


}

BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(biscuit::shape::xInsert);

