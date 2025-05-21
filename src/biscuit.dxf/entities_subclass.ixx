module;

#include "macro.h"

export module biscuit.dxf:entities_subclass;
import std;
import "biscuit/dependencies/fmt.hxx";
import "biscuit/dependencies/eigen.hxx";
import biscuit;
import :group;
import :group_code_type_alias;

using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::dxf::entities {

	//=============================================================================================================================

	enum class eAC_DB {
		none,

		SymbolTable,
		RegAppTableRecord,
		BlockTableRecord,
		DimStyleTableRecord,
		LayerTableRecord,
		LineTypeTableRecord,
		TextStyleTableRecord,
		UCSTableRecord,
		ViewTableRecord,
		ViewportTableRecord,

		BlockBegin,
		BlockEnd,

		Entity,
		Face,
		ModelerGeometry,
		_3dSolid,
		ProxyEntity,
		Circle,
		Arc,
		Text,
		AttributeDefinition,
		Xrecord,
		MText,
		Attribute,
		Dimension,
		AlignedDimension,
		RotatedDimension,
		RadialDimension,
		DiametricDimension,
		_3PointAngularDimension,
		OrdinateDimension,
		Ellipse,
		Hatch,
		Helix,
		RasterImage,
		BlockReference,
		Leader,
		Light,
		Line,
		Polyline,
		SubDMesh,
		MLine,
		OleFrame,
		Ole2Frame,
		Point,
		_2dPolyline,
		_3dPolyline,
		Ray,
		Section,
		Shape,
		Trace,
		Spline,
		Sun,
		Surface,
		ExtrudedSurface,
		LoftedSurface,
		RevolvedSurface,
		SweptSurface,
		Table,
		Fcf,
		UnderlayReference,
		Vertex,
		_2dVertex,
		_3dPolylineVertex,
		Viewport,
		Xline,
	};

	enum class eSPACE : int16 { model, paper };
	enum class eCOLOR : int16 { byBlock = 0, byLayer = 256, };

	using point_t = xPoint3d;

	//template < typename TSubclass >
	//bool ReadEntitySubclass(TSubclass& subclass, group_iter_t& iter, group_iter_t const& end) {
	//	TContainerMap<group_code_value_t, int> mapGroupCodeToIndex; // for duplicated group number
	//	for (iter++; iter != end; iter++) {
	//		if (iter->eCode == group_code_value_t::subclass or iter->eCode == group_code_value_t::entity) {
	//			iter--;
	//			return true;
	//		}

	//		// primary handler
	//		size_t index = mapGroupCodeToIndex[iter->eCode]++;
	//		if (ReadItemSingleMember<TSubclass>(subclass, *iter, index))
	//			continue;

	//		// secondary handler ex, ExtendedData (GroupCode 1001)
	//		if constexpr (requires(TSubclass v) { v.ReadPrivate(iter, end); }) {
	//			if (subclass.ReadPrivate(iter, end))
	//				continue;
	//		}

	//		if (iter->eCode == group_code_value_t::entity) {
	//			iter--;	// current item is for next sequence.
	//			return true;
	//		}
	//		return false;
	//	}
	//	return true;
	//}

	//-------------------------------------------------------------------------
	struct sField {
	public:
		using this_t = sField;
	public:
		gcv<  5> handle{};
		gcv<330> hOwnerBlock;
		gcv<100> marker;						// 100:AcDbEntity (SubclassMarker)
		gcv< 67, eSPACE> space{};				// 67:space, 0 for model, 1 for paper
		gcv<410> layout_tab_name;
		gcv<  8> layer;
		gcv<  6> line_type_name;
		gcv<347> ptrMaterial;
		gcv< 62, eCOLOR> color{ eCOLOR::byLayer };		// 62:color, 0 for ByBlock, 256 for ByLayer, negative value indicates layer is off.
		gcv<370> line_weight{};					// 370: Stored and moved around as a 16-bit integer (?)
		gcv< 48> line_type_scale{ 1.0 };		// 48: optional
		gcv< 60> hidden{ 0 };					// 60: 0: visible, 1: invisible
		gcv< 92> size_graphics_data{};
		gcv<310> graphics_data{};
		gcv<420, color_bgra_t> color24{};		// 420: 24-bit color value - lowest 8 bits are blue, next 8 are green, highest 8 are red
		gcv<430> color_name;
		gcv<440> transparency{};
		gcv<390> ptr_plot_style_object{};
		gcv<284> shadow_mode{};					// 0 : Casts and received shadows, 1 : Casts shadows, 2 : Receives shadows, 3 : Ignores shadows

		//point_t extrusion{0., 0., 1.};
		//group_code_value_t<039> thickness{};
		constexpr static inline auto group_members = std::make_tuple(
			std::pair{420, [](auto& self) -> auto& { color_bgra_t& cr = self.color24; return cr.Value(); } },
			0
		);

		bool operator == (this_t const&) const = default;
		bool operator != (this_t const&) const = default;
		auto operator <=> (this_t const&) const = default;

	};

}

