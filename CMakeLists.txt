cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER "/usr/bin/clang-18")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

project(3D)

include_directories(
	${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/include/libraries
)

file(GLOB all_SRCS
  "${PROJECT_SOURCE_DIR}/src/*/*.cpp"
  "${PROJECT_SOURCE_DIR}/src/libraries/glad.c"
  "${PROJECT_SOURCE_DIR}/src/main.cpp"
)

add_library(glad STATIC ${PROJECT_SOURCE_DIR}/src/libraries/glad.c)
target_include_directories(glad PUBLIC ${PROJECT_SOURCE_DIR}/include/glad)

add_executable(${PROJECT_NAME} ${all_SRCS})
target_link_libraries(${PROJECT_NAME} glad stdc++ glfw m)
