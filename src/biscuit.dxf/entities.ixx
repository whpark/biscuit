module;

#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"
#include "macro.h"

export module biscuit.dxf:entities;
import std;
import biscuit;
import :group;
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

	enum class eSPACE : int16 { model, paper };
	enum class eCOLOR : int16 { byBlock = 0, byLayer = 256, };

	using point_t = Eigen::Vector3d;

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
	class xEntity : public subclass::sEntity {
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

		//point_t extrusion{0., 0., 1.};
		//code_to_value_t< 39> thickness{};
		constexpr static inline auto group_members = std::make_tuple(
			0
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
			return ReadEntity<subclass::sEntity, true>(*this, iter, end);
		}

		virtual bool ReadControlData(group_iter_t& iter, group_iter_t const& end) {
			if (iter->eCode != group_code_t::control)
				return false;
			static sGroup const groupEnd{ 102, "}"s };
			string_t str = iter->GetValue<string_t>().value_or(""s);
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
	};


	using entity_ptr_t = TCloneablePtr<xEntity>;
	using entities_t = std::deque<entity_ptr_t>;

	//============================================================================================================================
	class xUnknown : public xEntity {
	public:

		constexpr static inline auto group_members = std::make_tuple(
		);

		string_t m_name;
		std::vector<sGroup> groups;
		BSC__DXF_ENTITY_DEFINITION(eENTITY::unknown, "UNKNOWN", xUnknown, xEntity);
		xUnknown(string_t name) : m_name(std::move(name)) {}

		bool ReadPrivate(group_iter_t& iter, group_iter_t const& end) override {
			groups.push_back(*iter);
			return true;
		}
	};

	//============================================================================================================================
	class x3DFace : public xEntity {
	public:
		point_t pt1, pt2, pt3, pt4;
		enum fHIDDEN : code_to_value_t<70> { fHIDDEN_FIRST = 1, fHIDDEN_SECOND = 2, fHIDDEN_THIRD = 4, fHIDDEN_FOURTH = 8 };
		code_to_value_t<70, fHIDDEN> flags{};

		constexpr static inline auto group_members = std::make_tuple(
			10, BSC__LAMBDA_MEMBER_VALUE(pt1.x()), 20, BSC__LAMBDA_MEMBER_VALUE(pt1.y()), 30, BSC__LAMBDA_MEMBER_VALUE(pt1.z()),
			11, BSC__LAMBDA_MEMBER_VALUE(pt2.x()), 21, BSC__LAMBDA_MEMBER_VALUE(pt2.y()), 31, BSC__LAMBDA_MEMBER_VALUE(pt2.z()),
			12, BSC__LAMBDA_MEMBER_VALUE(pt3.x()), 22, BSC__LAMBDA_MEMBER_VALUE(pt3.y()), 32, BSC__LAMBDA_MEMBER_VALUE(pt3.z()),
			13, BSC__LAMBDA_MEMBER_VALUE(pt4.x()), 23, BSC__LAMBDA_MEMBER_VALUE(pt4.y()), 33, BSC__LAMBDA_MEMBER_VALUE(pt4.z()),
			70, &x3DFace::flags,
			0
		);

		BSC__DXF_ENTITY_DEFINITION(eENTITY::_3dface, "3DFACE", x3DFace, xEntity);

	};

	//============================================================================================================================
	class x3DSolid : public xEntity {
	public:
		code_to_value_t<100> marker;	// Subclass marker (AcDbModelerGeometry)
		code_to_value_t< 70> version{ 1 };
		string_t proprietary_data;		// GroupCode 1, 3
		code_to_value_t<100> marker2;	// Subclass marker (AcDb3dSolid)
		code_to_value_t<350> owner_id_handle_to_history_object{};

		constexpr static inline auto group_members = std::make_tuple(
			100, &x3DSolid::marker,
			70, &x3DSolid::version,
			100, &x3DSolid::marker2,
			350, &x3DSolid::owner_id_handle_to_history_object
		);

		BSC__DXF_ENTITY_DEFINITION(eENTITY::_3dsolid, "3DSOLID", x3DSolid, xEntity);

		bool ReadPrivate(group_iter_t& iter, group_iter_t const& end) override {
			if (iter->eCode == 1 or iter->eCode == 3) {
				proprietary_data += iter->GetValue<string_t>().value_or(""s);
				iter++;
			}
			return true;
		}
	};
	//=============================================================================================================================
	class xACADProxyEntity : public xEntity {
	public:
		code_to_value_t<100> marker{};	// Subclass marker (AcDbProxyEntity)
		code_to_value_t< 90> proxy_entity_class_id{ 498 };
		code_to_value_t< 91> application_entity_class_id{ 500 };
		code_to_value_t< 92> size_graphics_data_in_bytes{};
		code_to_value_t<310> graphics_data{};
		code_to_value_t< 93> size_entity_data_in_bits{};
		code_to_value_t<310> entity_data{};
		code_to_value_t<330> object_id0{};
		code_to_value_t<340> object_id1{};
		code_to_value_t<350> object_id2{};
		code_to_value_t<360> object_id3{};
		code_to_value_t< 94> end_of_object_id_section{};
		code_to_value_t< 95> size_proxy_data_in_bytes{};
		code_to_value_t< 70> dwg_or_dxf{};	// Original custom object data format (0 = DWG, 1 = DXF)

		using T = xACADProxyEntity;
		constexpr static inline auto group_members = std::make_tuple(
			100, &T::marker,
			90, &T::proxy_entity_class_id,
			91, &T::application_entity_class_id,
			92, &T::size_graphics_data_in_bytes,
			310, &T::graphics_data,
			93, &T::size_entity_data_in_bits,
			310, &T::entity_data,
			330, &T::object_id0,
			340, &T::object_id1,
			350, &T::object_id2,
			360, &T::object_id3,
			94, &T::end_of_object_id_section,
			95, &T::size_proxy_data_in_bytes,
			70, &T::dwg_or_dxf
		);

		BSC__DXF_ENTITY_DEFINITION(eENTITY::acad_proxy_entity, "ACAD_PROXY_ENTITY", xACADProxyEntity, xEntity)
	};

	//=============================================================================================================================
	class xArc : public xEntity {
	public:
		code_to_value_t<100> markerCircle{};	// Subclass marker (AcDbCircle)
		code_to_value_t< 39> thickness{};
		point_t pt{};	// center
		code_to_value_t< 40> radius{};
		code_to_value_t<100> marker{};	// Subclass marker (AcDbArc)
		code_to_value_t< 50> start_angle{};
		code_to_value_t< 51> end_angle{};
		point_t extrusion{ 0., 0., 1. };
		BSC__DXF_ENTITY_DEFINITION(eENTITY::arc, "ARC", xArc, xEntity);

		using T = xArc;
		constexpr static inline auto const group_members = std::make_tuple(
			100, &T::markerCircle,
			 39, &T::thickness,
			 10, BSC__LAMBDA_MEMBER_VALUE(pt.x()),
			 20, BSC__LAMBDA_MEMBER_VALUE(pt.y()),
			 30, BSC__LAMBDA_MEMBER_VALUE(pt.z()),
			 40, &T::radius,
			100, &T::marker,
			 50, &T::start_angle,
			 51, &T::end_angle,
			210, BSC__LAMBDA_MEMBER_VALUE(extrusion.x()),
			220, BSC__LAMBDA_MEMBER_VALUE(extrusion.y()),
			230, BSC__LAMBDA_MEMBER_VALUE(extrusion.z())
		);
	};


	//=============================================================================================================================
	class xPoint : public xEntity {
	public:
		code_to_value_t<100> marker{};	// Subclass marker (AcDbPoint)
		point_t pt{};
		code_to_value_t< 39> thickness{};
		point_t extrusion{ 0., 0., 1. };
		code_to_value_t< 50> angle{};	// angle of x axis for UCS inf effect when the point was drawn (optional, default = 0); used when PDMODE is nonzero

		constexpr static inline auto group_members = std::make_tuple(
			100, &xPoint::marker,
			10, BSC__LAMBDA_MEMBER_VALUE(pt.x()), 20, BSC__LAMBDA_MEMBER_VALUE(pt.y()), 30, BSC__LAMBDA_MEMBER_VALUE(pt.z()),
			39, &xPoint::thickness,
			210, BSC__LAMBDA_MEMBER_VALUE(extrusion.x()), 220, BSC__LAMBDA_MEMBER_VALUE(extrusion.y()), 230, BSC__LAMBDA_MEMBER_VALUE(extrusion.z()),
			50, &xPoint::angle
		);

		BSC__DXF_ENTITY_DEFINITION(eENTITY::point, "POINT", xPoint, xEntity)
	};

	//=============================================================================================================================
	class xLine : public xEntity {
	public:
		code_to_value_t<100> marker;
		point_t pt0{};
		point_t pt1{};
		code_to_value_t< 39> thickness{};
		point_t extrusion{ 0., 0., 1. };
		using T = xLine;
		constexpr static inline auto group_members = std::make_tuple(
			100, &T::marker,
			39, &T::thickness,
			10, BSC__LAMBDA_MEMBER_VALUE(pt0.x()),
			20, BSC__LAMBDA_MEMBER_VALUE(pt0.y()),
			30, BSC__LAMBDA_MEMBER_VALUE(pt0.z()),
			11, BSC__LAMBDA_MEMBER_VALUE(pt1.x()),
			21, BSC__LAMBDA_MEMBER_VALUE(pt1.y()),
			31, BSC__LAMBDA_MEMBER_VALUE(pt1.z()),
			210, BSC__LAMBDA_MEMBER_VALUE(extrusion.x()),
			220, BSC__LAMBDA_MEMBER_VALUE(extrusion.y()),
			230, BSC__LAMBDA_MEMBER_VALUE(extrusion.z())
		);

		BSC__DXF_ENTITY_DEFINITION(eENTITY::line, "LINE", xLine, xEntity)
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
		code_to_value_t<100> marker{};	// Subclass marker (AcDbText)
		code_to_value_t< 39> thickness{};
		point_t pt_align0{};	// alignment point
		code_to_value_t< 40> text_height{};
		code_to_value_t<  1> text{};
		code_to_value_t< 50> rotation{};
		code_to_value_t< 41> relative_x_scale_factor{1.};
		code_to_value_t< 51> oblique_angle{};
		code_to_value_t<  7> text_style_name{"STANDARD"s};
		enum fTEXT_GENERATION_FLAGS : code_to_value_t< 71> { fTEXT_GENERATION_FLAGS__BACKWARD = 2, fTEXT_GENERATION_FLAGS__UPSIDE_DOWN = 4 };
		code_to_value_t< 71, fTEXT_GENERATION_FLAGS> text_generation_flags{};
		enum class eHORIZONTAL_TEXT_JUSTIFICATION : code_to_value_t<72> { left, center, right, aligned, middle, fit };
		code_to_value_t< 72, eHORIZONTAL_TEXT_JUSTIFICATION> horizontal_justification{};
		point_t pt_align1{};	// second alignment point
		point_t extrusion{ 0., 0., 1. };
		code_to_value_t<100> marker2;	// Subclass marker (AcDbText)
		enum class eVERTICAL_ALIGNMENT : code_to_value_t<73> { baseline, bottom, middle, top};
		code_to_value_t< 73, eVERTICAL_ALIGNMENT> vertical_alignment{};

		using T = xText;
		constexpr static inline auto const group_members = std::make_tuple(
			100, &T::marker,
			39, &T::thickness,
			10, BSC__LAMBDA_MEMBER_VALUE(pt_align0.x()), 20, BSC__LAMBDA_MEMBER_VALUE(pt_align0.y()), 30, BSC__LAMBDA_MEMBER_VALUE(pt_align0.z()),
			40, &T::text_height,
			1, &T::text,
			50, &T::rotation,
			41, &T::relative_x_scale_factor,
			51, &T::oblique_angle,
			7, &T::text_style_name,
			71, &T::text_generation_flags,
			72, &T::horizontal_justification,
			11, BSC__LAMBDA_MEMBER_VALUE(pt_align1.x()), 21, BSC__LAMBDA_MEMBER_VALUE(pt_align1.y()), 31, BSC__LAMBDA_MEMBER_VALUE(pt_align1.z()),
			210, BSC__LAMBDA_MEMBER_VALUE(extrusion.x()), 220, BSC__LAMBDA_MEMBER_VALUE(extrusion.y()), 230, BSC__LAMBDA_MEMBER_VALUE(extrusion.z()),
			100, &T::marker,
			73, &T::vertical_alignment
		);

		BSC__DXF_ENTITY_DEFINITION(eENTITY::text, "TEXT", xText, xEntity)

	};

	//=============================================================================================================================
	class xMText : public xEntity {
	public:
		code_to_value_t<100> marker{};	// Subclass marker (AcDbMText)
		point_t pt;	// insertion point
		code_to_value_t< 40> nominal_text_height{};
		code_to_value_t< 41> reference_rectangle_width{};
		code_to_value_t< 46> defined_annotation_height{};	// valid before 2012

		enum class eTEXT_ATTACHMENT_POINT : code_to_value_t< 71> {
			top_left = 1, top_center, top_right,
			middle_left, middle_center, middle_right,
			bottom_left, bottom_center, bottom_right
		};
		code_to_value_t< 71, eTEXT_ATTACHMENT_POINT> attachment_point{eTEXT_ATTACHMENT_POINT::top_left};

		enum class eDRAWING_DIRECTION : code_to_value_t< 72> {
			left_to_right = 1, top_to_bottom = 3, by_style = 5
		};
		code_to_value_t< 72, eDRAWING_DIRECTION> drawing_direction{eDRAWING_DIRECTION::left_to_right};

		string_t text;	// group code 1, 3

		code_to_value_t<  7> text_style_name{"STANDARD"s};
		point_t extrusion{ 0., 0., 1. };
		point_t direction{};	// 11, 21, 31
		code_to_value_t< 42> character_width{};
		code_to_value_t< 43> character_height{};
		code_to_value_t< 50> rotation{};	// in radian.
		enum class eLINE_SPACING_STYLE : code_to_value_t<73> { at_least = 1, exact = 2 };
		code_to_value_t< 73, eLINE_SPACING_STYLE> line_spacing_style{eLINE_SPACING_STYLE::exact};
		code_to_value_t< 44> line_spacing_factor{ 1. };	// 0.25 ~ 4.0
		enum class eBACKGROUND_FILL : code_to_value_t< 90> { off = 0, color = 1, window = 2 };
		code_to_value_t< 90, eBACKGROUND_FILL> background_fill_color{eBACKGROUND_FILL::off};
		code_to_value_t<420, color_bgra_t> background_fill_color_value{};
		code_to_value_t<430> background_fill_color_name{};
		code_to_value_t< 45> fill_box_scale{1.};
		code_to_value_t< 63> background_fill_color_index{};	// valid when groupcode 90 is 1
		code_to_value_t<441> transparency_na{};	// not implemented
		code_to_value_t< 75> column_type{};	// 0 : no columns, 1 : one column, 2 : continuous columns
		code_to_value_t< 76> column_count{};
		code_to_value_t< 78> column_flow_reversed{};
		code_to_value_t< 79> column_auto_height{};
		code_to_value_t< 48> column_width{};
		code_to_value_t< 49> column_gutter{};
		code_to_value_t< 50> column_heights{};	// this code is followed by column count (int16), and then the number of column heights

		using T = xMText;
		constexpr static inline auto const group_members = std::make_tuple(
			100, &T::marker,
			10, BSC__LAMBDA_MEMBER_VALUE(pt.x()), 20, BSC__LAMBDA_MEMBER_VALUE(pt.y()), 30, BSC__LAMBDA_MEMBER_VALUE(pt.z()),
			40, &T::nominal_text_height,
			41, &T::reference_rectangle_width,
			46, &T::defined_annotation_height,
			71, &T::attachment_point,
			72, &T::drawing_direction,
			7, &T::text_style_name,
			210, BSC__LAMBDA_MEMBER_VALUE(extrusion.x()), 220, BSC__LAMBDA_MEMBER_VALUE(extrusion.y()), 230, BSC__LAMBDA_MEMBER_VALUE(extrusion.z()),
			11, BSC__LAMBDA_MEMBER_VALUE(direction.x()), 21, BSC__LAMBDA_MEMBER_VALUE(direction.y()), 31, BSC__LAMBDA_MEMBER_VALUE(direction.z()),
			42, &T::character_width,
			43, &T::character_height,
			50, &T::rotation,
			73, &T::line_spacing_style,
			44, &T::line_spacing_factor,
			90, &T::background_fill_color,
			420, BSC__LAMBDA_MEMBER_VALUE(background_fill_color_value.Value()),
			430, &T::background_fill_color_name,
			45, &T::fill_box_scale,
			63, &T::background_fill_color_index,
			441, &T::transparency_na,
			75, &T::column_type,
			76, &T::column_count,
			78, &T::column_flow_reversed,
			79, &T::column_auto_height,
			48, &T::column_width,
			49, &T::column_gutter,
			50, &T::column_heights,

			0
		);

		BSC__DXF_ENTITY_DEFINITION(eENTITY::mtext, "MTEXT", xMText, xEntity);

		bool ReadPrivate(group_iter_t& iter, group_iter_t const& end) {
			if (iter->eCode == 1 or iter->eCode == 3) {
				text += iter->GetValue<string_t>().value_or(""s);
				//iter++;
				return true;
			}
			return false;
		}
	};

	//=============================================================================================================================
	class xAttributeDefinition : public xEntity {
	public:
		xText text;
		code_to_value_t<100> marker;	// Subclass marker (AcDbAttribute Definition)
		code_to_value_t<280> version_number;	// 0 = 2010
		code_to_value_t<  3> prompt{};
		code_to_value_t<  2> tag{};
		enum fFLAG : code_to_value_t< 70> { fHIDDEN = 0x01, fCONSTANT = 0x02, fVERIFICATION_REQUIRED = 0x04, fPRESET = 0x08 };
		code_to_value_t< 70, fFLAG> flags{};
		code_to_value_t< 73> field_length{};
		code_to_value_t< 74> vertical_text_justification{};
		code_to_value_t<280> lock_position{};
		code_to_value_t<100> markerRecord{};	// Subclass marker (AcDbXrecord)
		code_to_value_t<280> duplicate_record_cloning_flag{};	// 1 : keep existing
		enum fMTEXT_FLAG : code_to_value_t< 70> { fMTEXT_MULTILINE = 0x02, fMTEXT_CONSTANT_MULTILINE = 0x04 };
		code_to_value_t< 70, fMTEXT_FLAG> fMText{};
		code_to_value_t< 70> is_really_locked{};	// 0 : unlocked, 1 : locked
		code_to_value_t< 70> number_of_secondary_attributes{};
		code_to_value_t<340> hard_pointer_id_of_secondary_attributes{};
		point_t pt_alignment_attribute{};	// alignment point for attribute
		code_to_value_t< 40> current_annotation_scale{ 1. };	//// no default value speicified
		code_to_value_t<  2> attribute;
		xMText mtext;

		using T = xAttributeDefinition;
		constexpr static inline auto const group_members = std::make_tuple(
			100, &T::marker,
			280, &T::version_number,
			3, &T::prompt,
			2, &T::tag,
			70, &T::flags,
			73, &T::field_length,
			74, &T::vertical_text_justification,
			280, &T::lock_position,
			100, &T::markerRecord,
			280, &T::duplicate_record_cloning_flag,
			70, &T::fMText,
			70, &T::is_really_locked,
			70, &T::number_of_secondary_attributes,
			340, &T::hard_pointer_id_of_secondary_attributes,
			10, BSC__LAMBDA_MEMBER_VALUE(pt_alignment_attribute.x()), 20, BSC__LAMBDA_MEMBER_VALUE(pt_alignment_attribute.y()), 30, BSC__LAMBDA_MEMBER_VALUE(pt_alignment_attribute.z()),
			40, &T::current_annotation_scale,
			2, &T::attribute
		);

		BSC__DXF_ENTITY_DEFINITION(eENTITY::attdef, "ATTDEF", xAttributeDefinition, xEntity);

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
	class xAttribute : public xEntity {
	public:
		//code_to_value_t<100> markerText{};	// Subclass marker (AcDbText)
		//code_to_value_t< 39> thickness{};
		//point_t pt_align0{};	// alignment point
		//code_to_value_t< 40> text_height{};
		//code_to_value_t<  1> text{};
		//code_to_value_t< 50> rotation{};
		//code_to_value_t< 41> relativ_x_scale_factor{1.};
		//code_to_value_t< 51> oblique_angle{};
		//code_to_value_t<  7> text_style_name{"STANDARD"s};
		//code_to_value_t< 71> text_generation_flags{};
		//code_to_value_t< 72> horizontal_justification{};
		//point_t pt_align1{};	// second alignment point
		//point_t extrusion{ 0., 0., 1. };
		code_to_value_t<100> marker;	// Subclass marker (AcDbAttribute Definition)
		code_to_value_t<280> version_number;	// 0 = 2010
		code_to_value_t<  2> tag{};
		enum fFLAG : code_to_value_t< 70> { fHIDDEN = 0x01, fCONSTANT = 0x02, fVERIFICATION_REQUIRED = 0x04, fPRESET = 0x08 };
		code_to_value_t< 70, fFLAG> flags{};
		code_to_value_t< 73> field_length{};
		code_to_value_t< 50> rotation_angle{};
		code_to_value_t< 41> relative_x_scale_factor{ 1. };
		code_to_value_t< 51> oblique_angle{};
		code_to_value_t<  7> text_style_name{ "STANDARD"s };
		code_to_value_t< 71> text_generation_flags{};
		code_to_value_t< 72> horizontal_justification{};
		code_to_value_t< 74> vertical_text_justification{};	// see groupcode 73 in TEXT
		point_t pt_alignment{};	// 11, 21, 31, alignment point for attribute
		point_t extrusion{ 0., 0., 1. };	// 210, 220, 230,
		code_to_value_t<280> lock_position{};	// 280, 0 : unlocked, 1 : locked

		//code_to_value_t<100> markerXrecord{};	// Subclass marker (AcDbXrecord)
		//code_to_value_t<280> duplicate_record_cloning_flag{};	// 1 : keep existing
		//enum fMTEXT_FLAG : code_to_value_t< 70> { fMTEXT_MULTILINE = 0x02, fMTEXT_CONSTANT_MULTILINE = 0x04 };
		//code_to_value_t< 70, fMTEXT_FLAG> fMText{};
		//code_to_value_t< 70> is_really_locked{};	// 0 : unlocked, 1 : locked
		//code_to_value_t< 70> number_of_secondary_attributes{};
		//code_to_value_t<340> hard_pointer_id_of_secondary_attributes{};
		//point_t pt_alignment_attribute{};	// alignment point for attribute
		//code_to_value_t< 40> current_annotation_scale{ 1. };	//// no default value speicified
		//code_to_value_t<  2> attribute;
		xMText mtext;

		using T = xAttribute;
		constexpr static inline auto const group_members = std::make_tuple(
			100, &T::marker,
			280, &T::version_number,
			2, &T::tag,
			70, &T::flags,
			73, &T::field_length,
			50, &T::rotation_angle,
			41, &T::relative_x_scale_factor,
			51, &T::oblique_angle,
			7, &T::text_style_name,
			71, &T::text_generation_flags,
			72, &T::horizontal_justification,
			74, &T::vertical_text_justification,
			11, BSC__LAMBDA_MEMBER_VALUE(pt_alignment.x()), 21, BSC__LAMBDA_MEMBER_VALUE(pt_alignment.y()), 31, BSC__LAMBDA_MEMBER_VALUE(pt_alignment.z()),
			210, BSC__LAMBDA_MEMBER_VALUE(extrusion.x()), 220, BSC__LAMBDA_MEMBER_VALUE(extrusion.y()), 230, BSC__LAMBDA_MEMBER_VALUE(extrusion.z()),
			0
		);

		BSC__DXF_ENTITY_DEFINITION(eENTITY::attrib, "ATTRIB", xAttribute, xEntity);

		bool ReadPrivate(group_iter_t& iter, group_iter_t const& end) override {
			if (*iter == sGroup(0, "MTEXT")) {
				return mtext.Read(iter, end);
			}
			return false;
		}
	};

	//=============================================================================================================================
	std::unique_ptr<xEntity> xEntity::CreateEntity(string_t const& name) {
		if (auto iter = m_mapEntityFactory.find(name); iter != m_mapEntityFactory.end() and iter->second)
			return iter->second();
		return std::make_unique<biscuit::dxf::entities::xUnknown>(name);
	}

}

