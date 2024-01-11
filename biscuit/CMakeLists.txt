cmake_minimum_required (VERSION 3.28)
# CMakeList.txt : CMake project for biscuit, include source and define
# project specific logic here.
#

# Add source to this project's library.
add_library (biscuit STATIC "biscuit.ixx" "biscuit.h")

set_property(TARGET biscuit PROPERTY CXX_STANDARD 23)

find_package(fmt REQUIRED)

# TODO: Add tests and install targets if needed.

include(CheckCXXCompilerFlag)

check_cxx_compiler_flag("-fmodules" MODULES_TS)

#if(MODULES_TS)
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts -fprebuilt-module-path=${CMAKE_BINARY_DIR} -fmodule-file=${CMAKE_BINARY_DIR}/foo.pcm")
#else()
#	message(FATAL_ERROR "C++20 Modules required!")
#endif()

target_sources(biscuit PUBLIC FILE_SET biscuit_modules TYPE CXX_MODULES FILES
               ${CMAKE_CURRENT_SOURCE_DIR}/biscuit.ixx
)

find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)
find_package(glaze REQUIRED)

target_link_libraries(biscuit
	PRIVATE
		fmt::fmt
)
