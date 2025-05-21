#include <catch.hpp>

#include "biscuit/dependencies/glaze.h"

import std;
import "biscuit/dependencies/fmt.hxx";
import "biscuit/dependencies/ctre.hxx";
import "biscuit/dependencies/magic_enum.hxx";
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
		////L"/DXF/sample_bin_r12.dxf"s,
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
			fmt::println(out, "class:{}", c.name());
			fmt::println(out, "\tcpp_class_name:{}", c.cpp_class_name());
			fmt::println(out, "\tapp_name:{}", c.app_name());
		}

		// tables
		for (auto const& t : dxf.m_tables) {
			fmt::println(out, "table:{}", t.table_type());
			fmt::println(out, "\thandle:{}", t.handle());
			fmt::println(out, "\tclass:{}", t.class_name());
			fmt::println(out, "\tmax_entries:{}", t.max_entries());
		}

		// blocks
		for (auto const& b : dxf.m_blocks) {
			fmt::println(out, "block:{}", b.name());
			fmt::println(out, "\tlayer:{}", b.layer());
		}

		// entities
		for (auto const& e : dxf.m_entities) {
			fmt::println(out, "entity:{}", magic_enum::enum_name(e->GetEntityType()));
			if (e->GetEntityType() == biscuit::dxf::entities::eENTITY::unknown) {
				if (auto* entity = dynamic_cast<biscuit::dxf::entities::xUnknown*>(e.get()))
					fmt::println(out, "\tname:{}", entity->m_name);
			}
			fmt::println(out, "\tlayer:{}", e->entity.layer());
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

namespace biscuit::dxf::entities::inline test {

	//=============================================================================================================================
	//using namespace biscuit::dxf::entities;

	template < sGroup::eGROUP_CODE eGroupCode_, auto offset1, auto offset2 >
	struct TGroupValueMAccessor {
		constexpr inline static sGroup::eGROUP_CODE const eGroupCode{eGroupCode_};
		constexpr inline static sGroup::eVALUE_TYPE const eValueType = sGroup::GET_VALUE_TYPE_ENUM(eGroupCode);
		using value_t = biscuit::dxf::detail::TGroupValue<eGroupCode>::value_t;

		TGroupValueMAccessor& operator = (TGroupValueMAccessor const&) = default;
		auto operator <=> (TGroupValueMAccessor const&) const = default;

		template < typename TEntity >
		value_t& Value(TEntity& entity) {
			auto& e1 = entity.*offset1;
			auto& e2 = e1.*offset2;
			return e2;
		}
	};

	template < sGroup::eGROUP_CODE eGroupCode_, auto Accessor>
	struct TGroupValueAccessor {
		constexpr inline static sGroup::eGROUP_CODE const eGroupCode{eGroupCode_};
		template < typename TEntity >
		auto& Value(TEntity& e) { return Accessor(e); }
	};

	struct sTestEntity {
		using this_t = sTestEntity;

		gcv<100> marker{};	// Subclass marker (AcDbProxyEntity)
		gcv< 90> proxy_entity_class_id{ 498 };
		gcv< 91> application_entity_class_id{ 500 };
		gcv< 92> size_graphics_data_in_bytes{};
		gcv<310> graphics_data{};
		gcv< 93> size_entity_data_in_bits{};
		gcv<310> entity_data{};
		gcv<330> object_id0{};
		gcv<340> object_id1{};
		gcv<350> object_id2{};
		gcv<360> object_id3{};
		gcv< 94> end_of_object_id_section{};
		gcv< 95> size_proxy_data_in_bytes{};
		gcv< 70> dwg_or_dxf{};	// Original custom object data format (0 = DWG, 1 = DXF)

		point_t pt;
		//TGroupValueMAccessor< 10, &this_t::pt, &point_t::x> pt_x;
		//TGroupValueAccessor< 20, [](auto& e) -> auto& { return e.pt.y; }> pt_y;
		//TGroupValueAccessor< 30, [](auto& e) -> auto& { return e.pt.z; }> pt_z;

		auto operator <=> (this_t const&) const = default;

		constexpr static inline auto group_members = std::make_tuple(
			std::pair{10, [](auto& self) -> auto& { return self.pt.x; }},
			std::pair{20, [](auto& self) -> auto& { return self.pt.y; }},
			std::pair{30, [](auto& self) -> auto& { return self.pt.z; }}
		);

	};

	//=============================================================================================================================
	template < typename TEntity >
	bool ReadItemEntityMember(TEntity& item, sGroup const& group, size_t& index) {

		// direct access to entity members as is using glz::reflect
		constexpr static size_t nMemberSize = glz::reflect<TEntity>::size;
		bool bFound = ForEachIntSeq<nMemberSize>(
			[&]<int I>{
				auto& v = glz::reflect<TEntity>::elem<I>(item);
				using member_t = std::remove_cvref_t<decltype(v)>;
				if constexpr (requires (member_t m) { m.eGroupCode; }) {
					if (v.eGroupCode == group.eCode) {
						if (index > 0) {
							index--;
							return false;
						}
						if constexpr (requires (member_t m) { m.value; }) {
							group.GetValue(v.value);
						}
						else if constexpr (requires (member_t m) { m.Value(item); }) {
							group.GetValue(v.Value(item));
						}
						return true;
					}
				}
				return false;
			});
		if (bFound) return true;

		// indirect access via group_members
		if constexpr (requires (TEntity ) { TEntity::group_members; }) {
			constexpr static size_t nTupleSize = std::tuple_size_v<decltype(TEntity::group_members)>;
			bFound = ForEachIntSeq<nTupleSize>([&]<int I>{
				auto const& pair = std::get<I>(TEntity::group_members);
				if constexpr (std::is_integral_v<std::remove_cvref_t<decltype(pair)>>) {
					// end marker. do nothing.
				}
				else {
					if (pair.first == group.eCode) {
						if (index > 0) {
							index--;
							return false;
						}
						group.GetValue(pair.second(item));
						return true;
					}
				}
				return false;
			});

			if (bFound)
				return true;
		}

		//constexpr static size_t nInvokableSize = TEntity::member;
		//if (ForEachIntSeq<nTupleSize>)

		return false;
	}

}

TEST_CASE("glaze.dxf") {
	using T = biscuit::dxf::entities::test::sTestEntity;
	//static_assert(glz::reflect<T>::size == 15);

	size_t index{};
	biscuit::dxf::sGroup group{100, "AcDbProxyEntity"s};
	T entity;
	ReadItemEntityMember(entity, group, index);
	ReadItemEntityMember(entity, {10, 3.2}, index);
	ReadItemEntityMember(entity, {20, 42.0}, index);
	ReadItemEntityMember(entity, {30, 11.0}, index);

	REQUIRE(entity.marker.value == "AcDbProxyEntity"s);
	REQUIRE(entity.pt.x == 3.2);
	REQUIRE(entity.pt.y == 42.0);
	REQUIRE(entity.pt.z == 11.0);

}
