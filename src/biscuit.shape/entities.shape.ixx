﻿module;

//////////////////////////////////////////////////////////////////////
//
// shape.ixx:
//
// PWH
// 2021.05.28
// 2021.08.11 Compare
// 2024-10-10. biscuit
//
///////////////////////////////////////////////////////////////////////////////

#include "biscuit/macro.h"
#include "biscuit/dependencies/glaze.h"

export module biscuit.shape.entities.shape;
import std;
import "biscuit/dependencies/fmt.hxx";
import "biscuit/dependencies/cereal.hxx";
import "biscuit/dependencies/eigen.hxx";
import biscuit;
import biscuit.shape_basic;

export namespace biscuit::shape {
	class ICanvas;

	//=============================================================================================================================
	/// @brief shape interface class
	class xShape {
	protected:
		friend class xDrawing;
		mutable int m_crIndex{};		// 0 : byblock, 256 : bylayer, negative : layer is turned off (optional)
	public:
		mutable string_t m_strLayer;	// temporary value. (while loading from dxf)
	public:
		using this_t = xShape;

	public:
		constexpr static inline uint32_t s_version{1u};
		color_t m_color{};
		int m_eLineType{};
		string_t m_strLineType;
		int m_lineWeight{1};
		bool m_bVisible{true};
		bool m_bTransparent{};
		std::optional<sCookie> m_cookie;

	public:
		enum class eLINE_WIDTH : int {
			ByLayer = 29,
			ByBlock = 30,
			Default = 31
		};

	public:
		xShape() = default;
		xShape(xShape const&) = default;
		xShape(xShape&&) = default;
		xShape& operator = (xShape const&) = default;
		xShape& operator = (xShape&&) = default;
	public:
		virtual ~xShape() {}

	public:
		virtual std::unique_ptr<xShape> clone() const = 0;
		virtual eSHAPE GetShapeType() const { return eSHAPE::none; }

		auto operator <=> (xShape const&) const = default;

		virtual bool Compare(xShape const& B) const {
			if (GetShapeType() != B.GetShapeType())
				return false;
			return *this == B;
		}

		struct glaze {
			using GLZ_T = this_t;
			[[maybe_unused]] static constexpr std::string_view name = "xShape";
			static constexpr auto value = glz::object(
				"color", &GLZ_T::m_color, "LineType", &GLZ_T::m_eLineType, "LineTypeName", &GLZ_T::m_strLineType, "LineWeight", &GLZ_T::m_lineWeight,
				"Visible", &GLZ_T::m_bVisible, "Transparent", &GLZ_T::m_bTransparent, "Cookie", &GLZ_T::m_cookie);
		};

		template < typename archive >
		void serialize(archive& ar, [[maybe_unused]] std::uint32_t const file_version) {
			ar(m_color.Value(), m_cookie, m_strLineType, m_lineWeight, m_eLineType, m_bVisible, m_bTransparent);
		}

		string_t const& GetShapeName() const { return GetShapeName(GetShapeType()); }
		static string_t const& GetShapeName(eSHAPE eType);

		//virtual point_t PointAt(double t) const = 0;
		virtual std::optional<line_t> GetStartEndPoint() const = 0;
		virtual void FlipX() = 0;
		virtual void FlipY() = 0;
		virtual void FlipZ() = 0;
		virtual void Reverse() = 0;
		virtual void Transform(ct_t const& ct, bool bRightHanded /*= ct.IsRightHanded()*/) = 0;
		virtual bool UpdateBounds(rect_t&) const = 0;
		virtual rect_t GetBounds() const {
			rect_t bounds;
			bounds.SetEmptyForMinMax2d();
			UpdateBounds(bounds);
			return bounds;
		}
		int GetLineWidthInUM() const { return GetLineWidthInUM(m_lineWeight); }
		static int GetLineWidthInUM(int lineWeight);

		virtual void Draw(ICanvas& canvas) const;
		virtual bool DrawROI(ICanvas& canvas, rect_t const& rectROI) const;

		//virtual void PrintOut(std::wostream& os) const {
		//	fmt::print(os, L"Type:{} - Color({:02x},{:02x},{:02x}), {}{}", GetShapeName(GetShapeType()), m_color.r, m_color.g, m_color.b, !m_bVisible?L"Invisible ":L"", m_bTransparent?L"Transparent ":L"");
		//	fmt::print(os, L"lineType:{}, lineWeight:{}\n", m_strLineType, m_lineWeight);
		//}

		static std::unique_ptr<xShape> CreateShapeFromEntityName(std::string const& strEntityName);
	};

	using shapes_t = TSmartPtrContainer<xShape, TCloneablePtr, std::deque>;
}

export CEREAL_CLASS_VERSION(biscuit::shape::xShape, biscuit::shape::xShape::s_version);

