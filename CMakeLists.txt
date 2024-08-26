cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

project(3D)

include_directories(
	${PROJECT_SOURCE_DIR}/include
)

file(GLOB all_SRCS
  "${PROJECT_SOURCE_DIR}/src/*/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*/*.c"
  "${PROJECT_SOURCE_DIR}/src/main.cpp"
)

add_executable(${PROJECT_NAME} ${all_SRCS})
target_link_libraries(${PROJECT_NAME} glfw)
