cmake_minimum_required(VERSION 3.20)

project(minecraft)

set(CMAKE_CXX_STANDARD 17)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw-3.4)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glad)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb_image)

add_executable(minecraft)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/**")

target_include_directories(minecraft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(minecraft PRIVATE ${SOURCES})
target_link_libraries(minecraft PRIVATE glfw glad stb_image glm)