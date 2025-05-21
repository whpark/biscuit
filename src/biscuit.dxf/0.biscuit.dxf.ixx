module;

export module biscuit.dxf;
export import :group;
import :group_code_type_alias;
export import :stream;
export import :sections;
import std;
import "biscuit/dependencies/fmt.hxx";
import "biscuit/dependencies/eigen.hxx";
import biscuit;
import biscuit.shape;


using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::dxf {

	class xDXF :
		public xSectionHead,
		public xSectionClasses,
		public xSectionTables,
		public xSectionBlocks,
		public xSectionEntities,
		public xSectionObjects,
		public xSectionThumbnailImage
	{
	public:
		using this_t = xDXF;
	protected:
		groups_t m_groups;

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

			TContainerMap<std::string, std::function<bool(this_t& self, group_iter_t& iter, group_iter_t const& end)>, std::deque> mapReader;
			mapReader["HEADER"]			= [](this_t& self, group_iter_t& iter, group_iter_t const& end) {
				return ReadSection(static_cast<xSectionHead&>(self), iter, end); };
			mapReader["CLASSES"]		= [](this_t& self, group_iter_t& iter, group_iter_t const& end) {
				return ReadSection(static_cast<xSectionClasses&>(self), iter, end); };
			mapReader["TABLES"]			= [](this_t& self, group_iter_t& iter, group_iter_t const& end) {
				return ReadSection(static_cast<xSectionTables&>(self), iter, end); };
			mapReader["BLOCKS"]			= [](this_t& self, group_iter_t& iter, group_iter_t const& end) {
				return ReadSection(static_cast<xSectionBlocks&>(self), iter, end); };
			mapReader["ENTITIES"]		= [](this_t& self, group_iter_t& iter, group_iter_t const& end) {
				return ReadSection(static_cast<xSectionEntities&>(self), iter, end); };
			mapReader["OBJECTS"]		= [](this_t& self, group_iter_t& iter, group_iter_t const& end) {
				return ReadSection(static_cast<xSectionObjects&>(self), iter, end); };
			mapReader["THUMBNAILIMAGE"] = [](this_t& self, group_iter_t& iter, group_iter_t const& end) {
				return ReadSection(static_cast<xSectionThumbnailImage&>(self), iter, end); };

			for (auto iter = groups.cbegin(), end = groups.cend(); iter != end; iter++) {
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
				if (group.eCode != 2 or !section) {
					auto pos = iter - groups.cbegin()+1;
					fmt::println("ReadDXF: invalid section name, pos: {}(rec:{}), group: {}", 2*pos, pos, *iter);
					return false;
				}
				if (auto p = mapReader.find(*section); p != mapReader.end()) {
					auto& reader = p->second;
					if (!reader or !reader(*this, iter, end)) {
						auto pos = iter - groups.cbegin()+1;
						fmt::println("ReadDXF Section - Error, pos: {}(rec:{}), group: {}", 2*pos, pos, *iter);
						return false;
					}
					mapReader.erase(p);
				}
			}
			return mapReader.size() <= 1;
		}

	};

}

