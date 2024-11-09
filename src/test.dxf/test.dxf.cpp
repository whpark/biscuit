#include <catch.hpp>

#include "biscuit/dependencies_fmt.h"
//#include "biscuit/dependencies_glaze.h"

import std;
import biscuit;
import biscuit.dxf;

using namespace std::literals;
using namespace biscuit::literals;

TEST_CASE("Test biscuit.dxf") {
	fmt::println("start");
	SECTION("Test biscuit.dxf") {
	}

	std::vector<std::filesystem::path> paths = {
		L"/DXF/sample_ascii.dxf"s,
		//L"/DXF/sample_bin_r12.dxf"s,
		//L"/DXF/sample_bin_2010.dxf"s,
	};

	//biscuit::dxf::sClass s;
	//auto str = glz::write_json(s);

	for (auto const& path : paths) {
		biscuit::dxf::xDXF dxf;
		if (!dxf.ReadDXF(path)) {
			fmt::print("ReadDXF: failed to read {}\n", path);
			continue;
		}
		auto const& groups = dxf.GetGroups();
		auto path_out = path;
		path_out.replace_extension(".txt");
		std::ofstream out(path_out, std::ios::binary);

		// variables
		for (auto const& [key, groups] : dxf.m_mapVariables) {
			fmt::println(out, "key:{}", key);
			for (auto const& group : groups) {
				fmt::println(out, "\t{}:{}", group.iGroupCode, group.value);
			}
		}

		// classes
		for (auto const& c : dxf.m_classes) {
			fmt::println(out, "class:{}", c.name);
			fmt::println(out, "\tstrCppClassName:{}", c.strCppClassName);
			fmt::println(out, "\tstrAppName:{}", c.strAppName);
		}
		//for (auto const& group : groups) {
		//	std::visit([&](auto const& v) {
		//		if constexpr (std::is_floating_point_v<std::remove_cvref_t<decltype(v)>>) {
		//			fmt::println(out, "{}:{:f}", group.iGroupCode, v);
		//		}
		//		else {
		//			fmt::println(out, "{}:{}", group.iGroupCode, v);
		//		}
		//	}, group.value);
		//}
	}

}

TEST_CASE("benchmark") {
	std::vector<int> values(10'000uz, 0);
	constexpr auto map_size = 2000uz;
	for (auto& v : values)
		v = rand() % map_size;
	std::vector<int> map;
	map.assign(map_size, 0);
	for (auto v : std::ranges::views::iota(0uz, map_size))
		map[v] = v/10;

	BENCHMARK("dividing") {
		int64_t r {};
		for (auto v : values)
			r += v / 10;
		return r;
	};

	BENCHMARK("map") {
		int64_t r {};
		for (auto v : values)
			r += map[v];
		return r;
	};

}

