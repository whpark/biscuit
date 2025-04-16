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
// 2024-10-14. biscuit
//
///////////////////////////////////////////////////////////////////////////////

#include "biscuit/macro.h"
#include "biscuit/dependencies_glaze.h"

#include "shape_macro.h"

export module biscuit.shape.entities.text;
import std;
import "biscuit/dependencies_fmt.hxx";
import "biscuit/dependencies_cereal.hxx";
import "biscuit/dependencies_eigen.hxx";
import biscuit;
import biscuit.shape_basic;
import biscuit.shape.entities.shape;
import biscuit.shape.canvas;

export namespace biscuit::shape {

	//-----------------------------------------------------------------------------------------------------------------------------
	class xText : public xShape {
	public:
		enum class eALIGN_VERT : int { base_line = 0, bottom, mid, top };
		enum class eALIGN_HORZ : int { left = 0, center, right, aligned, middle, fit };

		point_t m_pt0, m_pt1;
		string_t m_text;
		double m_height{};
		deg_t m_angle{};
		double m_widthScale{};
		deg_t m_oblique{};
		string_t m_textStyle;
		int m_textgen{};
		eALIGN_HORZ m_alignHorz{eALIGN_HORZ::left};
		eALIGN_VERT m_alignVert{eALIGN_VERT::base_line};

		BSC__SHAPE_BASE_DEFINITION(xText, xShape, eSHAPE::text);
		BSC__SHAPE_ARCHIVE_MEMBER(xText, xShape, 1u,
			m_pt0, m_pt1, m_text, m_height, m_angle, m_widthScale,
			m_oblique, m_textStyle, m_textgen, m_alignHorz, m_alignVert);

		//virtual point_t PointAt(double t) const override {};
		virtual std::optional<line_t> GetStartEndPoint() const override {
			// todo : Start/End Point
			return line_t{ m_pt0, m_pt1 };
		}
		virtual void FlipX() override { m_pt0.x = -m_pt0.x; m_pt1.x = -m_pt1.x; }
		virtual void FlipY() override { m_pt0.y = -m_pt0.y; m_pt1.y = -m_pt1.y; }
		virtual void FlipZ() override { m_pt0.z = -m_pt0.z; m_pt1.z = -m_pt1.z; }
		virtual void Reverse() override {
		}
		virtual void Transform(ct_t const& ct, [[maybe_unused]] bool bRightHanded) override {
			m_pt0 = ct(m_pt0);
			m_pt1 = ct(m_pt1);
		}
		virtual bool UpdateBounds(rect_t& bounds) const override {
			// todo : upgrade.
			bool b{};
			b |= bounds.UpdateBounds(m_pt0);
			//b |= bounds.UpdateBounds(pt1);
			return b;
		}
		virtual void Draw(ICanvas& canvas) const override {
			xShape::Draw(canvas);
			canvas.Text(*this);
		}

		//virtual bool LoadFromCADJson(json_t& _j) override {
		//	xShape::LoadFromCADJson(_j);
		//	using namespace std::literals;
		//	gtl::bjson j(_j);

		//	m_pt0 = PointFrom(j["basePoint"sv]);
		//	m_pt1 = PointFrom(j["secPoint"sv]);
		//	m_text = j["text"sv];
		//	m_height = j["height"sv];
		//	m_angle = deg_t{(double)j["angle"sv]};
		//	m_widthScale = j["widthscale"sv];
		//	m_oblique = deg_t{(double)j["oblique"sv]};
		//	m_textStyle = j["style"sv];
		//	m_textgen = j["textgen"sv];
		//	m_alignHorz = (eALIGN_HORZ)(int)j["alignH"sv];
		//	m_alignVert = (eALIGN_VERT)(int)j["alignV"sv];

		//	return true;
		//}
	};

	//-----------------------------------------------------------------------------------------------------------------------------
	class xMText : public xText {
	public:
	protected:
		using xText::m_alignVert;
	public:
		double m_interlin{};
		enum class eATTACH {
			topLeft = 1,
			topCenter,
			topRight,
			middleLeft,
			middleCenter,
			middleRight,
			bottomLeft,
			bottomCenter,
			bottomRight
		};

		BSC__SHAPE_BASE_DEFINITION(xMText, xText, eSHAPE::mtext);
		BSC__SHAPE_ARCHIVE_MEMBER(xMText, xText, 1u, m_interlin);

		virtual void Draw(ICanvas& canvas) const override {
			xShape::Draw(canvas);
			canvas.Text(*this);
		}

		//virtual bool LoadFromCADJson(json_t& _j) override {
		//	xText::LoadFromCADJson(_j);
		//	using namespace std::literals;
		//	gtl::bjson j(_j);

		//	m_interlin = j["interlin"sv];

		//	return true;
		//}
		//eATTACH& eAttch{(eATTACH&)alignVert};
		eATTACH GetAttachPoint() const { return (eATTACH)m_alignVert; }
		void SetAttachPoint(eATTACH eAttach) { m_alignVert = (eALIGN_VERT)eAttach; }

	};

}


BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(biscuit::shape::xText);
BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(biscuit::shape::xMText);

