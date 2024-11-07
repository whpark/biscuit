#include <catch.hpp>

#include "biscuit/dependencies_fmt.h"

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
		//L"/DXF/sample_ascii.dxf"s,
		L"/DXF/sample_bin_r12.dxf"s,
		L"/DXF/sample_bin_2010.dxf"s,
	};

	for (auto const& path : paths) {
		if (auto records = biscuit::dxf::ReadRecords(path)) {
			auto path_out = path;
			path_out.replace_extension(".txt");
			std::ofstream out(path_out, std::ios::binary);
			for (auto const& r : records.value()) {
				std::visit([&](auto const& v) {
					if constexpr (std::is_floating_point_v<std::remove_cvref_t<decltype(v)>>) {
						fmt::println(out, "{}:{:f}", r.iGroupCode, v);
					}
					else {
						fmt::println(out, "{}:{}", r.iGroupCode, v);
					}
				}, r.value);
			}
		}
		else {
			fmt::print("ReadRecords: failed to read {}\n", path);
		}

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

