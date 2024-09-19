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
		transform_t m_transform{transform_t::Identity()};	// NO INITIALIZING
		using vector_t = Eigen::Vector<double, dim>;
		using point_t = TPoint<double, DIM, false>;

	public:
		BSC__NODISCARD virtual std::unique_ptr<ICoordTrans> clone() const { return std::make_unique<this_t>(*this); }

		//friend class boost::serialization::access;
		template < typename Archive >
		friend void serialize(Archive &ar, std::uint32_t const version) {
			auto size = m_transform.matrix().array().size() * sizeof(m_transform.data()[0]);
			ar(cereal::BinaryData(m_transform.data(), size));
		}

	public:
		bool operator == (this_t const& B) const { return m_transform.matrix() == B.m_transform.matrix(); }
		bool operator != (this_t const& B) const { return m_transform.matrix() != B.m_transform.matrix(); }

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
				if constexpr (m_transform.HDim == 3) {
					auto pt = m_transform * Eigen::Vector<double, 3>(x, y, 1.0);
					return { pt.x()/pt.z(), pt.y()/pt.z()};
				}
				else {
					auto pt = m_transform * vector_t(x, y);
					return { pt.x(), pt.y() };
				}
			}
			else if constexpr (dim == 3) {
				auto pt = m_transform * vector_t(x, y, 0.0);
				return { pt.x(), pt.y() };
			}
			else static_assert(false);
		}
		BSC__NODISCARD virtual sPoint3d Trans(double x, double y, double z) const {
			if constexpr (dim == 2) {
				if constexpr (m_transform.HDim == 3) {
					auto p = m_transform * Eigen::Vector<double, 3>(x, y, 1.0);
					return {p.x()/p.z(), p.y()/p.z(), z};
				}
				else {
					auto p = m_transform * vector_t(x, y);
					return {p.x(), p.y(), z};
				}
			}
			else if constexpr (dim == 3) {
				auto p = m_transform * vector_t(x, y, z);
				return {p.x(), p.y(), p.z()};
			}
			else static_assert(false);
		}
		BSC__NODISCARD virtual bool IsRightHanded() const {
			return m_transform.matrix().determinant() >= 0;
		}

		//-------------------------------------------------------------------------
		BSC__NODISCARD bool SetFrom2Pairs(std::span<point_t const> ptsSource, std::span<point_t const> ptsTarget, bool bCalcScale = true, double dMinDeterminant = 0.0, bool bRightHanded = true) 
			requires (dim == 2)
		{
			m_transform = transform_t::Identity();
			if ( (ptsSource.size() < 2) or (ptsTarget.size() < 2) )
				return false;

			std::array<point_t, 3> ptsS{ptsSource[0], ptsSource[1]}, ptsT{ptsTarget[0], ptsTarget[1]};
			// 세번째 점은 각각, 0번 기준으로 1번을 90도 회전시킴
			ptsS[2].x = -(ptsS[1].y-ptsS[0].y) + ptsS[0].x;
			ptsS[2].y =  (ptsS[1].x-ptsS[0].x) + ptsS[0].y;
			if (bRightHanded) {
				ptsT[2].x = -(ptsT[1].y-ptsT[0].y) + ptsT[0].x;
				ptsT[2].y =  (ptsT[1].x-ptsT[0].x) + ptsT[0].y;
			}
			else {
				ptsT[2].x =  (ptsT[1].y-ptsT[0].y) + ptsT[0].x;
				ptsT[2].y = -(ptsT[1].x-ptsT[0].x) + ptsT[0].y;
			}

			return SetFrom3Pairs(ptsS, ptsT, bCalcScale, dMinDeterminant);
		}
		BSC__NODISCARD bool SetFrom3Pairs(std::span<point_t const> ptsSource, std::span<point_t const> ptsTarget, bool bCalcScale = true, double dMinDeterminant = 0.0)
			requires (dim == 2)
		{
			using affine_t = Eigen::Matrix2d;

			m_transform = transform_t::Identity();
			if ( (ptsSource.size() < 3) or (ptsTarget.size() < 3) )
				return false;

			auto v1s = ptsSource[1] - ptsSource[0];
			auto v2s = ptsSource[2] - ptsSource[0];
			auto v1t = ptsTarget[1] - ptsTarget[0];
			auto v2t = ptsTarget[2] - ptsTarget[0];

			affine_t matS;
			matS << v1s.x, v2s.x, v1s.y, v2s.y;

			// Check.
			double d = matS.determinant();
			if (std::abs(d) <= dMinDeterminant)
				return false;

			affine_t matT;
			matT << v1t.x, v2t.x, v1t.y, v2t.y;

			affine_t mat = matT * matS.inverse();

			if (!bCalcScale) {
				double scale = std::abs(mat.determinant());
				mat /= scale;
			}
			m_transform.matrix().topLeftCorner<dim, dim>() = mat;

			AdjustOffset(ptsSource[0], ptsTarget[0]);

			return true;
		}
		BSC__NODISCARD bool SetFrom4Pairs(std::span<point_t const> ptsSource, std::span<point_t const> ptsTarget, bool bCalcScale = true, double dMinDeterminant = 0.0) {
			if constexpr (dim == 2) {
				static_assert(transform_mode == Eigen::TransformTraits::Projective);
				// Homography
				Eigen::Matrix<double, 8, 8> A;
				for (int i = 0; i < 4; i++) {
					A.row(i*2) << ptsSource[i].x, ptsSource[i].y, 1, 0, 0, 0, -ptsTarget[i].x*ptsSource[i].x, -ptsTarget[i].x*ptsSource[i].y;
					A.row(i*2+1) << 0, 0, 0, ptsSource[i].x, ptsSource[i].y, 1, -ptsTarget[i].y*ptsSource[i].x, -ptsTarget[i].y*ptsSource[i].y;
				}
				Eigen::Matrix<double, 8, 1> b;
				for (int i = 0; i < 4; i++) {
					b(i*2, 0) = ptsTarget[i].x;
					b(i*2+1, 0) = ptsTarget[i].y;
				}
				Eigen::Matrix<double, 8, 1> h = A.bdcSvd(Eigen::ComputeFullU|Eigen::ComputeFullV).solve(b);
				mat_t& mat = m_transform.matrix();
				mat << h(0), h(1), h(2), h(3), h(4), h(5), h(6), h(7), 1.0;
				return true;
			}
			else if constexpr (dim == 3) {

			}
			else {
				static_assert(false);
			}
		}

		//-------------------------------------------------------------------------
		double Scale() const {
			return std::sqrt(std::abs(m_transform.matrix().topLeftCorner<dim, dim>().determinant()));
		}
		double SetScale(double scale) {
			double s = Scale();
			if (s == 0.0)
				return 0.0;
			double rate = scale / s;
			m_transform.matrix().topLeftCorner<dim, dim>() *= rate;
			return rate;
		}
		double SetScale(double scale, point_t const& origin) {
			vector_t offset = m_transform * origin.vec();
			double s = Scale();
			if (s == 0.0)
				return 0.0;
			double rate = scale / s;
			m_transform.matrix().topLeftCorner<dim, dim>() *= rate;
			AdjustOffset(origin, point_t(offset));
			return rate;
		}
		double ScaleAffinePart(double rate) {
			m_transform.matrix().topLeftCorner<dim, dim>() *= rate;
			return rate;
		}

		void AdjustOffset(point_t const& origin, point_t const& offset) {
			auto pt = m_transform * origin.vec();
			auto diff = offset.vec() - pt;
			m_transform.translation() += diff;
		}

		void FlipLR(double x = 0.0) {
			m_transform.matrix().row(0) *= -1;
			if (x != 0.0)
				m_transform.translation().x() += 2*x;
		}
		void FlipUD(double y = 0.0) {
			m_transform.matrix().row(1) *= -1;
			if (y != 0.0)
				m_transform.translation().y() += 2*y;
		}
		void FlipFD(double z = 0.0) requires (dim >= 3) {
			m_transform.matrix().row(2) *= -1;
			if (z != 0.0)
				m_transform.translation().z() += 2*z;
		}

		void FlipSourceX() { m_transform.matrix().col(0) *= -1; }
		void FlipSourceY() { m_transform.matrix().col(1) *= -1; }
		void FlipSourceZ() requires (dim >= 3) { m_transform.matrix().col(2) *= -1; }

	};

	//template TCoordTransDim<2>;
	using xCoordTrans2d = TCoordTransMat<2, Eigen::TransformTraits::AffineCompact>;
	using xCoordTrans2dP = TCoordTransMat<2, Eigen::TransformTraits::Projective>;

	//template TCoordTransDim<3>;
	using xCoordTrans3d = TCoordTransMat<3>;

#pragma pack(pop)
}	// namespace gtl
