cmake_minimum_required(VERSION 3.10)

project(Erosion-Project LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.6 # Replace with the desired version
)

# Disable unnecessary GLFW features to streamline the build
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

find_package(glm CONFIG REQUIRED)
find_package(GLEW REQUIRED)

add_executable(Erosion-Project
    erosion.cpp
    src/Camera.h
    src/Shader.h
    src/Terrain.h
    src/shader.vert
    src/shader.frag
)

target_include_directories(Erosion-Project PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(Erosion-Project PRIVATE glfw GLEW glm)
