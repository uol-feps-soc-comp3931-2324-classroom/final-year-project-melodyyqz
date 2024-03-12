file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS *.hpp)

source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${SOURCES} ${HEADERS})
add_executable(prog ${SOURCES} ${HEADERS})
target_link_libraries(prog embree rapidobj stb)
target_include_directories(prog PRIVATE ${CMAKE_CURRENT_LIST_DIR})
install(TARGETS prog RUNTIME DESTINATION bin)