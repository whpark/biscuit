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
// 2024-10-10. biscuit
//
///////////////////////////////////////////////////////////////////////////////

#include "biscuit/macro.h"
#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_glaze.h"
#include "biscuit/dependencies_units.h"
#include "biscuit/dependencies_cereal.h"

export module biscuit.shape.entities.block;
import std;
import biscuit;
import biscuit.shape.canvas;
import biscuit.shape.shape;
import biscuit.shape.entities.layer;

export namespace biscuit::shape {

	// internally
	class xBlock : public xLayer {
	public:
		using base_t = xLayer;
		using this_t = xBlock;

	public:
	 	constexpr static inline uint32_t s_version{1u};
		string_t m_layer;
		point_t m_pt;

	public:
		virtual bool Compare(xShape const& B_) const override {
			if (!base_t::Compare(B_))
				return false;
			this_t const& B = (this_t const&)B_;
			return true
				and (m_layer == B.m_layer)
				and (m_pt == B.m_pt)
				;
		}
		virtual eSHAPE GetShapeType() const { return eSHAPE::block; }

		GTL__DYNAMIC_VIRTUAL_DERIVED(xBlock);
		auto operator <=> (xBlock const&) const = default;

		template < typename archive >
		friend void serialize(archive& ar, xBlock& var, unsigned int const file_version) {
			boost::serialization::base_object<xLayer>(var);
			ar & var;
		}
		template < typename archive >
		friend archive& operator & (archive& ar, xBlock& var) {
			ar & boost::serialization::base_object<xLayer>(var);

			ar & var.m_layer & var.m_pt;

			return ar;
		}

	};

}

export CEREAL_REGISTER_TYPE(biscuit::shape::xBlock);
export CEREAL_REGISTER_POLYMORPHIC_RELATION(biscuit::shape::xShape, biscuit::shape::xBlock);
export CEREAL_CLASS_VERSION(biscuit::shape::xBlock, biscuit::shape::xBlock::s_version);
