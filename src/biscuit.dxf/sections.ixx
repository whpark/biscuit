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

	//=============================================================================================================================
	template < typename T >
	struct TGroupHandler;

	//-----------------------------------------------------------------------------------------------------------------------------
	template < typename TSection, typename TIter >
	using func_read_section_t = std::function<bool(TSection&, TIter&, TIter const&)>;

	template < typename TSection, typename TIter >
	bool TReadSection(TSection& section, TIter& iter, TIter const& end) {
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
		}
		return false;
	}

	template < typename TItem >
	bool TReadItemSingleMember(TItem& item, sGroup const& group) {
		constexpr static auto const handlers = TGroupHandler<TItem>::handlers;
		constexpr static auto nTupleSize = std::tuple_size_v<decltype(handlers)>/2;
		return ForEachIntSeq<nTupleSize>([&]<int I>{
			constexpr int code = std::get<I*2>(handlers);
			constexpr auto const offset_ptr = std::get<I*2+1>(handlers);
			if (group.iGroupCode != code)
				return false;
			if constexpr (std::is_member_object_pointer_v<decltype(offset_ptr)>) {
				group.Get(item.*offset_ptr);
				return true;
			}
			else if constexpr (std::invocable<decltype(offset_ptr), TItem&>) {
				group.Get(offset_ptr(item, 0));
				return true;
			}
			else if constexpr (std::invocable<decltype(offset_ptr), TItem&, sGroup const&>) {
				return offset_ptr(item, 0, group);
			}
		});
	}
	//template < typename TItem, typename TIter >
	//std::optional<TItem> TReadItem(TIter& iter, TIter const& end) {
	//	TItem item;
	//	for (iter++; iter != end; iter++) {
	//		if (group.iGroupCode == 0) {
	//			iter--;
	//			return std::nullopt;
	//		}
	//		if (!TReadItemSingleMember(item, *iter))
	//			return std::nullopt;
	//	}
	//	return item;
	//}

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

		template < typename TIter >
		bool ReadSectionItem(TIter& iter, TIter const& end) {
			auto const& r = *iter;
			if (r.iGroupCode != 9)
				return false;
			auto str = r.GetValue<string_t>();
			if (!str)
				return false;
			auto& sub = m_mapVariables[*str];
			for (iter++; iter != end; iter++) {
				auto const& item = *iter;
				if ((item.iGroupCode == 0) or (item.iGroupCode == 9)) {
					iter--;
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
	};
	using classes_t = std::vector<sClass>;

	template <>
	struct TGroupHandler<sClass> {
		constexpr static inline auto const handlers = std::make_tuple(
			//1, [](sClass& self) -> auto& { return self.name; },
			//1, [](sClass& self, sGroup const& g)->bool{ return g.Get(self.name); },
			1, &sClass::name,
			2, &sClass::cpp_class_name,
			3, &sClass::app_name,
			90, &sClass::flags,
			91, &sClass::count,
			280, &sClass::proxy,
			281, &sClass::entity
		);
	};
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
		template < typename TIter >
		bool ReadSectionItem(TIter& iter, TIter const& end) {
			auto const& r = *iter;
			static const sGroup groupClass{ 0, "CLASS"s };
			if (r != groupClass)
				return false;
			m_classes.emplace_back();
			auto& aClass = m_classes.back();
			for (iter++; iter != end; iter++) {
				if (iter->iGroupCode == 0) {
					iter--;
					return true;
				}
				if (!TReadItemSingleMember(aClass, *iter))
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
		string_t ac;
		int16 max_entries{};
	};
	template <>
	struct TGroupHandler<xTable> {
		constexpr static inline auto const handlers = std::make_tuple(
			2, &xTable::table_type,
			5, &xTable::handle,
			100, &xTable::ac,
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
		template < typename TIter >
		bool ReadSectionItem(TIter& iter, TIter const& end) {
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
				if (TReadItemSingleMember(m_tables.back(), *iter)) {
				}
				else {

				}
			}
			return false;
		}
	};

	//std::optional<tables_t> ReadTablesSection(TIter& iter, TIter const& end) {
	//	tables_t tables;
	//	while (iter != end) {
	//		auto const& r = *iter;
	//		if (r == groupEndSection) {
	//			iter++;
	//			break;
	//		}
	//		if (r.iGroupCode != 0)
	//			return std::nullopt;

	//	}
	//	return std::move(tables);
	//}


}	// namespace biscuit::dxf

