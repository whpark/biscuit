module;

#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"
#include "macro.h"

export module biscuit.dxf:entities_subclass;
import std;
import fmt;
import Eigen;
import biscuit;
import :group;

using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::dxf::subclass {

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

	using point_t = Eigen::Vector3d;

	//template < typename TSubclass >
	//bool ReadEntitySubclass(TSubclass& subclass, group_iter_t& iter, group_iter_t const& end) {
	//	TContainerMap<group_code_t, int> mapGroupCodeToIndex; // for duplicated group number
	//	for (iter++; iter != end; iter++) {
	//		if (iter->eCode == group_code_t::subclass or iter->eCode == group_code_t::entity) {
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

	//		if (iter->eCode == group_code_t::entity) {
	//			iter--;	// current item is for next sequence.
	//			return true;
	//		}
	//		return false;
	//	}
	//	return true;
	//}

	//-------------------------------------------------------------------------
	struct sEntity {
	public:
		using this_t = sEntity;
	public:
		code_to_value_t<  5> handle{};
		code_to_value_t<330> hOwnerBlock;
		code_to_value_t<100> marker;					// 100:AcDbEntity (SubclassMarker)
		code_to_value_t< 67, eSPACE> space{};			// 0 for model, 1 for paper
		code_to_value_t<410> layout_tab_name;
		code_to_value_t<  8> layer;
		code_to_value_t<  6> line_type_name;
		code_to_value_t<347> ptrMaterial;
		code_to_value_t< 62, eCOLOR> color{ 256 };		// 62:color, 0 for ByBlock, 256 for ByLayer, negative value indicates layer is off.
		code_to_value_t<370> line_weight{};				// 370: Stored and moved around as a 16-bit integer (?)
		code_to_value_t< 48> line_type_scale{ 1.0 };	// 48: optional
		code_to_value_t< 60> hidden{ 0 };				// 60: 0: visible, 1: invisible
		code_to_value_t< 92> size_graphics_data{};
		code_to_value_t<310> graphics_data{};
		code_to_value_t<420, color_bgra_t> color24{};	// 420: 24-bit color value - lowest 8 bits are blue, next 8 are green, highest 8 are red
		code_to_value_t<430> color_name;
		code_to_value_t<440> transparency{};
		code_to_value_t<390> ptr_plot_style_object{};
		code_to_value_t<284> shadow_mode{};				// 0 : Casts and received shadows, 1 : Casts shadows, 2 : Receives shadows, 3 : Ignores shadows

		//point_t extrusion{0., 0., 1.};
		//code_to_value_t< 39> thickness{};
		constexpr static inline auto group_members = std::make_tuple(
			5, &this_t::handle,
			330, &this_t::hOwnerBlock,
			100, &this_t::marker,
			67, &this_t::space,
			410, &this_t::layout_tab_name,
			8, &this_t::layer,
			6, &this_t::line_type_name,
			347, &this_t::ptrMaterial,
			62, &this_t::color,
			370, &this_t::line_weight,
			48, &this_t::line_type_scale,
			60, &this_t::hidden,
			92, &this_t::size_graphics_data,
			310, &this_t::graphics_data,
			420, BSC__LAMBDA_MEMBER_VALUE(color24.Value()),
			430, &this_t::color_name,
			440, &this_t::transparency,
			390, &this_t::ptr_plot_style_object,
			284, &this_t::shadow_mode,

			//39, &this_t::thickness,
			//210, BSC__LAMBDA_MEMBER_VALUE(extrusion.x()),
			//220, BSC__LAMBDA_MEMBER_VALUE(extrusion.y()),
			//230, BSC__LAMBDA_MEMBER_VALUE(extrusion.z())
			0
		);

		bool operator == (this_t const&) const = default;
		bool operator != (this_t const&) const = default;
		auto operator <=> (this_t const&) const = default;

	};


}

