cmake_minimum_required(VERSION 3.17)
project(3D)

set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include_directories(
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/resources/*
)

link_directories(
	${PROJECT_SOURCE_DIR}/lib
)

file(GLOB all_SRCS
	"${PROJECT_SOURCE_DIR}/include/*.h"
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.c"
	"${PROJECT_SOURCE_DIR}/src/resources/shaders/*"
)
add_executable(${PROJECT_NAME} ${all_SRCS})
target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/libglfw.3.dylib)
