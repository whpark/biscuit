module;

#include "glm/glm.hpp"
#include "biscuit/macro.h"

export module biscuit.coord.point;
import std;
import biscuit.aliases;
import biscuit.concepts;
import biscuit.coord.concepts;
import biscuit.coord.base;

export namespace biscuit {

	//template < typename T, int DIMENSION, bool bROUND > requires (std::is_arithmetic_v<T>)
	//using TPoint = coord::TCoordBase<coord::TPoint_, T, DIMENSION, bROUND>;

	template < typename T, int DIMENSION, bool bROUND >
		requires (std::is_arithmetic_v<T>)
	struct TPoint : coord::TCoordBase<coord::TPoint_, T, DIMENSION, bROUND> {
		using base_t = coord::TCoordBase<coord::TPoint_, T, DIMENSION, bROUND>;
		using this_t = TPoint<T, DIMENSION, bROUND>;
		using coord_type_t = base_t::coord_type_t;
		using array_t = base_t::array_t;
		using value_t = base_t::value_t;

		using base_t::base_t;
		using base_t::operator =;
		//using base_t::operator <=>;

		using base_t::operator +=;
		using base_t::operator -=;
		using base_t::operator *=;
		using base_t::operator /=;
		using base_t::operator +;
		using base_t::operator -;
		using base_t::operator *;

	public:
		using base_t::dim;
		using base_t::count;
		using base_t::arr;
		using base_t::x;
		using base_t::y;
		//using base_t::z requires (base_t::dim >= 3);
		// Math
		BSC__NODISCARD double Distance(this_t const& pt) const {
			double sum {};
			for (int i{}; i < count(); i++)
				sum += Square(arr()[i]-pt[i]);
			return std::sqrt(sum);
		}
		BSC__NODISCARD double GetLength() const {
			double sum {};
			for (auto v : arr())
				sum += Square(v);
			return std::sqrt(sum);
		}
		BSC__NODISCARD rad_t GetAngleXY() const { return rad_t(std::atan2(this->y, this->x)); }
		BSC__NODISCARD rad_t GetAngleYZ() const requires (dim >= 3) { return rad_t(std::atan2(this->z, this->y)); }
		BSC__NODISCARD rad_t GetAngleZX() const requires (dim >= 3) { return rad_t(std::atan2(this->x, this->z)); }

		BSC__NODISCARD this_t GetNormalizedVector() const { return *this / GetLength(); }	// Length == 1.0
		bool Normalize() { *this /= GetLength(); return base_t::IsAllValid(); }

		BSC__NODISCARD this_t GetNormalVectorXY() const { return {this->y, -this->x}; }								// Perpendicular(Normal) Vector (XY-Plan)
		BSC__NODISCARD this_t GetNormalVectorYZ() const requires (dim >= 3) { return {{}, this->z, -this->y}; }		// Perpendicular(Normal) Vector (YZ-Plan)
		BSC__NODISCARD this_t GetNormalVectorZX() const requires (dim >= 3) { return {-this->z, {}, this->x}; }		// Perpendicular(Normal) Vector (ZX-Plan)
		// Cross
		BSC__NODISCARD this_t operator * (this_t const& B) const						{ return Cross(B); }

		//// Mathmatical Operator
		//template < typename T2 >
		//friend this_t operator * (cv::Matx<T2, dim, dim> const& A, this_t const& B) {
		//	this_t C{};
		//	//C.x = A(0, 0) * B.x + A(0, 1) * B.y + A(0, 2) * B.z;
		//	//C.y = A(1, 0) * B.x + A(1, 1) * B.y + A(1, 2) * B.z;
		//	//C.z = A(2, 0) * B.x + A(2, 1) * B.y + A(2, 2) * B.z;
		//	for (int row {}; row < dim; row++) {
		//		for (int col {}; col < dim; col++) {
		//			C.data()[row] += A(row, col) * B.data()[col];
		//		}
		//	}
		//	return C;
		//}

		BSC__NODISCARD this_t Cross(this_t const& B) const requires (dim >= 3) {
			using base_t::z;
			return {y*B.z - z*B.y, z*B.x - x*B.z, x*B.y - y*B.x};
		}
		BSC__NODISCARD T CrossZ(this_t const& B) const {
			return this->x*B.y - this->y*B.x;
		}
		BSC__NODISCARD T Dot(this_t const& B) const {
			T sum{};
			for (size_t i{}; i < count(); i++)
				sum += arr()[i] * B[i];
			return sum;
		}
		BSC__NODISCARD this_t Mul(this_t const& B) const {
			this_t C(*this);
			for (size_t i{}; i < count(); i++)
				C[i] *= B[i];
			return C;
		}

		//-------------------------------------------------------------------------------------------------------------------------
		// glm
		BSC__NODISCARD glm::vec<dim, value_t>&		vec()								{ return *reinterpret_cast<glm::vec<dim, value_t>*>(this); }
		BSC__NODISCARD glm::vec<dim, value_t>const&	vec() const							{ return *reinterpret_cast<glm::vec<dim, value_t>const*>(this); }
		BSC__NODISCARD glm::vec<2, value_t>&		vec2()			requires (dim >= 2) { return *reinterpret_cast<glm::vec<2, value_t>*>(this); }
		BSC__NODISCARD glm::vec<2, value_t>const&	vec2() const	requires (dim >= 2) { return *reinterpret_cast<glm::vec<2, value_t>const*>(this); }
		BSC__NODISCARD glm::vec<3, value_t>&		vec3()			requires (dim >= 3) { return *reinterpret_cast<glm::vec<3, value_t>*>(this); }
		BSC__NODISCARD glm::vec<3, value_t>const&	vec3() const	requires (dim >= 3) { return *reinterpret_cast<glm::vec<3, value_t>const*>(this); }
		BSC__NODISCARD glm::vec<4, value_t>&		vec4()			requires (dim >= 4) { return *reinterpret_cast<glm::vec<4, value_t>*>(this); }
		BSC__NODISCARD glm::vec<4, value_t>const&	vec4() const	requires (dim >= 4) { return *reinterpret_cast<glm::vec<4, value_t>const*>(this); }

		operator glm::vec<dim, value_t>&		()			{ return vec(); }
		operator glm::vec<dim, value_t> const&	() const	{ return vec(); }


	};

	using sPoint2i = TPoint<int, 2, false>;
	using sPoint3i = TPoint<int, 3, false>;
	using sPoint4i = TPoint<int, 4, false>;
	using sPoint2d = TPoint<double, 2, false>;
	using sPoint3d = TPoint<double, 3, false>;
	using sPoint4d = TPoint<double, 4, false>;
	using xPoint2i = TPoint<int, 2, true>;
	using xPoint3i = TPoint<int, 3, true>;
	using xPoint4i = TPoint<int, 4, true>;
	using xPoint2d = TPoint<double, 2, true>;
	using xPoint3d = TPoint<double, 3, true>;
	using xPoint4d = TPoint<double, 4, true>;
	
	static_assert(requires(sPoint2d ) { sPoint2d{1.0, 2.0}; });
	static_assert(requires(sPoint3d ) { sPoint3d{1.0, 2.0, 3.0}; });
	static_assert(requires(sPoint4d ) { sPoint4d{1.0, 2.0, 3.0, 4.0}; });
	static_assert(sizeof(sPoint2d) == sizeof(double)*2);
	static_assert(sizeof(sPoint3d) == sizeof(double)*3);
	static_assert(sizeof(sPoint4d) == sizeof(double)*4);
	static_assert(std::is_trivially_copyable_v<sPoint2d>);
	static_assert(std::is_constructible_v<sPoint2d, double, double>);
	static_assert(std::is_constructible_v<sPoint2d, int, int>);
	static_assert(!std::is_constructible_v<sPoint2d, double, double, double>);
	static_assert(std::is_constructible_v<sPoint2d, double>);
	static_assert(!std::is_constructible_v<sPoint2d, int, int, int>);
	static_assert(std::is_constructible_v<sPoint2d, int>);

	static_assert(std::is_constructible_v<sPoint2i, double, double>);

	static_assert(concepts::coord::has_point2<sPoint2i>);

}
