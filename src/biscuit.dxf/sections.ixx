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

		bool Setter(sGroup const& c) { c.Get(name); name += "**********"; return true; }
		//static std::map<group_code_t, sHandler> s_mapHandlers;
		//THandler<10, sClass, string_t, &sClass::name> a;
		//constexpr static inline auto const handlers = std::make_tuple( THandler{THandlerIndex<1>{}, &sClass::name}, THandler{THandlerIndex<2>{}, &sClass::strCppClassName});
		constexpr static inline auto const handlers = std::make_tuple(
			//1, &sClass::Setter,
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
	template < typename tIter >
	std::optional<sClass> ReadClass(tIter& iter, tIter const& end) {
		sClass c;
		for (iter++; iter != end; iter++) {
			auto const& r = *iter;
			if (r.iGroupCode == 0)
				break;
			//bool bFound = ForEach<1>([&]<int I>{
			bool bFound = ForEach<std::tuple_size_v<decltype(sClass::handlers)>/2>([&]<int I>{
				constexpr int code = std::get<I*2>(c.handlers);
				constexpr auto const offset_ptr = std::get<I*2+1>(c.handlers);
				if (r.iGroupCode == code) {
					if constexpr (std::is_invocable_v<decltype(offset_ptr)>) {
						if (c.*offset_ptr(r))
							return true;
					}
					else {
						r.Get(c.*offset_ptr);
						return true;
					}
				}
				return false;
			});
			if (!bFound)
				return std::nullopt;
		}
		return c;
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
				if (auto c = ReadClass(iter, end))
					classes.push_back(std::move(*c));
			}
			else
				return std::nullopt;
		}
		return std::move(classes);
	}


}	// namespace biscuit::dxf

