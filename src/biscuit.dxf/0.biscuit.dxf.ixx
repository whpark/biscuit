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

	class xDXF : public xSectionHead, public xSectionClasses, public xSectionTables {
	public:
		//variable_map_t m_mapVariables;
		//classes_t m_classes;
		//tables_t m_tables;
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

			// TopMost. Read SECTIONs
			auto& groups = m_groups;
			for (auto iter = groups.begin(), end = groups.end(); iter != end; iter++) {

				// Read Section Mark
				{
					auto const& r = *iter;
						static sGroup const groupEOF{ 0, "EOF"s };
					if (r == groupEOF)
						break;
					static sGroup const groupSectionStart{ 0, "SECTION"s };
					if (r != groupSectionStart)
						return false;
					if (++iter == end)
						break;
				}

				// Read Section Content
				auto const& group = *iter;

				bool bRead = false;
				// Header
				static sGroup const groupHeader{2, "HEADER"s};
				if ( (group == groupHeader) and TReadSection(static_cast<xSectionHead&>(*this), iter, end))
					continue;

				// Classes
				static sGroup const groupClasses{2, "CLASSES"s};
				if ((group == groupClasses) and TReadSection(static_cast<xSectionClasses&>(*this), iter, end))
					continue;

				// Tables
				static sGroup const groupTables{2, "TABLES"s};
				if ((group == groupTables) and TReadSection(static_cast<xSectionTables&>(*this), iter, end))
					continue;

				return true;

			}
			return true;
		}

	};

}
