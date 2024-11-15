module;

#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"

export module biscuit.dxf:entities;
import std;
import biscuit;
import :group;

using namespace std::literals;
using namespace biscuit::literals;

#define BSC__DXF_ENTITY_DEFINITION(ENTITY, ENTITY_STRING, CLASS_NAME, BASE_NAME)\
	public:\
		using base_t = BASE_NAME;\
		using this_t = CLASS_NAME;\
		CLASS_NAME() = default;\
		CLASS_NAME(CLASS_NAME const&) = default;\
		CLASS_NAME(CLASS_NAME&&) = default;\
		CLASS_NAME& operator=(CLASS_NAME const&) = default;\
		CLASS_NAME& operator=(CLASS_NAME&&) = default;\
		virtual ~CLASS_NAME() = default;\
		bool operator == (this_t const&) const = default;\
		bool operator != (this_t const&) const = default;\
		auto operator <=> (this_t const&) const = default;\
		base_t& base() { return static_cast<base_t&>(*this); }\
		base_t const& base() const { return static_cast<base_t const&>(*this); }\
		eENTITY GetEntityType() const override { return eENTITY::ENTITY; }\
		std::unique_ptr<this_t::root_t> clone() const override { return std::make_unique<this_t>(*this); }\
		static std::unique_ptr<this_t::root_t> create() { return std::make_unique<this_t>(); }\
		bool Compare(xEntity const& other) const override {\
			if (!base_t::Compare(other))\
				return false;\
			if (auto const* p = dynamic_cast<this_t const*>(&other)) \
				return *this == *p;\
			return false;\
		}\
	private:\
		static inline xRegisterEntity s_registerEntity_##ENTITY{ENTITY_STRING, &this_t::create};\
	public:\

			//[]() -> std::unique_ptr<xEntity> { return std::make_unique<entity>(); });

#define BSC__LAMBDA_MEMBER_VALUE(member)  [](auto& self) -> decltype(auto) { return self.member; }

export namespace biscuit::dxf {

	//=============================================================================================================================

	enum class eENTITY {
		none,
		unknown,
		_3dface,
		point,
		line,
		ray,
		xline,
		circle,
		arc,
		ellipse,
		lw_polyline,
		polyline,
		spline,
		text,
		mtext,
		dimension,
		hatch,
		image,
		leader,
		mleader,
		mleader_style,
		mtext_style,
		plot_settings,
		viewport,
	};

	enum class eSPACE : int16 { model, paper };
	enum class eCOLOR : int16 { byBlock = 0, byLayer = 256, };

	using point_t = Eigen::Vector3d;

	//-------------------------------------------------------------------------
	class xEntity {
	public:
		using root_t = xEntity;
		using this_t = xEntity;

	public:
		code_to_value_t<102> handle{};
		std::vector<sGroup> app_data;				// 102:{application_name ... 102:}
		code_to_value_t<330> hReactor;				// 102:{ACD_REACTORS 330:value 102:}
		code_to_value_t<330> hOwner;				// 102:{ACAD_XDICTIONARY 330:owner_handle 102:}
		code_to_value_t<330> hOwnerBlock;
		code_to_value_t<100> marker;				// 100:AcDbEntity (SubclassMarker)
		code_to_value_t< 67, eSPACE> space{};		// 0 for model, 1 for paper
		code_to_value_t<410> layout_tab_name;
		code_to_value_t<  8> layer;
		code_to_value_t<  6> line_type_name;
		code_to_value_t<347> ptrMaterial;
		code_to_value_t< 62, eCOLOR> color{ 256 };	// 62:color, 0 for ByBlock, 256 for ByLayer, negative value indicates layer is off.
		code_to_value_t<370> line_weight{};			// 370: Stored and moved around as a 16-bit integer (?)
		code_to_value_t< 48> line_type_scale{1.0};	// 48: optional
		code_to_value_t< 60> hidden{0};				// 60: 0: visible, 1: invisible
		code_to_value_t< 92> size_graphics_data{};
		code_to_value_t<310> graphics_data{};
		code_to_value_t<420> color24{};				// 420: 24-bit color value - lowest 8 bits are blue, next 8 are green, highest 8 are red
		code_to_value_t<430> color_name;
		code_to_value_t<440> transparency{};
		code_to_value_t<390> ptr_plot_style_object{};
		code_to_value_t<284> shadow_mode{};			// 0 : Casts and received shadows, 1 : Casts shadows, 2 : Receives shadows, 3 : Ignores shadows


		point_t extrusion{0., 0., 1.};
		double thickness{};


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
	template <>
	struct TGroupHandler<xEntity> {
		//static double& GetX(xEntity& self, point_t xEntity::*point) { return (self.*point).x(); }

		constexpr static inline auto handlers = std::make_tuple(
			67, &xEntity::space,
			8, &xEntity::layer,
			39, &xEntity::thickness,
			//210, std::bind(&GetX, std::placeholders::_1, &xEntity::extrusion),
			210, BSC__LAMBDA_MEMBER_VALUE(extrusion.x()),
			220, BSC__LAMBDA_MEMBER_VALUE(extrusion.y()),
			230, BSC__LAMBDA_MEMBER_VALUE(extrusion.z())

		);
	};
	using entity_ptr_t = TCloneablePtr<xEntity>;
	using entities_t = std::deque<entity_ptr_t>;

	//============================================================================================================================
	class xUnknownEntity : public xEntity {
	public:
		std::vector<sGroup> groups;
		BSC__DXF_ENTITY_DEFINITION(unknown, "", xUnknownEntity, xEntity)
	};
	template <>
	struct TGroupHandler<xUnknownEntity> {
		constexpr static inline auto handlers = std::make_tuple(
		);
	};

	//=============================================================================================================================
	class xPoint : public xEntity {
	public:
		point_t pt;
		BSC__DXF_ENTITY_DEFINITION(point, "POINT", xPoint, xEntity)
	};
	template <>
	struct TGroupHandler<xPoint> {
		constexpr static inline auto handlers = std::make_tuple(
			10, BSC__LAMBDA_MEMBER_VALUE(pt.x()),
			20, BSC__LAMBDA_MEMBER_VALUE(pt.y()),
			30, BSC__LAMBDA_MEMBER_VALUE(pt.z())
		);
	};

	//=============================================================================================================================
	class xLine : public xPoint {
	public:
		point_t pt1;
		BSC__DXF_ENTITY_DEFINITION(line, "LINE", xLine, xPoint)
	};
	template <>
	struct TGroupHandler<xLine> {
		constexpr static inline auto handlers = std::make_tuple(
			11, BSC__LAMBDA_MEMBER_VALUE(pt1.x()),
			21, BSC__LAMBDA_MEMBER_VALUE(pt1.y()),
			31, BSC__LAMBDA_MEMBER_VALUE(pt1.z())
		);
	};

	//=============================================================================================================================
	class xRay : public xLine {
	public:
		BSC__DXF_ENTITY_DEFINITION(ray, "XRAY", xRay, xLine)
	};
	template <>
	struct TGroupHandler<xRay> {
		constexpr static inline auto handlers = std::make_tuple(
		);
	};

	//=============================================================================================================================
	class xXLine : public xLine {
	public:
		BSC__DXF_ENTITY_DEFINITION(xline, "XLINE", xXLine, xLine)
	};
	template <>
	struct TGroupHandler<xXLine> {
		constexpr static inline auto handlers = std::make_tuple(
		);
	};

	//=============================================================================================================================
	std::unique_ptr<xEntity> xEntity::CreateEntity(string_t const& name) {
		// todo: temporarily blocked
		//if (auto iter = m_mapEntityFactory.find(name); iter != m_mapEntityFactory.end() and iter->second)
		//	return iter->second();
		return std::make_unique<xUnknownEntity>();
	}

}

