module;

//////////////////////////////////////////////////////////////////////
//
// coord_trans.h: coord trans
//
// PWH
// 2019.11.02. 전체 수정
// 2021.05.21. renewal
// 2021.07.05. new - naming convention
// 2024-08-12. biscuit.
//
//////////////////////////////////////////////////////////////////////

#include "biscuit/config.h"
#include "biscuit/macro.h"
#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_glaze.h"
#include "biscuit/dependencies_units.h"


#include <cereal/types/polymorphic.hpp>
//#include <cereal/archives/binary.hpp>


export module biscuit.coord.transform;
import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.memory;
import biscuit.coord.concepts;
import biscuit.coord.point;
//import biscuit.misc;

namespace concepts = biscuit::concepts;

export namespace biscuit {
#pragma pack(push, 8)

	//-----------------------------------------------------------------------------
	// interface CoordTrans;
	class ICoordTrans {
	public:
		using this_t = ICoordTrans;
		using value_type = double;

		// Constructors
		//ICoordTrans() = default;
		virtual ~ICoordTrans() { }
	public:
		BSC__NODISCARD virtual std::unique_ptr<ICoordTrans> clone() const = 0;
		//static_assert(concepts::cloneable<this_t>);
		template < typename tarchive >
		void serialize(tarchive& ar/*, std::uint32_t const version*/) { }
		bool operator == (this_t const&) const = default;
		bool operator != (this_t const&) const = default;

	public:
		virtual std::unique_ptr<ICoordTrans> GetInverse() const = 0;

		/// @brief Transform
		/// @return sPoint3d for point, length for length
		template < typename tpoint >
			requires (concepts::coord::generic_point<tpoint> or concepts::arithmetic<tpoint>)
		BSC__NODISCARD auto operator () (tpoint const& pt) const {
			if constexpr (concepts::coord::has_ipoint3<tpoint>)			return (tpoint)Trans(pt.x(), pt.y(), pt.z());
			else if constexpr (concepts::coord::has_ipoint2<tpoint>)	return (tpoint)Trans(pt.x(), pt.y());
			else if constexpr (concepts::coord::has_point3<tpoint>)		return (tpoint)Trans(pt.x, pt.y, pt.z);
			else if constexpr (concepts::coord::has_point2<tpoint>)		return (tpoint)Trans(pt.x, pt.y);
			else if constexpr (concepts::arithmetic<tpoint>)			return Trans(pt);
		}

		BSC__NODISCARD virtual double Trans(double l) const {
			static double const sqrt_1_3 = 1.0 / std::sqrt(3.0);
			auto rate = Trans(1., 1., 1.).GetDistance() * sqrt_1_3;
			return rate * l;
		}
		BSC__NODISCARD virtual sPoint2d Trans(double x, double y) const { return Trans(x, y, 0.0); }
		BSC__NODISCARD virtual sPoint3d Trans(double x, double y, double z) const = 0;
		BSC__NODISCARD virtual bool IsRightHanded() const = 0;
	};
	static_assert(concepts::cloneable<ICoordTrans>);

	//-----------------------------------------------------------------------------
	class xCoordTransChain : public ICoordTrans {
	public:
		using base_t = ICoordTrans;
		using this_t = xCoordTransChain;

		std::deque<TCloneablePtr<ICoordTrans>> m_chain;	// 마지막 Back() CT부터 Front() 까지 Transform() 적용.

	public:

		bool operator == (this_t const&) const = default;
		bool operator != (this_t const&) const = default;
		BSC__NODISCARD std::unique_ptr<ICoordTrans> clone() const override { return std::make_unique<xCoordTransChain>(*this); }

		template < typename tarchive >
		void serialize(tarchive &ar, unsigned int const version) {
			ar((std::deque<std::unique_ptr<ICoordTrans>>&)m_chain);
		}

	public:
		xCoordTransChain& operator *= (xCoordTransChain const& B)	{
			std::ranges::copy(B.m_chain, std::back_inserter(m_chain));
			return *this;
		}
		xCoordTransChain& operator *= (ICoordTrans const& B) {
			m_chain.push_back(B.clone());
			return *this;
		}
		BSC__NODISCARD xCoordTransChain operator * (xCoordTransChain const& B) const {
			xCoordTransChain newChain(*this);
			return newChain *= B;
		}
		BSC__NODISCARD xCoordTransChain operator * (ICoordTrans const& B) const {
			xCoordTransChain newChain(*this);
			newChain *= B;
			return newChain;
		}

		BSC__NODISCARD friend xCoordTransChain operator * (ICoordTrans const& A, xCoordTransChain const& B) {
			xCoordTransChain newChain;
			newChain *= A;
			newChain *= B;
			return newChain;
		}

		virtual std::unique_ptr<ICoordTrans> GetInverse() const override {
			auto tr = std::make_unique<xCoordTransChain>();
			for (auto iter = m_chain.rbegin(); iter != m_chain.rend(); iter++) {
				if (auto r = (*iter)->GetInverse(); r) {
					tr->m_chain.push_back(std::move(r));
				} else {
					return {};
				}
			}
			return tr;
		}

		BSC__NODISCARD sPoint3d Trans(double x, double y, double z) const override {
			sPoint3d pt(x, y, z);
			for (auto iter = m_chain.rbegin(); iter != m_chain.rend(); iter++) {
				pt = (*iter)->Trans(pt.x, pt.y, pt.z);
			}
			return pt;
		}

		BSC__NODISCARD virtual bool IsRightHanded() const override {
			bool bRightHanded{true};
			for (auto const& ct : m_chain) {
				if (!ct->IsRightHanded())
					bRightHanded = !bRightHanded;
			}
			return bRightHanded;
		}

	};
	//static_assert(concepts::cloneable<xCoordTransChain>);

	//-----------------------------------------------------------------------------
	/// @brief class TCoordTransDim 
	/// TARGET = scale * mat ( SOURCE - origin ) + offset
	template < int DIM = 2, Eigen::TransformTraits TRANSFORM_MODE = Eigen::TransformTraits::AffineCompact >
	class TCoordTransMat : public ICoordTrans {
		static_assert( DIM >= 2 and DIM <= 3 );
	public:
		using this_t = TCoordTransMat;
		using base_t = ICoordTrans;
		constexpr static auto const dim = DIM;
		constexpr static auto const transform_mode = TRANSFORM_MODE;
		using transform_t = Eigen::Transform<double, dim, TRANSFORM_MODE>;
		using mat_t = transform_t::MatrixType;
		transform_t m_transform{};
	public:

		BSC__NODISCARD virtual std::unique_ptr<ICoordTrans> clone() const { return std::make_unique<this_t>(*this); }

		//friend class boost::serialization::access;
		template < typename Archive >
		friend void serialize(Archive &ar, std::uint32_t const version) {
			ar(cereal::BinaryData(m_transform.data(), m_transform.matrix().array().size()*sizeof(m_transform.data()[0])));
		}

	public:

		bool operator == (this_t const&) const = default;
		bool operator != (this_t const&) const = default;

		//-------------------------------------------------------------------------
		std::unique_ptr<ICoordTrans> GetInverse() const override {
			double d = m_transform.matrix().determinant();
			if (d < 1.e-10 or !std::isfinite(d))
				return {};

			std::unique_ptr<this_t> tr(new this_t);
			tr->m_transform = m_transform.inverse();

			return std::move(tr);
		}

		BSC__NODISCARD virtual double Trans(double l) const {
			if constexpr (dim == 2) {
				static double const sqrt_1_2 = 1.0 / std::sqrt(2.0);
				auto rate = Trans(1., 1., 0.).GetDistance() * sqrt_1_2;
				return rate * l;
			}
			else if constexpr (dim == 3) {
				static double const sqrt_1_3 = 1.0 / std::sqrt(3.0);
				auto rate = Trans(1., 1., 1.).GetDistance() * sqrt_1_3;
				return rate * l;
			}
		}
		BSC__NODISCARD virtual sPoint2d Trans(double x, double y) const {
			if constexpr (dim == 2) {
				auto pt = m_transform * Eigen::Vector2d(x, y);
				return { pt.x(), pt.y() };
			}
			else if constexpr (dim == 3) {
				auto pt = m_transform * Eigen::Vector3d(x, y, 0.0);
				return { pt.x(), pt.y() };
			}
			else static_assert(false);
		}
		BSC__NODISCARD virtual sPoint3d Trans(double x, double y, double z) const {
			if constexpr (dim == 2) {
				auto p = m_transform * Eigen::Vector2d(x, y);
				return {p.x(), p.y(), z};
			}
			else if constexpr (dim == 3) {
				auto p = m_transform * Eigen::Vector3d(x, y, z);
				return {p.x(), p.y(), p.z()};
			}
			else static_assert(false);
		}
		BSC__NODISCARD virtual bool IsRightHanded() const {
			return m_transform.matrix().determinant() >= 0;
		}

		//-------------------------------------------------------------------------
		void AdjustOffset(sPoint3d const& origin, sPoint3d const& offset) {
			auto pt = m_transform * (Eigen::Vector3d const&)origin;
			auto diff = (Eigen::Vector3d const&)offset - pt;
			m_transform.translation() += diff;
		}

	};

	//template TCoordTransDim<2>;
	using xCoordTrans2d = TCoordTransMat<2>;

	//template TCoordTransDim<3>;
	using xCoordTrans3d = TCoordTransMat<3>;

#pragma pack(pop)
}	// namespace gtl
