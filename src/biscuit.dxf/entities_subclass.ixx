module;

#include "macro.h"

export module biscuit.dxf:entities_subclass;
import std;
import "biscuit/dependencies/fmt.hxx";
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

}

