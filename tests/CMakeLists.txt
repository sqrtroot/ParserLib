file(GLOB SRCS ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
add_executable(tests ${SRCS})
find_package(Catch2 REQUIRED)
target_include_directories(tests PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include)
target_link_libraries(tests PRIVATE Catch2::Catch2)

if(COVERAGE AND NOT MSVC)
  add_custom_command(OUTPUT coverage.info
    COMMAND lcov --directory ${CMAKE_BINARY_DIR} --zerocounters
    COMMAND ${CMAKE_BINARY_DIR}/tests
    COMMAND lcov -c --directory ${CMAKE_BINARY_DIR} -o coverage.info --include "*parser*"
    DEPENDS tests)

  add_custom_target(coverage_summary
    DEPENDS coverage.info
    COMMAND lcov --summary ${CMAKE_BINARY_DIR}/coverage.info)

  add_custom_target(coverage_html
    DEPENDS coverage.info
    COMMAND genhtml --highlight --output-directory ${CMAKE_BINARY_DIR}/coverage_html ${CMAKE_BINARY_DIR}/coverage.info)
  set_target_properties(coverage_html PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_BINARY_DIR}/coverage_html/)

endif()

include(CTest)
include(Catch)
catch_discover_tests(tests)
