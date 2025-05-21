module;

export module biscuit.dxf:sections;
import std;
import "biscuit/dependencies/fmt.hxx";
import "biscuit/dependencies/eigen.hxx";
import biscuit;
import :group;
import :group_code_type_alias;
import :stream;
import :entities;
import :entities_subclass;

using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::dxf {

	//=============================================================================================================================

	template < typename TSection >
	bool ReadSection(TSection& section, group_iter_t& iter, group_iter_t const& end) {
		static sGroup const groupEndSection{ 0, "ENDSEC"s};
		if (!section.InitSection())
			return false;

		for (iter++; iter != end; iter++) {
			auto const& r = *iter;
			if (r == groupEndSection) {
				return true;
			}
			if (!section.ReadSectionItem(iter, end))
				return false;
			if (iter == end)
				return false;
		}
		return false;
	}

	//=============================================================================================================================
	// Header section
	using variable_map_t = TContainerMap<string_t, std::vector<sGroup>>;
	//-----------------------------------------------------------------------------------------------------------------------------
	class xSectionHead {
	public:
		using this_t = xSectionHead;

		variable_map_t m_mapVariables;
	public:
		bool InitSection() {
			m_mapVariables.clear();
			return true;
		}

		//template < typename group_iter_t >
		bool ReadSectionItem(group_iter_t& iter, group_iter_t const& end) {
			auto const& r = *iter;
			if (r.eCode != 9)
				return false;
			auto str = r.GetValue<string_t>();
			if (!str)
				return false;
			auto& sub = m_mapVariables[*str];
			for (iter++; iter != end; iter++) {
				auto const& item = *iter;
				if ((item.eCode == 0) or (item.eCode == 9)) {
					iter--;	// current item is NOT an EndCondition.
					break;
				}
				sub.push_back(item);
			}
			return true;
		}
	};

	//=============================================================================================================================
	// Classes Section
	struct sClass {
		group_code_value_t<  1> name;
		group_code_value_t<  2> cpp_class_name;
		group_code_value_t<  3> app_name;
		group_code_value_t< 90> flags{};
		group_code_value_t< 91> count{};
		group_code_value_t<280> proxy{};
		group_code_value_t<281> entity{};

		constexpr static inline auto const group_members = std::make_tuple(
			&sClass::name,
			&sClass::cpp_class_name,
			&sClass::app_name,
			&sClass::flags,
			&sClass::count,
			&sClass::proxy,
			&sClass::entity
		);
	};
	using classes_t = std::vector<sClass>;

	//-----------------------------------------------------------------------------------------------------------------------------
	class xSectionClasses {
	public:
		using this_t = xSectionClasses;

		classes_t m_classes;
	public:
		bool InitSection() {
			m_classes.clear();
			return true;
		}
		//template < typename group_iter_t >
		bool ReadSectionItem(group_iter_t& iter, group_iter_t const& end) {
			auto const& r = *iter;
			static const sGroup groupClass{ 0, "CLASS"s };
			if (r != groupClass)
				return false;
			m_classes.emplace_back();
			auto& aClass = m_classes.back();
			for (iter++; iter != end; iter++) {
				if (iter->eCode == 0) {
					iter--;	// current item is NOT an EndCondition.
					return true;
				}
				if (!ReadItemSingleMember(aClass, *iter))
					return false;
			}
			return true;
		}
	};

	//=============================================================================================================================
	class xTable {
	public:
		using this_t = xTable;
	public:
		group_code_value_t<  2> table_type;
		group_code_value_t<  5> handle{};
		group_code_value_t<100> class_name;
		group_code_value_t< 70> max_entries{};
		int16 flags{};

		constexpr static inline auto const group_members = std::make_tuple(
			&this_t::table_type,
			&this_t::handle,
			&this_t::class_name,
			&this_t::max_entries
		);
	};

	using tables_t = std::vector<xTable>;
	//-----------------------------------------------------------------------------------------------------------------------------
	class xSectionTables {
	public:
		using this_t = xSectionTables;

		tables_t m_tables;

	public:
		bool InitSection() {
			m_tables.clear();
			return true;
		}
		//template < typename group_iter_t >
		bool ReadSectionItem(group_iter_t& iter, group_iter_t const& end) {
			auto const& r = *iter;
			static const sGroup groupTable{ 0, "TABLE"s };
			if (r != groupTable)
				return false;

			m_tables.emplace_back();

			for (iter++; iter != end; iter++) {
				static sGroup const groupTableEnd{ 0, "ENDTAB"s };
				if (*iter == groupTableEnd) {
					return true;
				}
				if (ReadItemSingleMember(m_tables.back(), *iter)) {
				}
				else {
					// todo:

				}
			}
			return false;
		}
	};

	//=============================================================================================================================
	class xBlock {
	public:
		using this_t = xBlock;
	public:
		group_code_value_t<  5> handle{};
		//string_t entity;	// fixed value : AcDbEntity
		group_code_value_t<  8> layer;
		//string_t sign;		// fixed value : AcDbBlockBegin
		group_code_value_t<  2> name;
		group_code_value_t< 70> flags{};
		entities::point_t ptBase;
		group_code_value_t<  3> name2;
		group_code_value_t<  1> xref_path;

		constexpr static inline auto const group_members = std::make_tuple(
			&this_t::handle,
			//100, &this_t::entity,
			&this_t::layer,
			//100, &this_t::sign,
			&this_t::name,
			&this_t::flags,
			std::pair{10, [](auto& self) -> auto& { return self.ptBase.x; }},
			std::pair{20, [](auto& self) -> auto& { return self.ptBase.y; }},
			std::pair{30, [](auto& self) -> auto& { return self.ptBase.z; }},
			&this_t::name2,
			&this_t::xref_path
		);
	};
	class xSectionBlocks {
	public:
		using this_t = xSectionBlocks;
		std::vector<xBlock> m_blocks;
	//protected:
	//	binary_t hExit;

	public:
		bool InitSection() {
			m_blocks.clear();
			return true;
		}

		//template < typename group_iter_t >
		bool ReadSectionItem(group_iter_t& iter, group_iter_t const& end) {
			auto const& r = *iter;
			static sGroup const groupBlock{ 0, "BLOCK"s };
			if (r != groupBlock)
				return false;

			m_blocks.emplace_back();
			for (iter++; iter != end; iter++) {
				//static sGroup const groupBlockEnd{ 0, "ENDBLK"s };
				static sGroup const groupBlockEnd{ 100, "AcDbBlockEnd"s };
				if (*iter == groupBlockEnd) {
					return true;
				}
				if (ReadItemSingleMember(m_blocks.back(), *iter)) {
				}
				else {
					// todo:
				}
			}
			return false;
		}
	};

	//=============================================================================================================================
	class xSectionEntities {
	public:
		using this_t = xSectionEntities;
		entities::entities_t m_entities;

	public:
		bool InitSection() {
			m_entities.clear();
			return true;
		}

		//template < typename group_iter_t >
		bool ReadSectionItem(group_iter_t& iter, group_iter_t const& end) {
			auto const& r = *iter;
			auto const* entity_name = std::get_if<string_t>(&r.value);
			if (!entity_name or r.eCode != 0)
				return false;
			if (auto entity = entities::xEntity::CreateEntity(*entity_name))
				m_entities.push_back(std::move(entity));
			else
				return false;

			auto& entity = *m_entities.back();
			return entity.Read(iter, end);
		}
	};

	//=============================================================================================================================
	class xSectionObjects {
	public:

	public:
		bool InitSection() {
			return true;
		}

		//template < typename group_iter_t >
		bool ReadSectionItem(group_iter_t& iter, group_iter_t const& end) {
			auto const& r = *iter;
			//static const sGroup groupObject{ 0, "OBJECTS"s };
			//if (r != groupObject)
			//	return false;
			for (iter++; iter != end; iter++) {
				static sGroup const groupObjectEnd{ 0, "ENDSEC"s };
				if (*iter == groupObjectEnd) {
					iter--;	// current item is for next sequence.
					return true;
				}
			}
			return false;
		}
	}; 

	//=============================================================================================================================
	class xSectionThumbnailImage {
	public:
		bool InitSection() {
			return true;
		}
		//template < typename group_iter_t >
		bool ReadSectionItem(group_iter_t& iter, group_iter_t const& end) {
			//auto const& r = *iter;
			//static const sGroup groupThumbnailImage{ 0, "THUMBNAILIMAGE"s };
			//if (r != groupThumbnailImage)
			//	return false;
			for (iter++; iter != end; iter++) {
				static sGroup const groupThumbnailImageEnd{ 0, "ENDSEC"s };
				if (*iter == groupThumbnailImageEnd) {
					iter--;	// current item is for next sequence.
					return true;
				}
			}
			return false;
		}
	};

}	// namespace biscuit::dxf

