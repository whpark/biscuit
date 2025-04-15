cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(test.biscuit)

add_executable(test.biscuit
	main.cpp)

target_sources(test.biscuit PUBLIC
			${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/test_archive.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/test_coord.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/test_lazy_profile.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/test_memory.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/test_misc.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/test_mutex.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/test_sequence_map.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/test_sequence_tReturn.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/test_sequence.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/test_string.arithmetic.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/test_string.basic.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/test_string.convert_codepage.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/test_string.manipulates.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/test_win.cpp
)

if (MSVC)
	target_compile_options(${PROJECT_NAME} PUBLIC "/Zc:preprocessor")
endif()

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


target_link_libraries(test.biscuit
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

add_dependencies(test.biscuit biscuit)
