cmake_minimum_required (VERSION 2.6)

file(GLOB SRC_FILES src/*.cpp)
add_library(simplewav ${SRC_FILES})

target_include_directories(simplewav PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


