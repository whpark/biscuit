module;

//#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"
#include "macro.h"

export module biscuit.dxf:entities;
import std;
import fmt;
import Eigen;
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
		//group_code_039> thickness{};
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
		enum fHIDDEN : group_code_070_t { fHIDDEN_FIRST = 1, fHIDDEN_SECOND = 2, fHIDDEN_THIRD = 4, fHIDDEN_FOURTH = 8 };
		fHIDDEN flags{};	// 70:

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
		group_code_100_t marker;	// Subclass marker (AcDbModelerGeometry)
		group_code_070_t version{ 1 };
		string_t proprietary_data;		// GroupCode 1, 3
		group_code_100_t marker2;	// Subclass marker (AcDb3dSolid)
		group_code_350_t owner_id_handle_to_history_object{};

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
		group_code_100_t marker{};	// Subclass marker (AcDbProxyEntity)
		group_code_090_t proxy_entity_class_id{ 498 };
		group_code_091_t application_entity_class_id{ 500 };
		group_code_092_t size_graphics_data_in_bytes{};
		group_code_310_t graphics_data{};
		group_code_093_t size_entity_data_in_bits{};
		group_code_310_t entity_data{};
		group_code_330_t object_id0{};
		group_code_340_t object_id1{};
		group_code_350_t object_id2{};
		group_code_360_t object_id3{};
		group_code_094_t end_of_object_id_section{};
		group_code_095_t size_proxy_data_in_bytes{};
		group_code_070_t dwg_or_dxf{};	// Original custom object data format (0 = DWG, 1 = DXF)

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
		group_code_100_t markerCircle{};	// Subclass marker (AcDbCircle)
		group_code_039_t thickness{};
		point_t pt{};	// center
		group_code_040_t radius{};
		group_code_100_t marker{};	// Subclass marker (AcDbArc)
		group_code_050_t start_angle{};
		group_code_051_t end_angle{};
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
		group_code_100_t marker{};	// Subclass marker (AcDbPoint)
		point_t pt{};
		group_code_039_t thickness{};
		point_t extrusion{ 0., 0., 1. };
		group_code_050_t angle{};	// angle of x axis for UCS inf effect when the point was drawn (optional, default = 0); used when PDMODE is nonzero

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
		group_code_100_t marker;
		point_t pt0{};
		point_t pt1{};
		group_code_039_t thickness{};
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
		group_code_100_t marker{};	// Subclass marker (AcDbText)
		group_code_039_t thickness{};
		point_t pt_align0{};	// alignment point
		group_code_040_t text_height{};
		group_code_001_t text{};
		group_code_050_t rotation{};
		group_code_041_t relative_x_scale_factor{1.};
		group_code_051_t oblique_angle{};
		group_code_007_t text_style_name{"STANDARD"s};
		enum fTEXT_GENERATION_FLAGS : group_code_071_t { fTEXT_GENERATION_FLAGS__BACKWARD = 2, fTEXT_GENERATION_FLAGS__UPSIDE_DOWN = 4 };
		fTEXT_GENERATION_FLAGS text_generation_flags{};	// 71:
		enum class eHORIZONTAL_TEXT_JUSTIFICATION : group_code_072_t { left, center, right, aligned, middle, fit };
		eHORIZONTAL_TEXT_JUSTIFICATION horizontal_justification{};	// 72:
		point_t pt_align1{};	// second alignment point
		point_t extrusion{ 0., 0., 1. };
		group_code_100_t marker2;	// Subclass marker (AcDbText)
		enum class eVERTICAL_ALIGNMENT : group_code_073_t { baseline, bottom, middle, top};
		eVERTICAL_ALIGNMENT vertical_alignment{};	// 73:

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
		group_code_100_t marker{};	// Subclass marker (AcDbMText)
		point_t pt;	// insertion point
		group_code_040_t nominal_text_height{};
		group_code_041_t reference_rectangle_width{};
		group_code_046_t defined_annotation_height{};	// valid before 2012

		enum class eTEXT_ATTACHMENT_POINT : group_code_071_t {
			top_left = 1, top_center, top_right,
			middle_left, middle_center, middle_right,
			bottom_left, bottom_center, bottom_right
		};
		eTEXT_ATTACHMENT_POINT attachment_point{eTEXT_ATTACHMENT_POINT::top_left};	// 71

		enum class eDRAWING_DIRECTION : group_code_072_t {
			left_to_right = 1, top_to_bottom = 3, by_style = 5
		};
		eDRAWING_DIRECTION drawing_direction{eDRAWING_DIRECTION::left_to_right};	// 72

		string_t text;	// group code 1, 3

		group_code_007_t text_style_name{"STANDARD"s};
		point_t extrusion{ 0., 0., 1. };
		point_t direction{};	// 11, 21, 31
		group_code_042_t character_width{};
		group_code_043_t character_height{};
		group_code_050_t rotation{};	// in radian.
		enum class eLINE_SPACING_STYLE : group_code_073_t { at_least = 1, exact = 2 };
		eLINE_SPACING_STYLE line_spacing_style{eLINE_SPACING_STYLE::exact};	// 73:
		group_code_044_t line_spacing_factor{ 1. };	// 0.25 ~ 4.0
		enum class eBACKGROUND_FILL : group_code_090_t { off = 0, color = 1, window = 2 };
		eBACKGROUND_FILL background_fill_color{eBACKGROUND_FILL::off};	// 90:
		color_bgra_t background_fill_color_value{};	// 420:
		group_code_430_t background_fill_color_name{};
		group_code_045_t fill_box_scale{1.};
		group_code_063_t background_fill_color_index{};	// valid when groupcode 90 is 1
		group_code_441_t transparency_na{};	// not implemented
		group_code_075_t column_type{};	// 0 : no columns, 1 : one column, 2 : continuous columns
		group_code_076_t column_count{};
		group_code_078_t column_flow_reversed{};
		group_code_079_t column_auto_height{};
		group_code_048_t column_width{};
		group_code_049_t column_gutter{};
		group_code_050_t column_heights{};	// this code is followed by column count (int16), and then the number of column heights

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
		group_code_100_t marker;	// Subclass marker (AcDbAttribute Definition)
		group_code_280_t version_number;	// 0 = 2010
		group_code_003_t prompt{};
		group_code_002_t tag{};
		enum fFLAG : group_code_070_t { fHIDDEN = 0x01, fCONSTANT = 0x02, fVERIFICATION_REQUIRED = 0x04, fPRESET = 0x08 };
		fFLAG flags{};	// 70:
		group_code_073_t field_length{};
		group_code_074_t vertical_text_justification{};
		group_code_280_t lock_position{};
		group_code_100_t markerRecord{};	// Subclass marker (AcDbXrecord)
		group_code_280_t duplicate_record_cloning_flag{};	// 1 : keep existing
		enum fMTEXT_FLAG : group_code_070_t { fMTEXT_MULTILINE = 0x02, fMTEXT_CONSTANT_MULTILINE = 0x04 };
		fMTEXT_FLAG fMText{};	// 70:
		group_code_070_t is_really_locked{};	// 0 : unlocked, 1 : locked
		group_code_070_t number_of_secondary_attributes{};
		group_code_340_t hard_pointer_id_of_secondary_attributes{};
		point_t pt_alignment_attribute{};	// alignment point for attribute
		group_code_040_t current_annotation_scale{ 1. };	//// no default value speicified
		group_code_002_t attribute;
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
		group_code_100_t markerText{};	// Subclass marker (AcDbText)
		xText text;
		group_code_100_t marker;	// Subclass marker (AcDbAttribute Definition)
		group_code_280_t version_number;	// 0 = 2010
		group_code_002_t tag{};
		enum fFLAG : group_code_070_t { fHIDDEN = 0x01, fCONSTANT = 0x02, fVERIFICATION_REQUIRED = 0x04, fPRESET = 0x08 };
		fFLAG flags{};	// 70:
		group_code_073_t field_length{};
		group_code_050_t rotation_angle{};
		group_code_041_t relative_x_scale_factor{ 1. };
		group_code_051_t oblique_angle{};
		group_code_007_t text_style_name{ "STANDARD"s };
		group_code_071_t text_generation_flags{};
		group_code_072_t horizontal_justification{};
		group_code_074_t vertical_text_justification{};	// see groupcode 73 in TEXT
		point_t pt_alignment{};	// 11, 21, 31, alignment point for attribute
		point_t extrusion{ 0., 0., 1. };	// 210, 220, 230,
		group_code_280_t lock_position{};	// 280, 0 : unlocked, 1 : locked

		//group_code_100> markerXrecord{};	// Subclass marker (AcDbXrecord)
		//group_code_280> duplicate_record_cloning_flag{};	// 1 : keep existing
		//enum fMTEXT_FLAG : group_code_070> { fMTEXT_MULTILINE = 0x02, fMTEXT_CONSTANT_MULTILINE = 0x04 };
		//group_code_070, fMTEXT_FLAG> fMText{};
		//group_code_070> is_really_locked{};	// 0 : unlocked, 1 : locked
		//group_code_070> number_of_secondary_attributes{};
		//group_code_340> hard_pointer_id_of_secondary_attributes{};
		//point_t pt_alignment_attribute{};	// alignment point for attribute
		//group_code_040> current_annotation_scale{ 1. };	//// no default value speicified
		//group_code_002> attribute;
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
	std::unique_ptr<xEntity> xEntity::CreateEntity(string_t const& name) {
		if (auto iter = m_mapEntityFactory.find(name); iter != m_mapEntityFactory.end() and iter->second)
			return iter->second();
		return std::make_unique<biscuit::dxf::entities::xUnknown>(name);
	}

}

