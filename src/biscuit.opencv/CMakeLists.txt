cmake_minimum_required (VERSION 3.30)
# CMakeList.txt : CMake project for biscuit, include source and define
# project specific logic here.
#

set(VCPKG_INSTALLED_DIR ${CMAKE_SOURCE_DIR}/vcpkg_installed)

project(biscuit.opencv)

# Add source to this project's library.
add_library (biscuit.opencv STATIC
"biscuit.opencv.ixx"
"bmp_file_archive.ixx"
"mat_gl.ixx"
"mat_operation.ixx"
"dllmain.cpp"
)

#Set different output names based on the build type
set_target_properties(biscuit.opencv PROPERTIES
    DEBUG_POSTFIX ".D"
    #RELEASE_POSTFIX ".x64R"
)

set_property(TARGET biscuit.opencv PROPERTY CXX_STANDARD 23)

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

target_sources(biscuit.opencv PUBLIC FILE_SET biscuit_modules TYPE CXX_MODULES FILES
                ${CMAKE_CURRENT_SOURCE_DIR}/biscuit.opencv.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/bmp_file_archive.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/mat_gl.ixx
                ${CMAKE_CURRENT_SOURCE_DIR}/mat_operation.ixx
                
)

target_include_directories(biscuit PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(asio CONFIG REQUIRED)
find_package(boost_pfr CONFIG REQUIRED)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
find_package(catch2 CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)
find_package(ctre CONFIG REQUIRED)
find_package(eigen3 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(freeimage CONFIG REQUIRED)
find_package(glaze CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)
find_package(Iconv REQUIRED)
find_package(magic_enum CONFIG REQUIRED)

find_package(opencv CONFIG REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(simdutf CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(tinyspline CONFIG REQUIRED)
find_package(units CONFIG REQUIRED)
# find_package(WinReg REQUIRED)


target_link_libraries(ThirdParties
	{OpenCV_LIBS}
	fmt::fmt
	asio::asio
	Catch2::Catch2
	cereal::cereal
	ctre::ctre
	Eigen3::Eigen
	FreeImage
	glaze::glaze
	GLEW::GLEW
	Iconv::Iconv
	magic_enum::magic_enum
	simdutf::simdutf
	spdlog::spdlog
	tinyspline::tinyspline
	units::units
#	winreg::winreg
)


#add_custom_command(TARGET biscuit POST_BUILD
##    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:biscuit> "/biscuit"
#    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_SOURCE_DIR}/include/biscuit/"
#)
