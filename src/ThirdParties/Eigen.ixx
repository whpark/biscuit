module;

//#define _SILENCE_ALL_CXX23_DEPRECATION_WARNINGS

export module Eigen;
import std;

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable: 4127 4996 5054)
#endif

export import <Eigen/Core>;
export import <Eigen/Geometry>;

#ifdef _MSC_VER
#	pragma warning(pop)
#endif

Eigen::Vector3d ff;
void Func() {
	ff.x() = 1.0;
}
