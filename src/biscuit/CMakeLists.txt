cmake_minimum_required (VERSION 3.30)
# CMakeList.txt : CMake project for biscuit, include source and define
# project specific logic here.
#

set(VCPKG_INSTALLED_DIR ${CMAKE_SOURCE_DIR}/vcpkg_installed)

project(biscuit)

# Add source to this project's library.
add_library (biscuit STATIC
"framework.h"
"dll.h"
"aliases.ixx"
"archive.ixx"
"arithmetic.ixx"
"axis.ixx"
"biscuit.ixx"
"bmp_file_header.ixx"
"cereal_helper.ixx"
"charset_kssm.ixx"
"codepage.ixx"
"color.ixx"
"concepts.ixx"
"container_map.ixx"
"convert_codepage.ixx"
"coord.base.ixx"
"coord.bounds.ixx"
"coord.concepts.ixx"
"coord.ixx"
"coord.point.ixx"
"coord.rect.ixx"
"coord.size.ixx"
"coord.transform.ixx"
"iconv_wrapper.ixx"
"lazy_profile.ixx"
"memory.ixx"
"misc.ixx"
"mutex.ixx"
"rand.ixx"
"sequence.base.ixx"
"sequence.coroutine_handle.ixx"
"sequence.ixx"
"sequence.map.ixx"
"sequence.tReturn.ixx"
"smart_ptr_container.ixx"
"stop_watch.ixx"
"string.arithmetic.ixx"
"string.basic.ixx"
"string.compares.ixx"
"string.defines.ixx"
"string.ixx"
"string.manipulates.ixx"
"units.ixx"
"utils.ixx"
"views.ixx"
"dllmain.cpp"

)

#Set different output names based on the build type
set_target_properties(biscuit PROPERTIES
    DEBUG_POSTFIX ".D"
    #RELEASE_POSTFIX ".x64R"
)

set_property(TARGET biscuit PROPERTY CXX_STANDARD 23)

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

target_sources(biscuit PUBLIC FILE_SET biscuit_modules TYPE CXX_MODULES FILES
                ${CMAKE_CURRENT_SOURCE_DIR}/aliases.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/archive.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/arithmetic.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/axis.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/biscuit.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/bmp_file_header.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/cereal_helper.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/charset_kssm.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
                ${CMAKE_CURRENT_SOURCE_DIR}/codepage.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/color.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/concepts.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/container_map.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/convert_codepage.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/coord.base.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/coord.bounds.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/coord.concepts.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/coord.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/coord.point.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/coord.rect.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/coord.size.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/coord.transform.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/iconv_wrapper.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/lazy_profile.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/memory.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/misc.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/mutex.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/rand.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/sequence.base.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/sequence.coroutine_handle.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/sequence.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/sequence.map.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/sequence.tReturn.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/smart_ptr_container.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/stop_watch.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/string.arithmetic.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/string.basic.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/string.compares.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/string.defines.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/string.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/string.manipulates.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/units.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/utils.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/views.ixx
                
)

target_include_directories(biscuit PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(asio REQUIRED)
#find_package(boost-pfr REQUIRED)
find_package(catch2 REQUIRED)
find_package(cereal REQUIRED)
find_package(ctre REQUIRED)
#find_package(eigen3 REQUIRED)
find_package(fmt REQUIRED)
#find_package(freeimage REQUIRED)
find_package(glaze REQUIRED)
#find_package(glew REQUIRED)
find_package(libiconv REQUIRED)
find_package(magic-enum REQUIRED)
#find_package(opencv REQUIRED)
find_package(simdutf REQUIRED)
find_package(spdlog REQUIRED)
#find_package(tinyspline REQUIRED)
find_package(units REQUIRED)
#find_package(winreg REQUIRED)


target_link_libraries(biscuit PRIVATE fmt::fmt)

#add_custom_command(TARGET biscuit POST_BUILD
##    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:biscuit> "/biscuit"
#    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_SOURCE_DIR}/include/biscuit/"
#)
