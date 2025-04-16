#pragma once
//module;

#include "biscuit/dependencies_glaze.h"

import std;
import "biscuit/dependencies_opencv.hxx";
import biscuit;

/*export */namespace biscuit {

	//template <typename T>
	//T& ValueOr(glz::json_t const& j, T const& def) {
	//	if (auto* v = j.get_if<T>()) {
	//		return *v;
	//	}
	//	return def;
	//};

	// for cv::Mat
	namespace detail {
		struct cvMat {
			int rows{};
			int cols{};
			int type{};
			std::vector<std::string> data{};

			void SetFrom(cv::Mat m) {
				rows = m.rows;
				cols = m.cols;
				type = m.type();
				data.reserve(m.rows);
				size_t const step = m.step;
				for (int y{}; y < m.rows; y++) {
					auto* ptr = m.ptr(y);
					// ptr to base64
					data.emplace_back(ToBase64(std::span(ptr, step)));
				}
			}

			operator cv::Mat() const {
				cv::Mat m = cv::Mat::zeros(rows, cols, type);
				size_t const step = m.step;
				for (int y{}; y < m.rows; y++) {
					// base64 to ptr
					auto [r, v] = FromBase64(data[y]);
					if (!r.error) {
						auto* ptr = m.ptr(y);
						memcpy(ptr, v.data(), step);
					}
				}
				return m;
			}

		};
	}

}	// namespace biscuit;


namespace glz::detail {
	// cv::Mat
	template <>
	struct from<JSON, cv::Mat> {
		template <auto Opts>
		static void op(cv::Mat& mat, auto&&... args) {
			biscuit::detail::cvMat cvmat;
			read<JSON>::op<Opts>(cvmat, args...);
			mat = cvmat;
		}
	};
	template <>
	struct to<JSON, cv::Mat> {
		template <auto Opts>
		static void op(cv::Mat const& mat, auto&&... args) noexcept {
			biscuit::detail::cvMat cvmat(mat);
			write<JSON>::op<Opts>(cvmat, args...);
		}
	};

	// cv::Matx
	template <typename _Tp, int m, int n>
	struct from<JSON, cv::Matx<_Tp, m, n>> {
		template <auto Opts>
		static void op(cv::Matx<_Tp, m, n>& mat, auto&&... args) {
			read<JSON>::op<Opts>(static_cast(std::array<_Tp, m*n>&)mat.val, args...);
		}
	};
	template <typename _Tp, int m, int n>
	struct to<JSON, cv::Matx<_Tp, m, n>> {
		template <auto Opts>
		static void op(cv::Matx<_Tp, m, n> const& mat, auto&&... args) noexcept {
			write<JSON>::op<Opts>(static_cast(std::array<_Tp, m*n> const&)mat.val, args...);
		}
	};

	// cv::Vec
	template <typename _Tp, int cn>
	struct from<JSON, cv::Vec<_Tp, cn>> {
		template <auto Opts>
		static void op(cv::Vec<_Tp, cn>& vec, auto&&... args) {
			read<JSON>::op<Opts>(static_cast(std::array<_Tp, cn>&)vec.val, args...);
		}
	};
	template <typename _Tp, int cn>
	struct to<JSON, cv::Vec<_Tp, cn>> {
		template <auto Opts>
		static void op(cv::Vec<_Tp, cn> const& vec, auto&&... args) noexcept {
			write<JSON>::op<Opts>(static_cast(std::array<_Tp, cn> const&)vec.val, args...);
		}
	};

	// cv::Scalar_
	template <typename _Tp>
	struct from<JSON, cv::Scalar_<_Tp>> {
		template <auto Opts>
		static void op(cv::Scalar_<_Tp>& scalar, auto&&... args) {
			read<JSON>::op<Opts>(static_cast(std::array<_Tp, sizeof(scalar.val) / sizeof(_Tp)>&)scalar.val, args...);
		}
	};
	template <typename _Tp>
	struct to<JSON, cv::Scalar_<_Tp>> {
		template <auto Opts>
		static void op(cv::Scalar_<_Tp> const& scalar, auto&&... args) noexcept {
			write<JSON>::op<Opts>(static_cast(std::array<_Tp, sizeof(scalar.val) / sizeof(_Tp)>&)scalar.val, args...);
		}
	};

	// cv::Point_
	template <typename _Tp>
	struct from<JSON, cv::Point_<_Tp>> {
		template <auto Opts>
		static void op(cv::Point_<_Tp>& point, auto&&... args) {
			read<JSON>::op<Opts>(static_cast(std::array<_Tp, sizeof(point.val) / sizeof(_Tp)>&)point.val, args...);
		}
	};
	template <typename _Tp>
	struct to<JSON, cv::Point_<_Tp>) {
		template <auto Opts>
		static void op(cv::Point_<_Tp> const& point, auto&&... args) noexcept {
			write<JSON>::op<Opts>(static_cast(std::array<_Tp, sizeof(point.val) / sizeof(_Tp)>&)point.val, args...);
		}
	};

	// cv::Point3_
	template <typename _Tp>
	struct from<JSON, cv::Point3_<_Tp>> {
		template <auto Opts>
		static void op(cv::Point3_<_Tp>& point, auto&&... args) {
			read<JSON>::op<Opts>(static_cast(std::array<_Tp, sizeof(point.val) / sizeof(_Tp)>&)point.val, args...);
		}
	};
	template <typename _Tp>
	struct to<JSON, cv::Point3_<_Tp>) {
		template <auto Opts>
		static void op(cv::Point3_<_Tp> const& point, auto&&... args) noexcept {
			write<JSON>::op<Opts>(static_cast(std::array<_Tp, sizeof(point.val) / sizeof(_Tp)>&)point.val, args...);
		}
	};

	// cv::Size_
	template <typename _Tp>
	struct from<JSON, cv::Size_<_Tp>> {
		template <auto Opts>
		static void op(cv::Size_<_Tp>& size, auto&&... args) {
			read<JSON>::op<Opts>(static_cast(std::array<_Tp, sizeof(size.val) / sizeof(_Tp)>&)size.val, args...);
		}
	};
	template <typename _Tp>
	struct to<JSON, cv::Size_<_Tp>) {
		template <auto Opts>
		static void op(cv::Size_<_Tp> const& size, auto&&... args) noexcept {
			write<JSON>::op<Opts>(static_cast(std::array<_Tp, sizeof(size.val) / sizeof(_Tp)>&)size.val, args...);
		}
	};

	// cv::Rect_
	template <typename _Tp>
	struct from<JSON, cv::Rect_<_Tp>> {
		template <auto Opts>
		static void op(cv::Rect_<_Tp>& rect, auto&&... args) {
			read<JSON>::op<Opts>(static_cast(std::array<_Tp, sizeof(rect.val) / sizeof(_Tp)>&)rect.val, args...);
		}
	};
	template <typename _Tp>
	struct to<JSON, cv::Rect_<_Tp>) {
		template <auto Opts>
		static void op(cv::Rect_<_Tp> const& rect, auto&&... args) noexcept {
			write<JSON>::op<Opts>(static_cast(std::array<_Tp, sizeof(rect.val) / sizeof(_Tp)>&)rect.val, args...);
		}
	};

};

