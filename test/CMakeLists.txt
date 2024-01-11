cmake_minimum_required(VERSION 3.28)

project ("test")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(test
	main.cpp
	test.cpp)

find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)
find_package(glaze REQUIRED)
	
target_link_libraries(test 
	PRIVATE
		fmt::fmt
		Catch2::Catch2
		biscuit
)

add_dependencies(test biscuit)
