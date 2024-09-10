#include <cstddef>
#include <catch.hpp>
#include <fmt/core.h>

#include "biscuit/biscuit.h"

//#pragma warning(push)
#pragma warning(disable: 4127)	// conditional expression is constant
#include "units.h"
//#pragma warning(pop)

import std;
import biscuit;

using namespace std::literals;
//using namespace units;
//using namespace units::angle;
using namespace units::literals;

namespace test {
	using namespace biscuit;

	struct sTestAlign {
		sPoint2d pt;
		sPoint2i pt2;
		char a;
		sPoint2d pt3;
	};
	static_assert(offsetof(sTestAlign, pt) == 0);
	static_assert(offsetof(sTestAlign, pt2) == 16);
	static_assert(offsetof(sTestAlign, pt3) == 32);
	TEST_CASE("eigen_transform") {
		Eigen::Transform<double, 2, Eigen::Affine> t;
		t.setIdentity();
		t.translate(Eigen::Vector2d(1, 2));
		t.rotate(30);
		t.scale(Eigen::Vector2d(2, 3));

		Eigen::Matrix3d m = t.matrix();
		m.transposeInPlace();


		std::vector<int> lst { 1, 2, 3, 4, 5 };
		auto str = std::format("{:n}", lst);
		REQUIRE(str == "1, 2, 3, 4, 5");
	}

}

namespace test {
	using namespace biscuit;

	struct Point2i {
		int x, y;
		auto operator <=> (Point2i const&) const = default;
	};
	struct Point3d {
		double x, y, z;
		auto operator <=> (Point3d const&) const = default;
	};
	struct Size2i {
		int width, height;
	};
	struct Rect2d {
		double x, y, width, height;
		auto operator <=> (Rect2d const&) const = default;
	};
	struct Rect3d {
		double x, y, z, width, height, depth;
		auto operator <=> (Rect3d const&) const = default;
	};


	template < typename T >
	struct QPoint_ {
		T m_x, m_y;
		auto x() const { return m_x; }
		auto y() const { return m_y; }
		void setX(T x) { m_x = x; }
		void setY(T y) { m_y = y; }
	};
	template < typename T >
	struct QSize_ {
		T m_width, m_height;
		auto width() const { return m_width; }
		auto height() const { return m_height; }
		auto setWidth(T width) { m_width = width; }
		auto setHeight(T height) { m_height = height; }
	};

	TEST_CASE("coord") {

		biscuit::sRect2i rect2i{0, 0, 1, 2};
		biscuit::sPoint2i pt2i{100, 200};
		biscuit::sSize2i size2i;
		size2i = pt2i;

		REQUIRE(size2i.width == 100);
		REQUIRE(size2i.height == 200);

		rect2i += 2;
		REQUIRE(rect2i.x == 2);
		REQUIRE(rect2i.y == 2);
		REQUIRE(rect2i.width == 3);
		REQUIRE(rect2i.height == 4);

		rect2i = -rect2i;
		REQUIRE(rect2i.x == -2);
		REQUIRE(rect2i.y == -2);
		REQUIRE(rect2i.width == -3);
		REQUIRE(rect2i.height == -4);

		rect2i *= -1;
		REQUIRE(rect2i.x == 2);
		REQUIRE(rect2i.y == 2);
		REQUIRE(rect2i.width == 3);
		REQUIRE(rect2i.height == 4);

		REQUIRE(rect2i + 2 == 2 + rect2i);
		REQUIRE(rect2i + 3 != 2 + rect2i);
		REQUIRE(rect2i + 2 != 3 + rect2i);
		REQUIRE(rect2i + 3 > 2 + rect2i);
		REQUIRE(rect2i + 2 < 3 + rect2i);
		REQUIRE(rect2i - 2 == -2 + rect2i);
		REQUIRE(-rect2i + 2 == 2 - rect2i);

		REQUIRE(sPoint2i{150, 260} <= sPoint2i{150, 260});
		REQUIRE(!(sPoint2i{150, 260} < sPoint2i{150, 260}));
		REQUIRE(!(sPoint2i{150, 260} > sPoint2i{150, 260}));
		REQUIRE(!(sPoint2i{150, 260} != sPoint2i{150, 260}));
		REQUIRE(sPoint2i{150, 260} == sPoint2i{150, 260});

		Point2i cvPt = rect2i;
		Size2i cvSize = rect2i;
		REQUIRE(biscuit::sPoint2i(cvPt) == rect2i.pt());
		REQUIRE(biscuit::sSize2i(cvSize) == rect2i.size());

		QPoint_<int> qPt = rect2i;
		QSize_<int> qSize = rect2i;
		REQUIRE(biscuit::sPoint2i(qPt) == rect2i.pt());
		REQUIRE(biscuit::sSize2i(qSize) == rect2i.size());

		biscuit::sPoint3d pt3d{ 0.0, 2.0, 3.0 };
		auto angle = pt3d.GetAngleXY();
		REQUIRE(angle == 90_deg);
	}

	TEST_CASE("coord.round") {
		sPoint3rd pt(1.5, 2.5, 3.5);
		sPoint3i pti(pt);
		REQUIRE(pti == sPoint3i(1, 2, 3));
		sPoint3ri ptx(pt);
		REQUIRE(ptx == sPoint3ri(2, 3, 4));
	}

	TEST_CASE("coord.rect") {

		sRect2i rc2i(100, 200, 50, 60);

		REQUIRE(rc2i.Contains(sPoint2i(100, 200)));
		REQUIRE(!rc2i.Contains(sPoint2i(150, 260)));
		REQUIRE(rc2i.Contains(sRect2i(100, 200, 50, 60)));
		REQUIRE(!rc2i.Contains(sRect2i(100, 200, 51, 60)));
		REQUIRE(!rc2i.Contains(sRect2i(100, 200, 50, 61)));
		REQUIRE(!rc2i.Contains(sRect2i( 99, 200, 50, 60)));
		REQUIRE(!rc2i.Contains(sRect2i(100, 199, 50, 60)));
		REQUIRE(rc2i.Contains(sRect2i(110, 210, 0, 0)));
		REQUIRE(rc2i.Contains(sRect2i(110, 210, 0, 0)));
		REQUIRE(sRect2i(10, 30, 0, 0).IsEmpty());
		REQUIRE(sRect2i(10, 30, 0, 0).IsEmpty());
		REQUIRE(sRect2i().IsEmpty());
		REQUIRE(sRect2i().IsNull());
	}

	TEST_CASE("coord.from_to_string") {

		sRect2d rect{1., 2., 3., 4.};
		auto str = rect.ToString();
		sRect2d rect2;
		rect2.FromString(str);
		REQUIRE(rect == rect2);

		auto pt = sPoint2i{ 1, 2 };
		str = ToString(pt);
		sPoint2i pt2;
		pt2.FromString(str);
		REQUIRE(pt == pt2);

		auto pt3 = sPoint3d{ 1., 2., 3. };
		str = biscuit::ToString(pt3, "{:.3f}"sv);
		sPoint3d pt4;
		pt4.FromString(str);
		REQUIRE(pt3 == pt4);
	}

	TEST_CASE("generic_coord.from_to_string") {

		Rect2d rect{1., 2., 3., 4.};
		auto str = ToString<char>(rect);

		auto rect2 = FromString<Rect2d>(str);
		REQUIRE(rect == rect2);

		auto pt = Point2i{ 1, 2 };
		str = ToString(pt);
		auto pt2 = FromString<Point2i>(str);
		REQUIRE(pt == pt2);

		auto pt3 = Point3d{ 1., 2., 3. };
		str = biscuit::ToString(pt3, "{:.3f}"sv);
		auto pt4 = FromString<Point3d>(str);
		REQUIRE(pt3 == pt4);


	}

}

namespace bench {
	struct sRect {
		int x, y, width, height;

		auto operator <=> (sRect const&) const = default;

		inline void Normalize() {
			if (width < 0) { x = x+width; width = -width; }
			if (height < 0) { y = y+height; height = -height; }
		}

		sRect& IntersectSafe(sRect const& b) {
			auto pax0 = std::min(x, x+width);
			auto pay0 = std::min(y, y+height);
			auto pax1 = std::max(x, x+width);
			auto pay1 = std::max(y, y+height);

			auto pbx0 = std::min(b.x, b.x+b.width);
			auto pby0 = std::min(b.y, b.y+b.height);
			auto pbx1 = std::max(b.x, b.x+b.width);
			auto pby1 = std::max(b.y, b.y+b.height);

			x = std::max(pax0, pbx0);
			y = std::max(pay0, pby0);
			width = std::min(pax1, pbx1) - x;
			height = std::min(pay1, pby1) - y;

			return *this;
		}

		sRect& Intersect(sRect const& b) {
			width = std::min(x+width, b.x+b.width);
			height = std::min(y+height, b.y+b.height);
			x = std::max(x, b.x);
			y = std::max(y, b.y);
			width -= x;
			height -= y;
			return *this;
		}
	};

	struct xRect {
		int l, t, r, b;

		auto operator <=> (xRect const&) const = default;

		inline void Normalize() {
			if (l > r) std::swap(l, r);
			if (t > b) std::swap(t, b);
		}

		xRect& IntersectSafe0(xRect const& B) {
			xRect result;
			result.l = std::max(std::min(l, r), std::min(B.l, B.r));
			result.r = std::min(std::max(l, r), std::max(B.l, B.r));
			result.t = std::max(std::min(t, b), std::min(B.t, B.b));
			result.b = std::min(std::max(t, b), std::max(B.t, B.b));
			*this = result;
			return *this;
		}
		xRect& IntersectSafe(xRect& B) {
			if (l > r) std::swap(l, r);
			if (t > b) std::swap(t, b);
			if (B.l > B.r) std::swap(B.l, B.r);
			if (B.t > B.b) std::swap(B.t, B.b);
			return Intersect(B);
		}
		xRect& Intersect(xRect const& B) {
			l = std::max(l, B.l);
			r = std::min(r, B.r);
			t = std::max(t, B.b);
			b = std::min(t, B.t);
			return *this;
		}
	};

#if 0
	TEST_CASE("coord.rect.bench") {
		//sRect rc0 { 30, 40, -30, -40 };
		//sRect rc1 { 20, 40, -10, -20 };
		//xRect rc2 { 30, 40, 0, 0 };
		//xRect rc3 { 20, 40, 10, 20 };
		sRect rc0 { rand(), rand(), rand(), rand()};
		sRect rc1 { rand(), rand(), rand(), rand()};
		sRect rc2 { rand(), rand(), rand(), rand()};
		sRect rc3 { rand(), rand(), rand(), rand()};

		BENCHMARK("sRect::IntersectSafe") {
			rc0.IntersectSafe(rc1);
		};

		BENCHMARK("sRect::Intersect") {
			rc0.Normalize();
			rc1.Normalize();
			rc0.Intersect(rc1);
		};

		BENCHMARK("sRect::Intersect2") {
			rc0.Intersect(rc1);
		};

		BENCHMARK("xRect::IntersectSafe") {
			rc2.IntersectSafe(rc3);
		};

		BENCHMARK("xRect::Intersect") {
			rc2.Normalize();
			rc2.Normalize();
			rc2.Intersect(rc3);
		};

		BENCHMARK("xRect::Intersect2") {
			rc2.Intersect(rc3);
		};
	}
#endif


}
