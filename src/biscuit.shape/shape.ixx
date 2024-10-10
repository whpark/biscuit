module;

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
#include "biscuit/dependencies_glaze.h"
#include <cereal/cereal.hpp>

export module biscuit.shape.shape;
import std;
import biscuit;

export namespace biscuit::shape {

	using char_t = char;
	using string_t = std::string;
	using point_t = sPoint3d;
	using rect_t = sRect3d;
	using bounds_t = sBounds3d;
	struct line_t { point_t pt0, pt1; };

	//-------------------------------------------------------------------------
	template < typename T >
	struct TPolyPoint : public TPoint<T, 4, false> {
		using base_t = TPoint<T, 4, false>;
		using this_t = TPolyPoint;

		using base_t::base_t;

		T& Bulge() { return base_t::w; }
		T Bulge() const { return base_t::w; }
		auto operator <=> (TPolyPoint const&) const = default;
	};

	using polypoint_t = TPolyPoint<double>;

	using color_t = color_rgba_t;

	//constexpr color_t const CR_DEFAULT = ColorRGBA(255, 255, 255);

	//-------------------------------------------------------------------------
	enum class eSHAPE {
		none = -1,
		e3dface = 0,
		arc_xy,
		block,
		circle_xy,
		dimension,
		dimaligned,
		dimlinear,
		dimradial,
		dimdiametric,
		dimangular,
		dimangular3p,
		dimordinate,
		ellipse_xy,
		hatch,
		image,
		insert,
		leader,
		line,
		lwpolyline,
		mtext,
		dot,
		polyline,
		ray,
		solid,
		spline,
		text,
		trace,
		underlay,
		vertex,
		viewport,
		xline,

		layer = 127,
		drawing = 128,

		user_defined_1 = 256,
		user_defined_2,
		user_defined_3,
		user_defined_4,
		user_defined_5,
		user_defined_6,
		user_defined_7,
		user_defined_8,
		user_defined_9,
		user_defined_10,
		user_defined_11,
		user_defined_12,
	};

	//-------------------------------------------------------------------------
	struct sCookie {
		void* ptr{};
		std::vector<uint8_t> buffer;
		string_t str;
		std::chrono::nanoseconds duration{};

		GLZ_LOCAL_META(sCookie, ptr, buffer, str, duration);

		auto operator <=> (sCookie const&) const = default;

		template < typename archive >
		friend void serialize(archive& ar, sCookie& var, unsigned int const file_version) {
			ar & (ptrdiff_t&)var.ptr;
			ar & var.buffer;
			ar & var.str;
			decltype(var.duration)::rep count{};
			if constexpr (archive::is_saving()) {
				count = var.duration.count();
			}
			ar & count;

			if constexpr (archive::is_loading()) {
				var.duration = std::chrono::nanoseconds(count);
			}
			return ar;
		};
	};

	//-------------------------------------------------------------------------
	struct sLineType {
		string_t name;
		enum class eFLAG { xref = 16, xref_resolved = xref|32, modified_internal = 64 /* auto-cad internal command */ };
		int flags{};
		string_t description;
		std::vector<double> path;

		auto operator <=> (sLineType const&) const = default;

		template < typename archive >
		friend void serialize(archive& ar, sLineType& var, unsigned int const file_version) {
			ar & var.name;
			ar & var.flags;
			ar & var.path;
			if (file_version >= 1) {
				ar & var.description;
			}
		}
	};

	class ICanvas;

	//=============================================================================================================================
	/// @brief shape interface class
	class xShape {
	protected:
		friend class xDrawing;
		//mutable int m_crIndex{};		// 0 : byblock, 256 : bylayer, negative : layer is turned off (optional)
	public:
		//mutable string_t m_strLayer;	// temporary value. (while loading from dxf)
		using this_t = xShape;

	public:
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
	private:
		xShape(xShape const&) = default;
		xShape(xShape&&) = default;
		xShape& operator = (xShape const&) = default;
		xShape& operator = (xShape&&) = default;
	public:
		virtual ~xShape() {}

	public:
		virtual std::unique_ptr<xShape> clone() = 0;
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
		friend void serialize(archive& ar, xShape& shape, unsigned int const file_version) {
			ar & shape.m_color.Value();
			ar & shape.m_cookie;
			ar & shape.m_strLineType;
			ar & shape.m_lineWeight;
			ar & shape.m_eLineType;
			ar & shape.m_bVisible;
			ar & shape.m_bTransparent;
			return ar;
		}

		string_t const& GetShapeName() const { return GetShapeName(GetShapeType()); }
		static string_t const& GetShapeName(eSHAPE eType);

		//virtual point_t PointAt(double t) const = 0;
		virtual std::optional<std::pair<point_t, point_t>> GetStartEndPoint() const = 0;
		virtual void FlipX() = 0;
		virtual void FlipY() = 0;
		virtual void FlipZ() = 0;
		virtual void Reverse() = 0;
		virtual void Transform(xCoordTrans3d const& ct, bool bRightHanded /*= ct.IsRightHanded()*/) = 0;
		virtual bool UpdateBounds(bounds_t&) const = 0;
		virtual bounds_t GetBounds() const {
			bounds_t bounds;
			bounds.SetRectEmptyForMinMax2d();
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


	bool CohenSutherlandLineClip(biscuit::sRect2d roi, biscuit::sPoint2d& pt0, biscuit::sPoint2d& pt1);

}

export CEREAL_CLASS_VERSION(biscuit::shape::sLineType, 1);
//BOOST_CLASS_VERSION(biscuit::shape::xShape, 1);

