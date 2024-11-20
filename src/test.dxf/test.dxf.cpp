#include <catch.hpp>

#include <magic_enum.hpp>
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
		auto ok = dxf.ReadDXF(path);
		{
			auto path_out = path;
			path_out.replace_extension(".values.txt");
			std::ofstream out(path_out, std::ios::binary);
			for (auto const& group : dxf.GetGroups()) {
				std::visit([&](auto const& v) {
					if constexpr (std::is_floating_point_v<std::remove_cvref_t<decltype(v)>>) {
						fmt::println(out, "{:>4}:{}", group.eCode, biscuit::ToChars(v));
					}
					else {
						fmt::println(out, "{:>4}:{}", group.eCode, v);
					}
				}, group.value);
			}
		}
		if (!ok) {
			fmt::println("!!! ReadDXF: failed to read {}", path);
			continue;
		}
		fmt::println("DXF({}) Read", path);
		auto const& groups = dxf.GetGroups();
		auto path_out = path;
		path_out.replace_extension(".txt");
		std::ofstream out(path_out, std::ios::binary);

		// variables
		for (auto const& [key, groups] : dxf.m_mapVariables) {
			fmt::println(out, "key:{}", key);
			for (auto const& group : groups) {
				std::visit([&](auto const& v) {
					if constexpr (std::is_floating_point_v<std::remove_cvref_t<decltype(v)>>) {
						fmt::println(out, "{:>4}:{}", group.eCode, biscuit::ToChars(v));
					}
					else {
						fmt::println(out, "{:>4}:{}", group.eCode, v);
					}
				}, group.value);
			}
		}

		// classes
		for (auto const& c : dxf.m_classes) {
			fmt::println(out, "class:{}", c.name);
			fmt::println(out, "\tcpp_class_name:{}", c.cpp_class_name);
			fmt::println(out, "\tapp_name:{}", c.app_name);
		}

		// tables
		for (auto const& t : dxf.m_tables) {
			fmt::println(out, "table:{}", t.table_type);
			fmt::println(out, "\thandle:{}", t.handle);
			fmt::println(out, "\tclass:{}", t.class_name);
			fmt::println(out, "\tmax_entries:{}", t.max_entries);
		}

		// entities
		for (auto const& e : dxf.m_entities) {
			fmt::println(out, "entity:{}", magic_enum::enum_name(e->GetEntityType()));
			if (e->GetEntityType() == biscuit::dxf::entities::eENTITY::unknown) {
				if (auto* entity = dynamic_cast<biscuit::dxf::entities::xUnknown*>(e.get()))
					fmt::println(out, "\tname:{}", entity->m_name);
			}
			fmt::println(out, "\tlayer:{}", e->layer);
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
		map[v] = (int)v/10;

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

