module;

//////////////////////////////////////////////////////////////////////
//
// entities.group.ixx:
//
// PWH
// 2017.07.20
// 2021.05.27
// 2021.08.11 Compare
// 2023-06-09 분리
// 2024-10-22. biscuit. 
//
///////////////////////////////////////////////////////////////////////////////

#include "biscuit/macro.h"
#include "biscuit/dependencies/glaze.h"

#include "shape_macro.h"

export module biscuit.shape.entities.group;
import std;
import "biscuit/dependencies/fmt.hxx";
import "biscuit/dependencies/cereal.hxx";
import "biscuit/dependencies/eigen.hxx";
import biscuit;
import biscuit.shape_basic;
import biscuit.shape.entities.shape;
import biscuit.shape.entities.layer;
import biscuit.shape.canvas;


export namespace biscuit::shape {

	class xGroup : public xLayer {	// used by xDXFLoaderDime
	public:
		point_t m_pt;
		rect_t m_bounds;

	public:
		~xGroup() override = default;

	public:
		BSC__SHAPE_BASE_DEFINITION(xGroup, xLayer, eSHAPE::group);
		BSC__SHAPE_ARCHIVE_MEMBER(xGroup, xLayer, 1u, m_pt, m_bounds);
	};


}	// namespace biscuit::shape;

BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(biscuit::shape::xGroup);

