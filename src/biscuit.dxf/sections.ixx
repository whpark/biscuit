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
	class xSectionClasses {
	public:

	};

}	// namespace biscuit::dxf

