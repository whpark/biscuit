#pragma once

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
		eENTITY GetEntityType() const override { return ENTITY; }\
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
		static inline xRegisterEntity s_registerEntity_##CLASS_NAME{ENTITY_STRING, &this_t::create};\
	public:\
		bool Read(group_iter_t& iter, group_iter_t const& end) override { return ReadEntity<this_t>(*this, iter, end); }\

#define BSC__LAMBDA_MEMBER_VALUE(member)  [](auto& self) -> decltype(auto) { return self.member; }

