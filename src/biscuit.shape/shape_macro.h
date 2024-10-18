#pragma once

#define BSC__SHAPE_BASE_DEFINITION(CLASS_NAME, BASE_NAME, CLASS_TYPE)\
		using base_t = BASE_NAME;\
		using this_t = CLASS_NAME;\
		virtual std::unique_ptr<xShape> clone() const override { return std::unique_ptr<this_t>(new this_t(*this)); }\
		auto& base() { return static_cast<base_t&>(*this); }\
		auto const& base() const { return static_cast<base_t const&>(*this); }\
		bool operator == (this_t const&) const = default;\
		bool operator != (this_t const&) const = default;\
		virtual bool Compare(xShape const& B_) const override {\
			if (!base_t::Compare(B_))\
				return false;\
			this_t const& B = (this_t const&)B_;\
			return *this == B;\
		}\
		virtual eSHAPE GetShapeType() const { return CLASS_TYPE; }\


#define BSC__SHAPE_ARCHIVE_MEMBER(CLASS_NAME, BASE_NAME, VERSION, ...)\
		constexpr static inline uint32_t s_version{VERSION};\
		template < typename archive >\
		void serialize(archive& ar, std::uint32_t const /*file_version*/) {\
			ar(base() __VA_OPT__(,) __VA_ARGS__);\
		}\
		//GLZ_LOCAL_META_DERIVED(CLASS_NAME, BASE_NAME __VA_OPT__(,) __VA_ARGS__);\


#define BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(CLASS_NAME)\
		export CEREAL_REGISTER_TYPE(CLASS_NAME);\
		export CEREAL_REGISTER_POLYMORPHIC_RELATION(CLASS_NAME::base_t, CLASS_NAME);\
		export CEREAL_CLASS_VERSION(CLASS_NAME, CLASS_NAME::s_version);\


