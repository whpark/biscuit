module;

#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_glaze.h"

export module biscuit.dxf:sections;
import std;
import biscuit;
import :group;
import :stream;

using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::dxf {

	using variable_map_t = std::map<string_t, std::vector<sGroup>>;

	template < typename tStruct, typename tType >
	struct THandler {
		group_code_t const iGroupCode;
		tType tStruct::* const member;
		constexpr THandler(group_code_t iGroupCode, tType tStruct::* member) : iGroupCode(iGroupCode), member(member) {}
		//THandler() : member(MEMBER) {}
	};

	struct sClass {
		string_t name;
		string_t strCppClassName;
		string_t strAppName;
		int32 flags{};
		int32 count{};
		int16 proxy{};	// 1 = entity is a proxy object
		int16 entity{};	// 1 = entity is an entity

		//static std::map<group_code_t, sHandler> s_mapHandlers;
		//THandler<10, sClass, string_t, &sClass::name> a;
		constexpr static inline auto const handlers = std::make_tuple( THandler{1, &sClass::name}, THandler{2, &sClass::strCppClassName} );

		void Func() {
			THandler a(1, &sClass::name);
			//this->*a.member = "name"s;
		}
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
		c.Func();	// temp
		for (iter++; iter != end; iter++) {
			auto const& r = *iter;
			if (r.iGroupCode == 0)
				break;
			switch (r.iGroupCode) {
			case 1: if (!r.Get(c.name)) return {};				break;
			case 2: if (!r.Get(c.strCppClassName)) return {};	break;
			case 3: if (!r.Get(c.strAppName)) return {};		break;
			case 90:if (!r.Get(c.flags)) return {};				break;
			case 91:if (!r.Get(c.count)) return {};				break;
			//default: c.groups.push_back(r);						break;
			}
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

