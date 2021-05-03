cmake_minimum_required(VERSION 3.15)
project(CommandLib)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
	add_compile_options(/W4)
else()
	add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(ParserLib main.cpp)
target_link_libraries(ParserLib PUBLIC fmt)
