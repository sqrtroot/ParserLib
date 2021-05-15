add_executable(float_parser ${CMAKE_CURRENT_LIST_DIR}/float_parser.cpp)
target_include_directories(float_parser PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include)
target_link_libraries(float_parser PUBLIC fmt)