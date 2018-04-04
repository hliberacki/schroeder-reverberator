file(GLOB SRC_FILES src/*.cpp)
add_executable(reverb ${SRC_FILES})

target_link_libraries(reverb simplewav ${CImg_SYSTEM_LIBS}) 
