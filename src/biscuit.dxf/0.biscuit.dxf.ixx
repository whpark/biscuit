module;

export module biscuit.dxf;
export import :group;
export import :stream;
export import :sections;


import std;
import biscuit;
using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::dxf {

	class xDXF {
	public:
		variable_map_t m_mapVariables;
		std::vector<sClass> m_classes;
	protected:
		std::vector<sGroup> m_groups;

	public:
		auto const& GetGroups() const { return m_groups; }

	public:
		bool ReadDXF(std::filesystem::path const& path) {
			if (auto g = ReadGroups(path); g)
				m_groups = std::move(*g);
			else
				return false;

			auto& groups = m_groups;
			for (auto iter = groups.begin(), end = groups.end(); iter != end; iter++) {
				auto const& r = *iter;

				static sGroup const groupEOF{ 0, "EOF"s };
				if (r == groupEOF)
					break;

				// Header
				static sGroup const groupHeader[]{ {0, "SECTION"s}, {2, "HEADER"s}, };
				if (std::ranges::starts_with(std::span(iter, end), groupHeader)) {
					iter += std::size(groupHeader);
					if (auto map = ReadHeadSection(iter, end))
						m_mapVariables = std::move(*map);
					else
						return false;
				}

				// Classes
				static sGroup const groupClasses[]{ {0, "SECTION"s}, {2, "CLASSES"s}, };
				if (std::ranges::starts_with(std::span(iter, end), groupClasses)) {
					iter += std::size(groupClasses);
					if (auto c = ReadClassesSection(iter, end))
						m_classes = std::move(*c);
					else
						return false;
				}


			}
			return true;
		}

	};

}

