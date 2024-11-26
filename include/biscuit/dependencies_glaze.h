#pragma once

#include "./config.h"

#include <glaze/glaze.hpp>

#define GLZ_X(x) #x, &GLZ_T::x

#define GLZ_META(C, ...)                                             \
   template <>                                                       \
   struct glz::meta<C>                                               \
   {                                                                 \
      using GLZ_T = C;                                               \
      [[maybe_unused]] static constexpr std::string_view name = #C;  \
      static constexpr auto value = object(                          \
        GLZ_FOR_EACH(GLZ_X, __VA_ARGS__));                           \
   }

#define GLZ_LOCAL_META(C, ...)                                       \
   struct glaze                                                      \
   {                                                                 \
      using GLZ_T = C;                                               \
      [[maybe_unused]] static constexpr std::string_view name = #C;  \
      static constexpr auto value = glz::object(                     \
        GLZ_FOR_EACH(GLZ_X, __VA_ARGS__));                           \
   }

#define GLZ_META_DERIVED(C, parent, ...)                             \
   template <>                                                       \
   struct glz::meta<C>                                               \
   {                                                                 \
      using GLZ_T = C;                                               \
      [[maybe_unused]] static constexpr std::string_view name = #C;  \
      static constexpr auto value = object(                          \
        #parent, [](auto&& self) -> auto& { return (parent&)self; } __VA_OPT__(,) \
        GLZ_FOR_EACH(GLZ_X, __VA_ARGS__));                           \
   }

#define GLZ_LOCAL_META_DERIVED(C, parent, ...)                       \
   struct glaze                                                      \
   {                                                                 \
      using GLZ_T = C;                                               \
      [[maybe_unused]] static constexpr std::string_view name = #C;  \
      static constexpr auto value = glz::object(                     \
        #parent, [](auto&& self) -> auto& { return (parent&)self; } __VA_OPT__(,) \
        GLZ_FOR_EACH(GLZ_X, __VA_ARGS__));                           \
   }


