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

#include <cereal/types/polymorphic.hpp>
//#include <cereal/archives/binary.hpp>

#include <Eigen/Core>
#include <Eigen/Geometry>

export module biscuit.coord.trans;
import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.memory;
import biscuit.coord.concepts;
import biscuit.coord.point;
//import biscuit.misc;

#if 1

namespace concepts = biscuit::concepts;

export namespace biscuit::coord {
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
		BSC__NODISCARD virtual std::unique_ptr<ICoordTrans> clone() const = 0;

		//static_assert(concepts::cloneable<this_t>);

	public:
		template < typename tarchive >
		void serialize(tarchive& ar/*, std::uint32_t const version*/) { }

	public:
		virtual std::unique_ptr<ICoordTrans> GetInverse() const = 0;

		/// @brief Transform
		/// @return tpoint for tpoint, length for length
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
			auto rate = Trans(1., 1., 1.).GetLength() * sqrt_1_3;
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

		std::deque<TCloneablePtr<ICoordTrans>> m_chain;	// 마지막 Back() CT부터 Front() 까지 Transform() 적용.

		BSC__NODISCARD std::unique_ptr<ICoordTrans> clone() const override { return std::make_unique<xCoordTransChain>(*this); }

		template < typename tarchive >
		void serialize(tarchive &ar, unsigned int const version) {
			ar(m_chain);
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


	////-----------------------------------------------------------------------------
	///// @brief class TCoordTransDim 
	///// TARGET = scale * mat ( SOURCE - origin ) + offset
	//template < int dim = 2 >
	//class TCoordTransMat : public ICoordTrans {
	//	static_assert( dim >= 2 and dim <= 4 );
	//public:
	//	using this_t = TCoordTransDim;
	//	using base_t = ICoordTrans;
	//	using mat_t = glm::mat<dim, dim, double, glm::qualifier::highp>;
	//	using point_t = glm::vec<dim, double, glm::qualifier::highp>;
	//	glm::vec4 dw;

	//public:
	//	double m_scale{1.0};	// additional scale value
	//	mat_t m_mat;			// transforming matrix
	//	point_t m_origin;		// pivot of source coordinate
	//	point_t m_offset;		// pivot of target coordinate

	//public:

	//	//friend class boost::serialization::access;
	//	template < typename Archive >
	//	friend void serialize(Archive &ar, std::uint32_t const version) {
	//		ar & ct;
	//	}
	//	template < typename Archive > friend Archive& operator & (Archive& ar, this_t& B) {
	//		//ar & boost::serialization::base_object<base_t>(*this);
	//		ar & (base_t&)B;
	//		ar & B.m_scale;
	//		for (auto& v : B.m_mat.val)
	//			ar & v;
	//		ar & B.m_origin & B.m_offset;
	//		return ar;
	//	}
	//	template < typename JSON > friend void from_json(JSON const& j, this_t& B) {
	//		j["scale"] = B.m_scale;
	//		if constexpr (dim == 2) {
	//			j["mat00"] = B.m_mat(0, 0); j["mat01"] = B.m_mat(0, 1);
	//			j["mat10"] = B.m_mat(1, 0); j["mat10"] = B.m_mat(1, 1);
	//		} else {
	//			j["mat00"] = B.m_mat(0, 0); j["mat01"] = B.m_mat(0, 1); j["mat02"] = B.m_mat(0, 2);
	//			j["mat10"] = B.m_mat(1, 0); j["mat11"] = B.m_mat(1, 1); j["mat12"] = B.m_mat(1, 2);
	//			j["mat20"] = B.m_mat(2, 0); j["mat21"] = B.m_mat(2, 1); j["mat22"] = B.m_mat(2, 2);
	//		}
	//		j["origin"] = B.m_origin;
	//		j["offset"] = B.m_offset;
	//	}
	//	template < typename JSON > friend void to_json(JSON& j, this_t const& B) {
	//		B.m_scale = j["scale"];
	//		if constexpr (dim == 2) {
	//			B.m_mat(0, 0) = j["mat00"]; B.m_mat(0, 1) = j["mat01"];
	//			B.m_mat(1, 0) = j["mat10"]; B.m_mat(1, 1) = j["mat10"];
	//		} else {
	//			B.m_mat(0, 0) = j["mat00"]; B.m_mat(0, 1) = j["mat01"]; B.m_mat(0, 2) = j["mat02"];
	//			B.m_mat(1, 0) = j["mat10"]; B.m_mat(1, 1) = j["mat11"]; B.m_mat(1, 2) = j["mat12"];
	//			B.m_mat(2, 0) = j["mat20"]; B.m_mat(2, 1) = j["mat21"]; B.m_mat(2, 2) = j["mat22"];
	//		}
	//		B.m_origin = j["origin"];
	//		B.m_offset = j["offset"];
	//	}

	//	GTL__DYNAMIC_VIRTUAL_DERIVED(TCoordTransDim);

	//public:
	//	// Constructors
	//	TCoordTransDim(double scale = 1.0, mat_t const& m = mat_t::eye(), point_t const& origin = {}, point_t const& offset = {}) :
	//		m_scale{scale},
	//		m_mat(m),
	//		m_origin(origin),
	//		m_offset(offset)
	//	{
	//	}

	//	TCoordTransDim(TCoordTransDim const& B) = default;
	//	TCoordTransDim& operator = (TCoordTransDim const& B) = default;

	//	auto operator <=> (TCoordTransDim const&) const = default;

	//	//bool operator == (const TCoordTransDim& B) const { return (m_scale == B.m_scale) && (m_mat == B.m_mat) && (m_origin == B.m_origin) && (m_offset == B.m_offset); }
	//	//bool operator != (const TCoordTransDim& B) const { return !(*this == B); }
	//	//-------------------------------------------------------------------------
	//	std::unique_ptr<ICoordTrans> GetInverse() const override {
	//		// Scale
	//		double scale = 1/m_scale;
	//		if (!std::isfinite(scale))
	//			return {};

	//		// Matrix
	//		bool bOK {};
	//		auto mat = m_mat.inv(0, &bOK);
	//		if (!bOK)
	//			return {};

	//		return std::make_unique<TCoordTransDim>(scale, mat, m_offset, m_origin);
	//	}
	//	bool GetInv(TCoordTransDim& ctI) const {
	//		// Scale
	//		double scale = 1/m_scale;
	//		if (!std::isfinite(scale))
	//			return false;

	//		// Matrix
	//		bool bOK {};
	//		auto mat = m_mat.inv(0, &bOK);
	//		if (!bOK)
	//			return false;

	//		ctI.Init(scale, mat, m_offset, m_origin);
	//		return true;
	//	}

	//	//-------------------------------------------------------------------------
	//	// Setting
	//	void Init(double scale = 1., mat_t const& m = mat_t::eye(), point_t const& origin = {}, point_t const& offset = {}) {
	//		m_scale = scale;
	//		m_mat = m;
	//		m_origin = origin;
	//		m_offset = offset;
	//	}
	//	void Init(double scale, rad_t angle, point_t const& origin = {}, point_t const& offset = {}) requires (dim == 2) {
	//		m_scale = scale;
	//		m_mat = GetRotatingMatrix(angle);
	//		m_origin = origin;
	//		m_offset = offset;
	//	}

	//	[[nodiscard]] bool SetFrom2Points(std::span<point_t const> ptsSource, std::span<point_t const> ptsTarget, bool bCalcScale = true, double dMinDeterminant = 0.0, bool bRightHanded = true) 
	//		requires (dim == 2)
	//	{
	//		if ( (ptsSource.size() < 2) or (ptsTarget.size() < 2) )
	//			return false;

	//		std::array<point_t, 3> ptsS{ptsSource[0], ptsSource[1]}, ptsT{ptsTarget[0], ptsTarget[1]};
	//		// 세번째 점은 각각, 0번 기준으로 1번을 90도 회전시킴
	//		ptsS[2].x = -(ptsS[1].y-ptsS[0].y) + ptsS[0].x;
	//		ptsS[2].y =  (ptsS[1].x-ptsS[0].x) + ptsS[0].y;
	//		if (bRightHanded) {
	//			ptsT[2].x = -(ptsT[1].y-ptsT[0].y) + ptsT[0].x;
	//			ptsT[2].y =  (ptsT[1].x-ptsT[0].x) + ptsT[0].y;
	//		}
	//		else {
	//			ptsT[2].x =  (ptsT[1].y-ptsT[0].y) + ptsT[0].x;
	//			ptsT[2].y = -(ptsT[1].x-ptsT[0].x) + ptsT[0].y;
	//		}

	//		return SetFrom3Points(ptsS, ptsT, bCalcScale, dMinDeterminant);
	//	}
	//	[[nodiscard]] bool SetFrom3Points(std::span<point_t const> ptsSource, std::span<point_t const> ptsTarget, bool bCalcScale = true, double dMinDeterminant = 0.0)
	//		requires (dim == 2)
	//	{
	//		if ( (ptsSource.size() < 3) or (ptsTarget.size() < 3) )
	//			return false;

	//		auto v1s = ptsSource[1] - ptsSource[0];
	//		auto v2s = ptsSource[2] - ptsSource[0];
	//		auto v1t = ptsTarget[1] - ptsTarget[0];
	//		auto v2t = ptsTarget[2] - ptsTarget[0];
	//		double dLenSource = v1s.GetLength();
	//		double dLenTarget = v1t.GetLength();

	//		mat_t matS { v1s.x, v2s.x, v1s.y, v2s.y };

	//		// Check.
	//		double d = cv::determinant(matS);
	//		if (fabs(d) <= dMinDeterminant)
	//			return false;

	//		mat_t matT { v1t.x, v2t.x, v1t.y, v2t.y };

	//		m_mat = matT * matS.inv();

	//		m_scale = fabs(cv::determinant(m_mat));
	//		m_mat /= m_scale;

	//		if (!bCalcScale)
	//			m_scale = 1.0;

	//		m_origin = ptsSource[0];
	//		m_offset = ptsTarget[0];

	//		return true;
	//	}
	//	[[nodiscard]] bool SetFrom3Points(std::span<point_t const> ptsSource, std::span<point_t const> ptsTarget, bool bCalcScale = true, double dMinDeterminant = 0.0)
	//		requires (dim == 3)
	//	{
	//		if ( (ptsSource.size() < 3) or (ptsTarget.size() < 3) )
	//			return false;

	//		std::array<point_t, 4> pts0{ptsSource[0], ptsSource[1], ptsSource[2]}, pts1{ptsTarget[0], ptsTarget[1], ptsTarget[2]};

	//		pts0[3] = (pts0[1] - pts0[0]) * (pts0[2]-pts0[0]) + pts0[0];
	//		pts1[3] = (pts1[1] - pts1[0]) * (pts1[2]-pts1[0]) + pts1[0];

	//		return SetFrom4Points(pts0, pts1, bCalcScale, dMinDeterminant);
	//	}
	//	[[nodiscard]] bool SetFrom4Points(std::span<point_t const> ptsSource, std::span<point_t const> ptsTarget, bool bCalcScale = true, double dMinDeterminant = 0.0)
	//		requires (dim == 3)
	//	{
	//		if ( (ptsSource.size() < 4) or (ptsTarget.size() < 4) )
	//			return false;

	//		auto& pts0 = ptsSource;
	//		auto& pts1 = ptsTarget;
	//		double dLenSource = pts0[0].Distance(pts0[1]);
	//		double dLenTarget = pts1[0].Distance(pts1[1]);

	//		if ( (dLenSource == 0.0) || (dLenTarget == 0.0) )
	//			return false;

	//		mat_t matS;
	//		for (int i = 0; i < matS.cols; i++) {
	//			matS(0, i) = pts0[i+1].x - pts0[0].x;
	//			matS(1, i) = pts0[i+1].y - pts0[0].y;
	//			matS(2, i) = pts0[i+1].z - pts0[0].z;
	//		}
	//		// Check.
	//		double d = cv::determinant(matS);
	//		if (fabs(d) <= dMinDeterminant)
	//			return false;

	//		mat_t matT;
	//		for (int i = 0; i < matT.cols; i++) {
	//			matT(0, i) = pts1[i+1].x - pts1[0].x;
	//			matT(1, i) = pts1[i+1].y - pts1[0].y;
	//			matT(2, i) = pts1[i+1].z - pts1[0].z;
	//		}

	//		m_mat = matT * matS.inv();

	//		m_scale = cv::determinant(m_mat);
	//		m_mat /= m_scale;

	//		if (!bCalcScale)
	//			m_scale = 1.0;

	//		m_origin = ptsSource[0];
	//		m_offset = ptsTarget[0];

	//		return true;
	//	}

	//	static inline [[nodiscard]] mat_t GetRotatingMatrix(rad_t angle) requires (dim == 2) {
	//		double c{cos(angle)}, s{sin(angle)};
	//		return mat_t(c, -s, s, c);
	//	}
	//	static inline [[nodiscard]] mat_t GetRotatingMatrixXY(rad_t angle) requires (dim >= 3) {
	//		double c{cos(angle)}, s{sin(angle)};
	//		return mat_t{c, -s, 0, s, c, 0, 0, 0, 1};
	//	}
	//	static inline [[nodiscard]] mat_t GetRotatingMatrixYZ(rad_t angle) requires (dim >= 3) {
	//		double c{cos(angle)}, s{sin(angle)};
	//		return mat_t{1, 0, 0, 0, c, -s, 0, s, c};
	//	}
	//	static inline [[nodiscard]] mat_t GetRotatingMatrixZX(rad_t angle) requires (dim >= 3) {
	//		double c{cos(angle)}, s{sin(angle)};
	//		return mat_t{c, 0, s, 0, 1, 0, -s, 0, c};
	//	}

	//	void SetMatrixRotational(rad_t angle) requires (dim == 2) {
	//		m_mat = GetRotatingMatrix(angle);
	//	}
	//	void RotateMatrix(rad_t angle) requires (dim == 2) {
	//		m_mat = GetRotatingMatrix(angle) * m_mat;
	//	}
	//	void Rotate(rad_t angle, point_t const& ptCenter) requires (dim == 2) {
	//		auto backup(*this);
	//		Init(1.0, angle, ptCenter, ptCenter);
	//		*this *= backup;
	//	}
	//	void FlipSourceX()										{ for (int i {}; i < m_mat.rows; i++) m_mat(i, 0) = -m_mat(i, 0); }
	//	void FlipSourceY()										{ for (int i {}; i < m_mat.rows; i++) m_mat(i, 1) = -m_mat(i, 1); }
	//	void FlipSourceZ()					requires (dim >= 3) { for (int i {}; i < m_mat.rows; i++) m_mat(i, 2) = -m_mat(i, 2); }
	//	void FlipSourceX(double x)								{ FlipSourceX(); m_origin.x = 2*x - m_origin.x; }
	//	void FlipSourceY(double y)								{ FlipSourceY(); m_origin.y = 2*y - m_origin.y; }
	//	void FlipSourceZ(double z)			requires (dim >= 3) { FlipSourceY(); m_origin.z = 2*z - m_origin.z; }

	//	void FlipSource()										{ m_mat = -m_mat; }
	//	void FlipSource(point_t pt)								{ m_mat = -m_mat; m_origin = 2*pt - m_origin; }

	//	void FlipMatX()											{ for (int i {}; i < m_mat.cols; i++) m_mat(0, i) = -m_mat(0, i); }
	//	void FlipMatY()											{ for (int i {}; i < m_mat.cols; i++) m_mat(1, i) = -m_mat(1, i); }
	//	void FlipMatZ()						requires (dim >= 3) { for (int i {}; i < m_mat.cols; i++) m_mat(2, i) = -m_mat(2, i); }
	//	void FlipTargetX(double x = {})							{ FlipMatX(); m_offset.x = 2*x - m_offset.x; }
	//	void FlipTargetY(double y = {})							{ FlipMatY(); m_offset.y = 2*y - m_offset.y; }
	//	void FlipTargetZ(double z = {})		requires (dim >= 3) { FlipMatZ(); m_offset.z = 2*z - m_offset.z; }
	//	void FlipTarget(point_t pt = {})						{ m_mat = -m_mat; m_offset = 2*pt - m_offset; }

	//	//-------------------------------------------------------------------------
	//	// Operation
	//	//
	//	virtual [[nodiscard]] point2_t Trans(point2_t const& pt) const override {
	//		if constexpr (dim == 2) {
	//			return m_scale * (m_mat * (pt-m_origin)) + m_offset;
	//		} else if constexpr (dim == 3) {
	//			return m_scale * (m_mat * (point_t(pt)-m_origin)) + m_offset;
	//		}
	//	}
	//	virtual [[nodiscard]] point3_t Trans(point3_t const& pt) const override {
	//		if constexpr (dim == 2) {
	//			point3_t ptNew(Trans((point2_t&)pt));
	//			ptNew.z = pt.z;
	//			return ptNew;
	//		} else if constexpr (dim == 3) {
	//			return m_scale * (m_mat * (pt-m_origin)) + m_offset;
	//		}
	//	}
	//	virtual [[nodiscard]] std::optional<point2_t> TransInverse(point2_t const& pt) const override {
	//		if (auto r = GetInverse(); r)
	//			return (*r)(pt);
	//		return {};
	//	}
	//	virtual [[nodiscard]] std::optional<point3_t> TransInverse(point3_t const& pt) const override {
	//		if (auto r = GetInverse(); r)
	//			return (*r)(pt);
	//		return {};
	//	}

	//	virtual [[nodiscard]] double Trans(double dLength) const override {
	//		point_t pt0 = Trans(point_t::All(0));
	//		point_t pt1 = Trans(point_t::All(1));
	//		double scale = pt0.Distance(pt1) / sqrt((double)dim);
	//		return scale * dLength;
	//	}
	//	virtual [[nodiscard]] std::optional<double> TransInverse(double dLength) const override {
	//		point2_t pt0 = Trans(point_t::All(0));
	//		point2_t pt1 = Trans(point_t::All(1));
	//		double scale = sqrt((double)dim) / pt0.Distance(pt1);
	//		return scale * dLength;
	//	}
	//	virtual [[nodiscard]] bool IsRightHanded() const override {
	//		return cv::determinant(m_mat) >= 0;
	//	}

	//	TCoordTransDim& operator *= (TCoordTransDim const& B) {
	//		// 순서 바꾸면 안됨.
	//		m_offset		= m_scale * m_mat * (B.m_offset - m_origin) + m_offset;

	//		m_origin		= B.m_origin;
	//		m_mat		= m_mat * B.m_mat;
	//		m_scale		= m_scale * B.m_scale;
	//		return *this;
	//	}
	//	[[nodiscard]] TCoordTransDim operator * (TCoordTransDim const& B) const {
	//		auto C(*this);
	//		return C *= B;
	//	}

	//};


	////template GTL__CLASS TCoordTransChain<double>;
	////using xCoordTransChain = TCoordTransChain<double>;

	////template TCoordTransDim<2>;
	//using xCoordTrans2d = TCoordTransDim<2>;

	////template TCoordTransDim<3>;
	//using xCoordTrans3d = TCoordTransDim<3>;


	//GTL__CLASS xCoordTrans2d;
	//GTL__CLASS xCoordTrans3d;

#pragma pack(pop)
}	// namespace gtl


#endif
