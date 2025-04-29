module;

#include "macro.h"

export module biscuit.dxf:entities;
import std;
import "biscuit/dependencies_fmt.hxx";
import "biscuit/dependencies_eigen.hxx";
import biscuit;
import :group;
import :group_code_type_alias;
import :entities_subclass;

using namespace std::literals;
using namespace biscuit::literals;

export namespace biscuit::dxf::entities {

	//=============================================================================================================================

	enum class eENTITY {
		none,
		unknown,
		_3dface,
		_3dsolid,
		acad_proxy_entity,
		arc,
		attdef,
		attrib,
		body,
		circle,
		dimension,
		ellipse,
		hatch,
		helix,
		image,
		insert,
		leader,
		light,
		line,
		lw_polyline,
		mesh,
		mline,
		mleader_style,
		mleader,
		mtext,
		mtext_attdef,
		ole_frame,
		ole2_frame,
		point,
		polyline,
		ray,
		region,
		section,
		seqend,
		shape,
		solid,
		spline,
		sun,
		surface,
		table,
		text,
		tolerance,
		trace,
		underlay,
		vertex,
		viewport,
		wipeout,
		xline,
	};

	template < typename TEntity, bool bSubclass = false >
	bool ReadEntity(TEntity& entity, group_iter_t& iter, group_iter_t const& end) {
		TContainerMap<group_code_t, int> mapGroupCodeToIndex; // for duplicated group number
		for (iter++; iter != end; iter++) {
			if constexpr (bSubclass) {
				if (iter->eCode == group_code_t::subclass) { iter--; return true; }
			}
			if (iter->eCode == group_code_t::entity) { iter--; return true; }

			if constexpr (requires (TEntity v) { v.ReadControlData(iter, end); }) {
				if (entity.ReadControlData(iter, end))
					continue;
				if (iter == end)
					return false;
			}

			size_t index = mapGroupCodeToIndex[iter->eCode]++;
			if (/*iter != end and*/ ReadItemSingleMember<TEntity>(entity, *iter, index))
				continue;

			if constexpr (requires(TEntity v) { v.ReadPrivate(iter, end); }) {
				if (iter != end and entity.ReadPrivate(iter, end))
					continue;
				if constexpr (bSubclass) {
					if (iter->eCode == group_code_t::subclass) { iter--; return true; }
				}
				if (iter != end and iter->eCode == group_code_t::entity) {
					iter--;	// current item is for next sequence.
					return true;
				}
			}

			if constexpr (requires(TEntity v) { v.ReadExtendedData(iter, end); }) {
				if (iter != end and entity.ReadExtendedData(iter, end))
					continue;
				if constexpr (bSubclass) {
					if (iter->eCode == group_code_t::subclass) { iter--; return true; }
				}
				if (iter != end and iter->eCode == group_code_t::entity) {	// !!! ATTDEF has 0 group code.
					iter--;	// current item is for next sequence.
					return true;
				}
			}
		}
		return true;
	}

	//-------------------------------------------------------------------------
	class xEntity : public sEntity {
	public:
		using root_t = xEntity;
		using this_t = xEntity;
		using string_t = std::string;
	public:
		groups_t extended_data;
		string_t app_name;								// 102:{application_name ... 102:}
		std::vector<sGroup> app_data;
		binary_t hReactor;								// 102:{ACD_REACTORS 330:value 102:}
		binary_t hOwner;								// 102:{ACAD_XDICTIONARY 330:owner_handle 102:}

		constexpr static inline auto group_members = std::make_tuple(
		);

	public:
		xEntity() = default;
		xEntity(xEntity const&) = default;
		xEntity(xEntity&&) = default;
		xEntity& operator=(xEntity const&) = default;
		xEntity& operator=(xEntity&&) = default;
		virtual ~xEntity() = default;

		bool operator == (xEntity const&) const = default;
		bool operator != (xEntity const&) const = default;
		auto operator <=> (xEntity const&) const = default;

		virtual std::unique_ptr<xEntity> clone() const = 0;
		virtual bool Compare(xEntity const& other) const {
			return (GetEntityType() == other.GetEntityType())
				&& (*this == other)
				;
		}
		virtual eENTITY GetEntityType() const { return eENTITY::none; }

		virtual bool Read(group_iter_t& iter, group_iter_t const& end) = 0;
		virtual bool ReadPrivate(group_iter_t& iter, group_iter_t const& end) {
			return ReadEntity<sEntity, true>(*this, iter, end);
		}

		virtual bool ReadControlData(group_iter_t& iter, group_iter_t const& end) {
			if (iter->eCode != group_code_t::control)
				return false;
			static sGroup const groupEnd{ 102, std::string("}") };
			string_t str = iter->GetValue<string_t>().value_or(std::string());
			string_view_t sv(str);
			sv = TrimView(sv);
			if (sv.starts_with("{")) {
				sv.remove_prefix(1);
				sv = TrimView(sv);
			}
			else
				return false;
			if (sv.starts_with("ACAD_REACTORS")) {
				if (++iter == end or (iter->eCode != 330) or !iter->GetValue(hReactor))
					return false;
				if (*++iter != groupEnd)
					return false;
			}
			else if (sv.starts_with("ACAD_XDICTIONARY")) {
				if (++iter == end or (iter->eCode != 360) or !iter->GetValue(hOwner))
					return false;
				if (*++iter != groupEnd)
					return false;
			}
			else {
				app_name = sv;
				for (iter++; iter != end and *iter != groupEnd; iter++) {
					app_data.push_back(*iter);
				}
				if (iter == end)
					return false;
			}
			return true;
		}
		virtual bool ReadExtendedData(group_iter_t& iter, group_iter_t const& end) {
			if (iter == end)
				return false;
			if (iter->eCode < sGroup::extended)
				return false;

			auto iExtendedData = extended_data.size();
			for (; iter != end; iter++) {
				if (iter->eCode < sGroup::extended) {
					iter--;
					break;
				}
				extended_data.push_back(*iter);
			}
			auto iter2 = extended_data.cbegin() + iExtendedData;
			ProcessExtendedData(iter2, extended_data.cend());
			return true;
		}
		bool ProcessExtendedData(group_iter_t& iter, group_iter_t const& end) {
			if (*iter == sGroup(1000, "ACAD")) {
				for (iter++; iter != end; iter++) {
					groups_t groupsRe;
					if (iter->eCode == 1000 and iter->GetValue<std::string>().value_or(""s).ends_with("BEGIN")) {
						for (iter++; iter != end and iter+1 != end; iter++) {
							if (iter->eCode == 1000 and iter->GetValue<std::string>().value_or(""s).ends_with("END")) {
								break;
							}
							if (auto code = iter->GetValue<int16>(); code and *code != 0) {
								groupsRe.emplace_back(*code, (++iter)->value);
							}
						}
					}

					if (*iter == sGroup(1002, "{")) {
						for (iter++; iter != end and *iter != sGroup(1002, "}"); iter++) {
							if (auto code = iter->GetValue<int16>(); code and *code != 0 and iter+1 != end) {
								groupsRe.emplace_back(*code, (++iter)->value);
							}
						}
					}

					if (auto iter = groupsRe.cbegin(); iter != groupsRe.cend()) {
						Read(iter, groupsRe.cend());
					}
				}
				return true;
			}
			return false;
		}

	public:
		static std::unique_ptr<xEntity> CreateEntity(string_t const& name);
	protected:
		// map of entity factory
		static inline std::map<string_t, std::function<std::unique_ptr<xEntity>()>> m_mapEntityFactory;
		//static void RegisterEntity(string_t const& name, std::function<std::unique_ptr<xEntity>()> fnCreate) {
		//	m_mapEntityFactory[name] = std::move(fnCreate);
		//}
		class xRegisterEntity {
		public:
			xRegisterEntity(std::string const& name, std::function<std::unique_ptr<xEntity>()> fnCreate) {
				m_mapEntityFactory[name] = std::move(fnCreate);
				//xEntity::RegisterEntity(name, std::move(fnCreate));
			}
		};

		static bool ReadStringTo(group_iter_t& iter, group_iter_t const& end, string_t& text)  {
			if (iter == end) return false;
			if (iter->eCode == 1) { text = iter->GetValue<string_t>().value_or(""s); return true; }
			if (iter->eCode == 3) { text += iter->GetValue<string_t>().value_or(""s); return true; }
			return false;
		}
	};


	using entity_ptr_t = TCloneablePtr<xEntity>;
	using entities_t = std::deque<entity_ptr_t>;

	//============================================================================================================================
	class xUnknown : public xEntity {
	public:

		string_t m_name;
		std::vector<sGroup> groups;

		BSC__DXF_ENTITY_DEFINITION(eENTITY::unknown, "UNKNOWN", xUnknown, xEntity);

		xUnknown(string_t name) : m_name(std::move(name)) {}

		constexpr static inline auto group_members = std::make_tuple(
		);

		bool ReadPrivate(group_iter_t& iter, group_iter_t const& end) override {
			groups.push_back(*iter);
			return true;
		}
	};

	//============================================================================================================================
	class x3DFace : public xEntity {
	public:
		point_t pt1, pt2, pt3, pt4;
		enum fHIDDEN : gcv_t< 70> { fHIDDEN_FIRST = 1, fHIDDEN_SECOND = 2, fHIDDEN_THIRD = 4, fHIDDEN_FOURTH = 8 };
		gcv< 70, fHIDDEN> flags{};	// 70:

		BSC__DXF_ENTITY_DEFINITION(eENTITY::_3dface, "3DFACE", x3DFace, xEntity);

		constexpr static inline auto group_members = std::make_tuple(
			std::pair{10, BSC__LAMBDA_MEMBER_VALUE(pt1.x)},
			std::pair{20, BSC__LAMBDA_MEMBER_VALUE(pt1.y)},
			std::pair{30, BSC__LAMBDA_MEMBER_VALUE(pt1.z)},

			std::pair{11, BSC__LAMBDA_MEMBER_VALUE(pt2.x)},
			std::pair{21, BSC__LAMBDA_MEMBER_VALUE(pt2.y)},
			std::pair{31, BSC__LAMBDA_MEMBER_VALUE(pt2.z)},

			std::pair{12, BSC__LAMBDA_MEMBER_VALUE(pt3.x)},
			std::pair{22, BSC__LAMBDA_MEMBER_VALUE(pt3.y)},
			std::pair{32, BSC__LAMBDA_MEMBER_VALUE(pt3.z)},

			std::pair{13, BSC__LAMBDA_MEMBER_VALUE(pt4.x)},
			std::pair{23, BSC__LAMBDA_MEMBER_VALUE(pt4.y)},
			std::pair{33, BSC__LAMBDA_MEMBER_VALUE(pt4.z)},
			&this_t::flags
		);

	};

	//============================================================================================================================
	class x3DSolid : public xEntity {
	public:
		gcv<100> marker;	// Subclass marker (AcDbModelerGeometry)
		gcv< 70> version{ 1 };
		gcv<  1> proprietary_data;		// GroupCode 1, 3
		gcv<100> marker2;	// Subclass marker (AcDb3dSolid)
		gcv<350> owner_id_handle_to_history_object{};

		BSC__DXF_ENTITY_DEFINITION(eENTITY::_3dsolid, "3DSOLID", x3DSolid, xEntity);

		constexpr static inline auto group_members = std::make_tuple(
			&this_t::marker,
			&this_t::version,
			&this_t::marker2,
			&this_t::owner_id_handle_to_history_object
		);

		bool ReadPrivate(group_iter_t& iter, group_iter_t const& end) override {
			if (ReadStringTo(iter, end, proprietary_data))
				return true;
			return false;
		}
	};


	//=============================================================================================================================
	class xACADProxyEntity : public xEntity {
	public:
		gcv<100> marker{};	// Subclass marker (AcDbProxyEntity)
		gcv< 90> proxy_entity_class_id{ 498 };
		gcv< 91> application_entity_class_id{ 500 };
		gcv< 92> size_graphics_data_in_bytes{};
		gcv<310> graphics_data{};
		gcv< 93> size_entity_data_in_bits{};
		gcv<310> entity_data{};
		gcv<330> object_id0{};
		gcv<340> object_id1{};
		gcv<350> object_id2{};
		gcv<360> object_id3{};
		gcv< 94> end_of_object_id_section{};
		gcv< 95> size_proxy_data_in_bytes{};
		gcv< 70> dwg_or_dxf{};	// Original custom object data format (0 = DWG, 1 = DXF)

		BSC__DXF_ENTITY_DEFINITION(eENTITY::acad_proxy_entity, "ACAD_PROXY_ENTITY", xACADProxyEntity, xEntity)

		constexpr static inline auto group_members = std::make_tuple(
			&this_t::marker,
			&this_t::proxy_entity_class_id,
			&this_t::application_entity_class_id,
			&this_t::size_graphics_data_in_bytes,
			&this_t::graphics_data,
			&this_t::size_entity_data_in_bits,
			&this_t::entity_data,
			&this_t::object_id0,
			&this_t::object_id1,
			&this_t::object_id2,
			&this_t::object_id3,
			&this_t::end_of_object_id_section,
			&this_t::size_proxy_data_in_bytes,
			&this_t::dwg_or_dxf
		);
	};

	//=============================================================================================================================
	class xArc : public xEntity {
	public:
		gcv<100> markerCircle{};	// Subclass marker (AcDbCircle)
		gcv< 39> thickness{};
		point_t pt{};	// center
		gcv< 40> radius{};
		gcv<100> marker{};	// Subclass marker (AcDbArc)
		gcv< 50> start_angle{};
		gcv< 51> end_angle{};
		point_t extrusion{ 0., 0., 1. };

		BSC__DXF_ENTITY_DEFINITION(eENTITY::arc, "ARC", xArc, xEntity);

		constexpr static inline auto const group_members = std::make_tuple(
			&this_t::markerCircle,
			&this_t::thickness,
			std::pair{10, BSC__LAMBDA_MEMBER_VALUE(pt.x)},
			std::pair{20, BSC__LAMBDA_MEMBER_VALUE(pt.y)},
			std::pair{30, BSC__LAMBDA_MEMBER_VALUE(pt.z)},
			&this_t::radius,
			&this_t::marker,
			&this_t::start_angle,
			&this_t::end_angle,
			std::pair{210, BSC__LAMBDA_MEMBER_VALUE(extrusion.x)},
			std::pair{220, BSC__LAMBDA_MEMBER_VALUE(extrusion.y)},
			std::pair{230, BSC__LAMBDA_MEMBER_VALUE(extrusion.z)}
		);
	};

	//=============================================================================================================================
	class xPoint : public xEntity {
	public:
		gcv<100> marker{};	// Subclass marker (AcDbPoint)
		point_t pt{};
		gcv< 39> thickness{};
		point_t extrusion{ 0., 0., 1. };
		gcv< 50> angle{};	// angle of x axis for UCS inf effect when the point was drawn (optional, default = 0); used when PDMODE is nonzero

		BSC__DXF_ENTITY_DEFINITION(eENTITY::point, "POINT", xPoint, xEntity)

		constexpr static inline auto group_members = std::make_tuple(
			&this_t::marker,
			std::pair{10, BSC__LAMBDA_MEMBER_VALUE(pt.x)},
			std::pair{20, BSC__LAMBDA_MEMBER_VALUE(pt.y)},
			std::pair{30, BSC__LAMBDA_MEMBER_VALUE(pt.z)},
			&this_t::thickness,
			std::pair{210, BSC__LAMBDA_MEMBER_VALUE(extrusion.x)},
			std::pair{220, BSC__LAMBDA_MEMBER_VALUE(extrusion.y)},
			std::pair{230, BSC__LAMBDA_MEMBER_VALUE(extrusion.z)},
			&this_t::angle
		);
	};

	//=============================================================================================================================
	class xLine : public xEntity {
	public:
		using this_t = xLine;
	public:
		gcv<100> marker;
		point_t pt0{};
		point_t pt1{};
		gcv< 39> thickness{};
		point_t extrusion{ 0., 0., 1. };

		BSC__DXF_ENTITY_DEFINITION(eENTITY::line, "LINE", xLine, xEntity)

		constexpr static inline auto group_members = std::make_tuple(
			&this_t::marker,
			&this_t::thickness,
			std::pair{ 10, BSC__LAMBDA_MEMBER_VALUE(pt0.x)},
			std::pair{ 20, BSC__LAMBDA_MEMBER_VALUE(pt0.y)},
			std::pair{ 30, BSC__LAMBDA_MEMBER_VALUE(pt0.z)},
			std::pair{ 11, BSC__LAMBDA_MEMBER_VALUE(pt1.x)},
			std::pair{ 21, BSC__LAMBDA_MEMBER_VALUE(pt1.y)},
			std::pair{ 31, BSC__LAMBDA_MEMBER_VALUE(pt1.z)},
			std::pair{210, BSC__LAMBDA_MEMBER_VALUE(extrusion.x)},
			std::pair{220, BSC__LAMBDA_MEMBER_VALUE(extrusion.y)},
			std::pair{230, BSC__LAMBDA_MEMBER_VALUE(extrusion.z)}
		);

	};

	//=============================================================================================================================
	class xRay : public xLine {
	public:
		constexpr static inline auto group_members = std::make_tuple(
		);
		BSC__DXF_ENTITY_DEFINITION(eENTITY::ray, "XRAY", xRay, xLine)
	};

	//=============================================================================================================================
	class xXLine : public xLine {
	public:
		constexpr static inline auto group_members = std::make_tuple(
		);
		BSC__DXF_ENTITY_DEFINITION(eENTITY::xline, "XLINE", xXLine, xLine)
	};

	//=============================================================================================================================
	class xText : public xEntity {
	public:
		gcv<100> marker{};	// Subclass marker (AcDbText)
		gcv< 39> thickness{};
		point_t pt_align0{};	// alignment point
		gcv< 40> text_height{};
		gcv<  1> text{};
		gcv< 50> rotation{};
		gcv< 41> relative_x_scale_factor{1.};
		gcv< 51> oblique_angle{};
		gcv<  7> text_style_name{"STANDARD"s};
		enum fTEXT_GENERATION_FLAGS : gcv_t< 71> { fTEXT_GENERATION_FLAGS__BACKWARD = 2, fTEXT_GENERATION_FLAGS__UPSIDE_DOWN = 4 };
		gcv< 71, fTEXT_GENERATION_FLAGS> text_generation_flags{};	// 71:
		enum class eHORIZONTAL_TEXT_JUSTIFICATION : gcv_t< 72> { left, center, right, aligned, middle, fit };
		gcv< 72, eHORIZONTAL_TEXT_JUSTIFICATION> horizontal_justification{};	// 72:
		point_t pt_align1{};	// second alignment point
		point_t extrusion{ 0., 0., 1. };
		gcv<100> marker2;	// Subclass marker (AcDbText)
		enum class eVERTICAL_ALIGNMENT : gcv_t< 73> { baseline, bottom, middle, top};
		gcv< 73, eVERTICAL_ALIGNMENT> vertical_alignment{};	// 73:

		BSC__DXF_ENTITY_DEFINITION(eENTITY::text, "TEXT", xText, xEntity)

		constexpr static inline auto const group_members = std::make_tuple(
			&this_t::marker,
			&this_t::thickness,
			std::pair{10, BSC__LAMBDA_MEMBER_VALUE(pt_align0.x)},
			std::pair{20, BSC__LAMBDA_MEMBER_VALUE(pt_align0.y)}, 
			std::pair{30, BSC__LAMBDA_MEMBER_VALUE(pt_align0.z)},
			&this_t::text_height,
			&this_t::text,
			&this_t::rotation,
			&this_t::relative_x_scale_factor,
			&this_t::oblique_angle,
			&this_t::text_style_name,
			&this_t::text_generation_flags,
			&this_t::horizontal_justification,
			std::pair{11, BSC__LAMBDA_MEMBER_VALUE(pt_align1.x)},
			std::pair{21, BSC__LAMBDA_MEMBER_VALUE(pt_align1.y)},
			std::pair{31, BSC__LAMBDA_MEMBER_VALUE(pt_align1.z)},
			std::pair{210, BSC__LAMBDA_MEMBER_VALUE(extrusion.x)},
			std::pair{220, BSC__LAMBDA_MEMBER_VALUE(extrusion.y)},
			std::pair{230, BSC__LAMBDA_MEMBER_VALUE(extrusion.z)},
			&this_t::marker,
			&this_t::vertical_alignment
		);

	};

	//=============================================================================================================================
	class xMText : public xEntity {
	public:
		gcv<100> marker{};	// Subclass marker (AcDbMText)
		point_t pt;	// insertion point
		gcv< 40> nominal_text_height{};
		gcv< 41> reference_rectangle_width{};

		enum class eTEXT_ATTACHMENT_POINT : gcv_t< 71> {
			top_left = 1, top_center, top_right,
			middle_left, middle_center, middle_right,
			bottom_left, bottom_center, bottom_right
		};
		gcv< 71, eTEXT_ATTACHMENT_POINT> attachment_point{eTEXT_ATTACHMENT_POINT::top_left};	// 71

		enum class eDRAWING_DIRECTION : gcv_t< 72> {
			left_to_right = 1, top_to_bottom = 3, by_style = 5
		};
		gcv< 72, eDRAWING_DIRECTION> drawing_direction{eDRAWING_DIRECTION::left_to_right};	// 72

		string_t text;	// group code 1, 3

		gcv< 7> text_style_name{"STANDARD"s};
		point_t extrusion{ 0., 0., 1. };
		point_t direction{};	// 11, 21, 31
		gcv< 42> character_width{};
		gcv< 43> character_height{};
		gcv< 50> rotation{};	// in radian.
		enum class eLINE_SPACING_STYLE : gcv_t< 73> { at_least = 1, exact = 2 };
		gcv< 73, eLINE_SPACING_STYLE> line_spacing_style{eLINE_SPACING_STYLE::exact};	// 73:
		gcv< 44> line_spacing_factor{ 1. };	// 0.25 ~ 4.0
		enum class eBACKGROUND_FILL : gcv_t< 90> { off = 0, color = 1, window = 2 };
		gcv< 90, eBACKGROUND_FILL> background_fill_color{eBACKGROUND_FILL::off};	// 90:
		color_bgra_t background_fill_color_value{};	// 420:
		gcv<430> background_fill_color_name{};
		gcv< 45> fill_box_scale{1.};
		gcv< 63> background_fill_color_index{};	// valid when groupcode 90 is 1
		gcv<441> transparency_na{};	// not implemented
		gcv< 75> column_type{};	// 0 : no columns, 1 : one column, 2 : continuous columns
		gcv< 76> column_count{};
		gcv< 78> column_flow_reversed{};
		gcv< 79> column_auto_height{};
		gcv< 48> column_width{};
		gcv< 49> column_gutter{};
		gcv< 50> column_heights{};	// this code is followed by column count (int16), and then the number of column heights

		BSC__DXF_ENTITY_DEFINITION(eENTITY::mtext, "MTEXT", xMText, xEntity);

		constexpr static inline auto const group_members = std::make_tuple(
			&this_t::marker,
			std::pair{10, BSC__LAMBDA_MEMBER_VALUE(pt.x)},
			std::pair{20, BSC__LAMBDA_MEMBER_VALUE(pt.y)},
			std::pair{30, BSC__LAMBDA_MEMBER_VALUE(pt.z)},
			&this_t::nominal_text_height,
			&this_t::reference_rectangle_width,
			&this_t::attachment_point,
			&this_t::drawing_direction,
			&this_t::text_style_name,
			std::pair{210, BSC__LAMBDA_MEMBER_VALUE(extrusion.x)},
			std::pair{220, BSC__LAMBDA_MEMBER_VALUE(extrusion.y)},
			std::pair{230, BSC__LAMBDA_MEMBER_VALUE(extrusion.z)},
			std::pair{ 11, BSC__LAMBDA_MEMBER_VALUE(direction.x)},
			std::pair{ 21, BSC__LAMBDA_MEMBER_VALUE(direction.y)},
			std::pair{ 31, BSC__LAMBDA_MEMBER_VALUE(direction.z)},
			&this_t::character_width,
			&this_t::character_height,
			&this_t::rotation,
			&this_t::line_spacing_style,
			&this_t::line_spacing_factor,
			&this_t::background_fill_color,
			std::pair{420, BSC__LAMBDA_MEMBER_VALUE(background_fill_color_value.Value())},
			&this_t::background_fill_color_name,
			&this_t::fill_box_scale,
			&this_t::background_fill_color_index,
			&this_t::transparency_na,
			&this_t::column_type,
			&this_t::column_count,
			&this_t::column_flow_reversed,
			&this_t::column_auto_height,
			&this_t::column_width,
			&this_t::column_gutter,
			&this_t::column_heights
		);

		bool ReadPrivate(group_iter_t& iter, group_iter_t const& end) {
			if (ReadStringTo(iter, end, text))
				return true;
			return false;
		}
	};

	//=============================================================================================================================
	class xMText_AttDef : public xMText {
	public:
		gcv< 46> defined_annotation_height{};	// valid before 2012
		gcv< 63> background_fill_color_index{};	// valid when groupcode 90 is 1

		BSC__DXF_ENTITY_DEFINITION(eENTITY::mtext_attdef, "MTEXT_ATTDEF", xMText_AttDef, xMText);

		constexpr static inline auto const group_members = std::make_tuple(
			&this_t::marker,
			std::pair{10, BSC__LAMBDA_MEMBER_VALUE(pt.x)},
			std::pair{20, BSC__LAMBDA_MEMBER_VALUE(pt.y)},
			std::pair{30, BSC__LAMBDA_MEMBER_VALUE(pt.z)},
			&this_t::nominal_text_height,
			&this_t::reference_rectangle_width,
			&this_t::defined_annotation_height,
			&this_t::attachment_point,
			&this_t::drawing_direction,
			&this_t::text_style_name,
			std::pair{210, BSC__LAMBDA_MEMBER_VALUE(extrusion.x)},
			std::pair{220, BSC__LAMBDA_MEMBER_VALUE(extrusion.y)},
			std::pair{230, BSC__LAMBDA_MEMBER_VALUE(extrusion.z)},
			std::pair{ 11, BSC__LAMBDA_MEMBER_VALUE(direction.x)},
			std::pair{ 21, BSC__LAMBDA_MEMBER_VALUE(direction.y)},
			std::pair{ 31, BSC__LAMBDA_MEMBER_VALUE(direction.z)},
			&this_t::character_width,
			&this_t::character_height,
			&this_t::rotation,
			&this_t::line_spacing_style,
			&this_t::line_spacing_factor,
			&this_t::background_fill_color,
			std::pair{420, BSC__LAMBDA_MEMBER_VALUE(background_fill_color_value.Value())},
			&this_t::background_fill_color_index,
			&this_t::background_fill_color_name,
			&this_t::fill_box_scale,
			&this_t::transparency_na,
			&this_t::column_type,
			&this_t::column_count,
			&this_t::column_flow_reversed,
			&this_t::column_auto_height,
			&this_t::column_width,
			&this_t::column_gutter,
			&this_t::column_heights
		);

	};


	//=============================================================================================================================
	class xAttributeDefinition : public xEntity {
	public:
		using this_t = xAttributeDefinition;
	public:
		xText text;
		gcv<100> marker;	// Subclass marker (AcDbAttribute Definition)
		gcv<280> version_number;	// 0 = 2010
		gcv<  3> prompt{};
		gcv<  2> tag{};
		enum fFLAG : gcv_t< 70> { fHIDDEN = 0x01, fCONSTANT = 0x02, fVERIFICATION_REQUIRED = 0x04, fPRESET = 0x08 };
		gcv< 70, fFLAG> flags{};	// 70:
		gcv< 73> field_length{};
		gcv< 74> vertical_text_justification{};
		gcv<280> lock_position{};
		gcv<100> markerRecord{};	// Subclass marker (AcDbXrecord)
		gcv<280> duplicate_record_cloning_flag{};	// 1 : keep existing
		enum fMTEXT_FLAG : gcv_t< 70> { fMTEXT_MULTILINE = 0x02, fMTEXT_CONSTANT_MULTILINE = 0x04 };
		gcv< 70, fMTEXT_FLAG> fMText{};	// 70:
		gcv< 70> is_really_locked{};	// 0 : unlocked, 1 : locked
		gcv< 70> number_of_secondary_attributes{};
		gcv<340> hard_pointer_id_of_secondary_attributes{};
		point_t pt_alignment_attribute{};	// alignment point for attribute
		gcv< 40> current_annotation_scale{ 1. };	//// no default value speicified
		gcv<  2> attribute;
		gcv<  0> entity_type_mtext;
		gcv<100> AcDbEntity;
		gcv< 67> space{};	// 0 : model space, 1 : paper space
		gcv<  8> layer_name{};
		xMText_AttDef mtext;

		constexpr static inline auto const group_members = std::make_tuple(
			&this_t::marker,
			&this_t::version_number,
			&this_t::prompt,
			&this_t::tag,
			&this_t::flags,
			&this_t::field_length,
			&this_t::vertical_text_justification,
			&this_t::lock_position,
			&this_t::markerRecord,
			&this_t::duplicate_record_cloning_flag,
			&this_t::fMText,
			&this_t::is_really_locked,
			&this_t::number_of_secondary_attributes,
			&this_t::hard_pointer_id_of_secondary_attributes,
			std::pair{10, BSC__LAMBDA_MEMBER_VALUE(pt_alignment_attribute.x)},
			std::pair{20, BSC__LAMBDA_MEMBER_VALUE(pt_alignment_attribute.y)},
			std::pair{30, BSC__LAMBDA_MEMBER_VALUE(pt_alignment_attribute.z)},
			&this_t::current_annotation_scale,
			&this_t::attribute,
			&this_t::entity_type_mtext
		);

		BSC__DXF_ENTITY_DEFINITION(eENTITY::attdef, "ATTDEF", xAttributeDefinition, xEntity);

		bool ReadPrivate(group_iter_t& iter, group_iter_t const& end) override {
			if (*iter == sGroup(100, "AcDbText")) {
				return text.Read(iter, end);
			}
			if (*iter == sGroup(100, "AcDbMText")) {
				return mtext.Read(iter, end);
			}
			return false;
		}
	};

	//=============================================================================================================================
	class xAttribute : public xEntity {
	public:
		using this_t = xAttribute;
	public:
		xText text;
		gcv<100> markerAttribute;	// Subclass marker (AcDbAttribute Definition)
		gcv<280> version_number;	// 0 = 2010
		gcv<  2> tag{};
		enum fFLAG : gcv_t< 70> { fHIDDEN = 0x01, fCONSTANT = 0x02, fVERIFICATION_REQUIRED = 0x04, fPRESET = 0x08 };
		gcv< 70, fFLAG> flags{};	// 70:
		gcv< 73> field_length{};
		gcv< 50> rotation_angle{};
		gcv< 41> relative_x_scale_factor{ 1. };
		gcv< 51> oblique_angle{};
		gcv<  7> text_style_name{ "STANDARD"s };
		gcv< 71> text_generation_flags{};
		gcv< 72> horizontal_justification{};
		gcv< 74> vertical_text_justification{};	// see groupcode 73 in TEXT
		point_t pt_alignment{};	// 11, 21, 31, alignment point for attribute
		point_t extrusion{ 0., 0., 1. };	// 210, 220, 230,
		gcv<280> lock_position{};	// 280, 0 : unlocked, 1 : locked

		gcv<100> markerXrecord{};	// Subclass marker (AcDbXrecord)
		gcv<280> duplicate_record_cloning_flag{};	// 1 : keep existing
		enum fMTEXT_FLAG : gcv_t< 70> { fMTEXT_MULTILINE = 0x02, fMTEXT_CONSTANT_MULTILINE = 0x04 };
		gcv< 70, fMTEXT_FLAG> fMText{};
		gcv< 70> is_really_locked{};	// 0 : unlocked, 1 : locked
		gcv< 70> number_of_secondary_attributes{};
		gcv<340> hard_pointer_id_of_secondary_attributes{};
		point_t pt_alignment_attribute{};	// alignment point for attribute
		gcv< 40> current_annotation_scale{ 1. };	//// no default value specified
		gcv<  2> attribute;
		xMText mtext;

		constexpr static inline auto const group_members = std::make_tuple(
			&this_t::markerAttribute,
			&this_t::version_number,
			&this_t::tag,
			&this_t::flags,
			&this_t::field_length,
			&this_t::rotation_angle,
			&this_t::relative_x_scale_factor,
			&this_t::oblique_angle,
			&this_t::text_style_name,
			&this_t::text_generation_flags,
			&this_t::horizontal_justification,
			&this_t::vertical_text_justification,
			std::pair{11, BSC__LAMBDA_MEMBER_VALUE(pt_alignment.x)},
			std::pair{21, BSC__LAMBDA_MEMBER_VALUE(pt_alignment.y)},
			std::pair{31, BSC__LAMBDA_MEMBER_VALUE(pt_alignment.z)},
			std::pair{210, BSC__LAMBDA_MEMBER_VALUE(extrusion.x)},
			std::pair{220, BSC__LAMBDA_MEMBER_VALUE(extrusion.y)},
			std::pair{230, BSC__LAMBDA_MEMBER_VALUE(extrusion.z)}
		);

		BSC__DXF_ENTITY_DEFINITION(eENTITY::attrib, "ATTRIB", xAttribute, xEntity);

		bool ReadPrivate(group_iter_t& iter, group_iter_t const& end) override {
			if (*iter == sGroup(100, "AcDbText")) {
				return text.Read(iter, end);
			}
			if (*iter == sGroup(0, "MTEXT")) {
				return mtext.Read(iter, end);
			}
			return false;
		}
	};

	//=============================================================================================================================
	class xBody : public xEntity {
	public:
		gcv<100> marker; // AcDbModelerGeometry
		gcv< 70> version{1};
		string_t proprietary_data;		// GroupCode 1, 3

		BSC__DXF_ENTITY_DEFINITION(eENTITY::body, "BODY", xBody, xEntity);

		bool ReadPrivate(group_iter_t& iter, group_iter_t const& end) {
			if (ReadStringTo(iter, end, proprietary_data))
				return true;
			return false;
		}

		constexpr static inline auto group_members = std::make_tuple(
			&this_t::marker,
			&this_t::version
		);
	};


	//=============================================================================================================================
	class xCircle : public xEntity {
	public:
		gcv<100> markerCircle{};	// Subclass marker (AcDbCircle)
		gcv< 39> thickness{};
		point_t pt;
		gcv< 40> radius;
		point_t extrusion;

		BSC__DXF_ENTITY_DEFINITION(eENTITY::circle, "CIRCLE", xCircle, xEntity);

		constexpr static inline auto group_members = std::make_tuple(
			&this_t::markerCircle,
			&this_t::thickness,
			std::pair{10, BSC__LAMBDA_MEMBER_VALUE(pt.x)},
			std::pair{20, BSC__LAMBDA_MEMBER_VALUE(pt.y)},
			std::pair{30, BSC__LAMBDA_MEMBER_VALUE(pt.z)},
			&this_t::radius,
			std::pair{210, BSC__LAMBDA_MEMBER_VALUE(extrusion.x)},
			std::pair{220, BSC__LAMBDA_MEMBER_VALUE(extrusion.y)},
			std::pair{230, BSC__LAMBDA_MEMBER_VALUE(extrusion.z)}
		);
	};

	//============================================================================================================================
	class xEllipse : public xEntity {
	public:
		gcv<100> markerEllipse; // Subclass marker (AcDbEllipse)
		gcv< 40> major_radius;
		gcv< 40> minor_radius;
		point_t extrusion;

		BSC__DXF_ENTITY_DEFINITION(eENTITY::ellipse, "ELLIPSE", xEllipse, xEntity);

		constexpr static inline auto group_members = std::make_tuple(
			&this_t::markerEllipse,
			&this_t::major_radius,
			&this_t::minor_radius,
			std::pair{210, BSC__LAMBDA_MEMBER_VALUE(extrusion.x)},
			std::pair{220, BSC__LAMBDA_MEMBER_VALUE(extrusion.y)},
			std::pair{230, BSC__LAMBDA_MEMBER_VALUE(extrusion.z)}
		);
	};

	//============================================================================================================================
	class xHatch : public xEntity {
	public:
		gcv<100> marker;
		point_t ptElevation;	// in OCS, x = y = 0, z : elevation
		point_t extrusion{0., 0., 1.};
		gcv<  2> pattern_name;
		gcv< 70> is_pattern_solid{};		// 0:pattern fill; 1=solid fill; for MPolygon, the version of MPolygon
		gcv< 63> pattern_fill_color_aci;	// for MPolygon
		gcv< 71> is_associative;			// boolean
		gcv< 91> number_boundary_path;
		std::vector<sGroup> boundary_path;
		enum class eHATCH_STYLE : gcv_t< 75> { odd_parity, outermost_area_only, through_entire_area };
		gcv< 75> hatch_style;
		enum class eHATCH_PATTERN_TYPE : gcv_t< 76> { user_defined = 0, predefined, custom };
		gcv< 76> pattern_type;
		gcv< 52> pattern_angle;
		gcv< 41> pattern_scale_or_spacing;	// pattern fill only
		gcv< 73> is_boundary_annotation;	// boolean
		gcv< 77> is_double_pattern;			// boolean

	};


	//=============================================================================================================================
	std::unique_ptr<xEntity> xEntity::CreateEntity(string_t const& name) {
		if (auto iter = m_mapEntityFactory.find(name); iter != m_mapEntityFactory.end() and iter->second)
			return iter->second();
		return std::make_unique<biscuit::dxf::entities::xUnknown>(name);
	}

}

