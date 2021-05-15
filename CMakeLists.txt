cmake_minimum_required(VERSION 3.15)
project(ParserLib)

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
	add_compile_options(/W4)
else()
	add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

option(COVERAGE "build with coverage enabled" NO)
if(COVERAGE)
  if(MSVC)
    message(SEND_ERROR "MSVC doesnt support coverage flags")
  else()
    set(CMAKE_BUILD_TYPE Debug)
    add_compile_options(-g -O0 -fprofile-arcs -ftest-coverage --coverage -fno-inline -fno-inline-small-functions -fno-default-inline)
    add_link_options(-g -O0 -fprofile-arcs -ftest-coverage --coverage -fno-inline -fno-inline-small-functions -fno-default-inline)
    set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)
  endif()
endif()

find_package(Doxygen REQUIRED)
doxygen_add_docs(
    doxygen
    ${CMAKE_CURRENT_LIST_DIR}/include
)
include(tests/CMakeLists.txt)
include(example/CMakeLists.txt)