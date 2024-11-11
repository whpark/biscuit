module;

export module biscuit.dxf;
export import :group;
import :stream;
import :sections;


import std;
import biscuit;
using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::dxf {

	class xDXF : public xSectionHead, public xSectionClasses, public xSectionTables {
	public:
		using this_t = xDXF;
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
			using iter_t = std::remove_cvref_t<decltype(groups)>::const_iterator;

			TContainerMap<std::string, std::function<bool(this_t& self, iter_t& iter, iter_t const& end)>, std::deque> mapReader;
			mapReader["HEADER"]		= [](this_t& self, iter_t& iter, iter_t const& end) { return ReadSection(static_cast<xSectionHead&>(self), iter, end); };
			mapReader["CLASSES"]	= [](this_t& self, iter_t& iter, iter_t const& end) { return ReadSection(static_cast<xSectionClasses&>(self), iter, end); };
			mapReader["TABLES"]		= [](this_t& self, iter_t& iter, iter_t const& end) { return ReadSection(static_cast<xSectionTables&>(self), iter, end); };
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
				auto section = group.GetValue<string_t>();
				if (group.iGroupCode != 2 or !section)
					return false;
				if (auto p = mapReader.find(*section); p != mapReader.end()) {
					auto& reader = p->second;
					if (!reader or !reader(*this, iter, end))
						return false;
					mapReader.erase(p);
				}

			}
			return mapReader.empty();
		}

	};

}

