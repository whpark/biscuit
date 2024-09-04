#include <catch.hpp>
#include <fmt/core.h>

#pragma warning(push)
#pragma warning(disable: 4127)	// conditional expression is constant
#include "units.h"
#pragma warning(pop)

#include "biscuit/biscuit.h"

import std;
import biscuit;

using namespace std::literals;
//using namespace units;
//using namespace units::angle;
using namespace units::literals;

namespace test {

	struct Point2i {
		int x, y;
	};
	struct Size2i {
		int width, height;
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
		//using namespace biscuit;

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
		REQUIRE(rect2i + 3 > 2 + rect2i);
		REQUIRE(rect2i + 2 < 3 + rect2i);
		REQUIRE(rect2i - 2 == -2 + rect2i);
		REQUIRE(-rect2i + 2 == 2 - rect2i);

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


}
