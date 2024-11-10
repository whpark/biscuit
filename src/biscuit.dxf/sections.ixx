module;

#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_glaze.h"
#include <boost/pfr.hpp>

export module biscuit.dxf:sections;
import std;
import biscuit;
import :group;
import :stream;

using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::dxf {

	using variable_map_t = std::map<string_t, std::vector<sGroup>>;


	template < typename T > 
	struct TGroupHandler;

	template < size_t N, size_t ... I, class Func >
	constexpr bool ForEachImpl(Func&& func, std::integer_sequence<size_t, I...>) {
		if constexpr (sizeof...(I) == 0) 
			return false;
		if (func.template operator()<N-(sizeof...(I))>())
			return true;
		if constexpr (sizeof...(I) > 1)
			return ForEachImpl<N>(std::move(func), std::make_integer_sequence<size_t, sizeof...(I)-1>{});
		return false;
	}

	template < size_t N, class Func >
	constexpr bool ForEach(Func&& func) {
		return ForEachImpl<N>(std::move(func), std::make_integer_sequence<size_t, N>{});
	}


	struct sClass {
		string_t name;
		string_t strCppClassName;
		string_t strAppName;
		int32 flags{};
		int32 count{};
		int16 proxy{};	// 1 = entity is a proxy object
		int16 entity{};	// 1 = entity is an entity
	};

	template <>
	struct TGroupHandler<sClass> {
		constexpr static inline auto const handlers = std::make_tuple(
			1, [](sClass& self) -> auto& { return self.name; },
			1, [](sClass& self, sGroup const& g)->bool{ return g.Get(self.name); },
			1, &sClass::name,
			2, &sClass::strCppClassName,
			3, &sClass::strAppName,
			90, &sClass::flags,
			91, &sClass::count,
			280, &sClass::proxy,
			281, &sClass::entity
		);
	};


	//-----------------------------------------------------------------------------------------------------------------------------
	template < typename tIter >
	std::optional<variable_map_t> ReadHeadSection(tIter& iter, tIter const& end) {
		variable_map_t map;
		while (iter != end) {
			auto const& r = *iter;
			static sGroup const groupEndSection{ 0, "ENDSEC"s};
			if (r == groupEndSection) {
				iter++;
				break;
			}

			if (r.iGroupCode != 9)
				return std::nullopt;
			if (auto str = r.GetValue<string_t>()) {
				auto& sub = map[*str];
				for (iter++; iter != end; iter++) {
					auto const& item = *iter;
					if ( (item.iGroupCode == 0) or (item.iGroupCode == 9) )
						break;
					sub.push_back(item);
				}
			}
			else
				return std::nullopt;
		}
		return std::move(map);
	}

	//-----------------------------------------------------------------------------------------------------------------------------
	template < typename tItem, typename tIter >
	std::optional<tItem> ReadItem(tIter& iter, tIter const& end) {
		tItem item;
		for (iter++; iter != end; iter++) {
			auto const& r = *iter;
			if (r.iGroupCode == 0)
				break;
			constexpr static auto const handlers = TGroupHandler<tItem>::handlers;
			constexpr static auto nTupleSize = std::tuple_size_v<decltype(handlers)>/2;
			bool bFound = ForEach<nTupleSize>([&]<int I>{
				constexpr int code = std::get<I*2>(handlers);
				constexpr auto const offset_ptr = std::get<I*2+1>(handlers);
				if (r.iGroupCode == code) {
					if constexpr (std::is_member_object_pointer_v<decltype(offset_ptr)>) {
						r.Get(item.*offset_ptr);
						return true;
					}
					else if constexpr (std::invocable<decltype(offset_ptr), tItem&>) {
						r.Get(offset_ptr(item, 0));
					}
					else if constexpr (std::invocable<decltype(offset_ptr), tItem&, sGroup const&>) {
						if (offset_ptr(item, 0, r))
							return true;
					}
				}
				return false;
			});
			if (!bFound)
				return std::nullopt;
		}
		return item;
	}

	//-----------------------------------------------------------------------------------------------------------------------------
	template < typename tIter >
	std::optional<std::vector<sClass>> ReadClassesSection(tIter& iter, tIter const& end) {
		std::vector<sClass> classes;
		while (iter != end) {
			auto const& r = *iter;
			static sGroup const groupEndSection{ 0, "ENDSEC"s };
			if (r == groupEndSection) {
				iter++;
				break;
			}
			if (r.iGroupCode != 0)
				return std::nullopt;
			if (auto str = r.GetValue<string_t>()) {
				if (auto c = ReadItem<sClass>(iter, end))
					classes.push_back(std::move(*c));
				else
					break;
			}
			else
				return std::nullopt;
		}
		return std::move(classes);
	}


}	// namespace biscuit::dxf

