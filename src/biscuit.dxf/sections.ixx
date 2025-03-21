module;

#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"

export module biscuit.dxf:sections;
import std;
import fmt;
import Eigen;
import biscuit;
import :type_alias;
import :group;
import :stream;
import :entities;

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
		string_t name;
		string_t cpp_class_name;
		string_t app_name;
		int32 flags{};
		int32 count{};
		int16 proxy{};
		int16 entity{};

		constexpr static inline auto const group_members = std::make_tuple(
			//1, [](sClass& self) -> auto& { return self.name; },
			//1, [](sClass& self, sGroup const& g)->bool{ return g.GetValue(self.name); },
			1, &sClass::name,
			2, &sClass::cpp_class_name,
			3, &sClass::app_name,
			90, &sClass::flags,
			91, &sClass::count,
			280, &sClass::proxy,
			281, &sClass::entity
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
		string_t table_type;
		int32 handle{};
		string_t class_name;
		int16 max_entries{};
		int16 flags{};

		constexpr static inline auto const group_members = std::make_tuple(
			2, &xTable::table_type,
			5, &xTable::handle,
			100, &xTable::class_name,
			70, &xTable::max_entries
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
		int32 handle{};
		//string_t entity;	// fixed value : AcDbEntity
		string_t layer;
		//string_t sign;		// fixed value : AcDbBlockBegin
		string_t name;
		int16 flags{};
		Eigen::Vector3d ptBase;
		string_t name2;
		string_t xref_path;

		constexpr static inline auto const group_members = std::make_tuple(
			5, &xBlock::handle,
			//100, &xBlock::entity,
			8, &xBlock::layer,
			//100, &xBlock::sign,
			2, &xBlock::name,
			70, &xBlock::flags,
			10, [](auto& self) -> decltype(auto) { return self.ptBase.x(); },
			20, [](auto& self) -> decltype(auto) { return self.ptBase.y(); },
			30, [](auto& self) -> decltype(auto) { return self.ptBase.z(); },
			3, & xBlock::name2,
			1, & xBlock::xref_path
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

