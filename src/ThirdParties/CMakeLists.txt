cmake_minimum_required (VERSION 3.30)
# CMakeList.txt : CMake project for ThirdParties, include source and define
# project specific logic here.
#

set(VCPKG_INSTALLED_DIR ${CMAKE_SOURCE_DIR}/vcpkg_installed)

project(ThirdParties)

# Add source to this project's library.
add_library (ThirdParties STATIC
"framework.h"
"cereal.ixx"
"Eigen.ixx"
"fmt.ixx"
"ThirdParties.ixx"
"units.ixx"
"fmt.hxx"
)

#Set different output names based on the build type
set_target_properties(ThirdParties PROPERTIES
    DEBUG_POSTFIX ".D"
    #RELEASE_POSTFIX ".x64R"
)

set_property(TARGET ThirdParties PROPERTY CXX_STANDARD 23)

# TODO: Add tests and install targets if needed.

include(CheckCXXCompilerFlag)

check_cxx_compiler_flag("-fmodules" MODULES_TS)

#if(MODULES_TS)
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts -fprebuilt-module-path=${CMAKE_BINARY_DIR} -fmodule-file=${CMAKE_BINARY_DIR}/foo.pcm")
#else()
#	message(FATAL_ERROR "C++20 Modules required!")
#endif()

if (MSVC)
	target_compile_options(${PROJECT_NAME} PUBLIC "/Zc:preprocessor")
endif()

target_sources(ThirdParties PUBLIC FILE_SET ThirdParties_modules TYPE CXX_MODULES FILES
                ${CMAKE_CURRENT_SOURCE_DIR}/cereal.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/Eigen.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/fmt.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParties.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/units.ixx
)

target_include_directories(ThirdParties PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(asio CONFIG REQUIRED)
find_package(boost_pfr CONFIG REQUIRED)
find_package(catch2 CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)
find_package(ctre CONFIG REQUIRED)
find_package(eigen3 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(freeimage CONFIG REQUIRED)
find_package(glaze CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)
# find_package(libiconv CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(opencv CONFIG REQUIRED)
find_package(simdutf CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(tinyspline CONFIG REQUIRED)
find_package(units CONFIG REQUIRED)
# find_package(winreg CONFIG REQUIRED)

target_link_libraries(ThirdParties PUBLIC fmt::fmt)
target_link_libraries(ThirdParties PUBLIC asio::asio)
target_link_libraries(ThirdParties PUBLIC boost::pfr)
target_link_libraries(ThirdParties PUBLIC catch2::catch2)
target_link_libraries(ThirdParties PUBLIC cereal::cereal)
target_link_libraries(ThirdParties PUBLIC ctre::ctre)
target_link_libraries(ThirdParties PUBLIC Eigen3::Eigen)
target_link_libraries(ThirdParties PUBLIC freeimage::freeimage)
target_link_libraries(ThirdParties PUBLIC glaze::glaze)
target_link_libraries(ThirdParties PUBLIC GLEW::GLEW)
# target_link_libraries(ThirdParties PUBLIC libiconv::libiconv)
target_link_libraries(ThirdParties PUBLIC magic_enum::magic_enum)
target_link_libraries(ThirdParties PUBLIC opencv::opencv)
target_link_libraries(ThirdParties PUBLIC simdutf::simdutf)
target_link_libraries(ThirdParties PUBLIC spdlog::spdlog)
target_link_libraries(ThirdParties PUBLIC tinyspline::tinyspline)
target_link_libraries(ThirdParties PUBLIC units::units)
# target_link_libraries(ThirdParties PUBLIC winreg::winreg)

#add_custom_command(TARGET ThirdParties POST_BUILD
##    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ThirdParties> "/ThirdParties"
#    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_SOURCE_DIR}/include/ThirdParties/"
#)
