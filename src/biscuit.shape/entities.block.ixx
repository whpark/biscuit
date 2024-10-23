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
#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_glaze.h"
#include "biscuit/dependencies_units.h"
#include "biscuit/dependencies_cereal.h"

#include "shape_macro.h"

export module biscuit.shape.entities.block;
import std;
import biscuit;
import biscuit.shape.canvas;
import biscuit.shape.entities.shape;
import biscuit.shape.entities.layer;
import biscuit.shape_basic;

export namespace biscuit::shape {

	// internally
	class xBlock : public xLayer {
	public:
		string_t m_layer;
		point_t m_pt;

	public:
		BSC__SHAPE_BASE_DEFINITION(xBlock, xLayer, eSHAPE::block);
		BSC__SHAPE_ARCHIVE_MEMBER(xBlock, xLayer, 1u, m_layer, m_pt);

	};

}

BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(biscuit::shape::xBlock);

