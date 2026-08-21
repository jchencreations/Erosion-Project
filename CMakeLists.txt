cmake_minimum_required(VERSION 3.10)

project(Erosion-Project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(Erosion-Project
    erosion.cpp
    perlin.cpp
)

target_include_directories(Erosion-Project PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(glm CONFIG REQUIRED)

target_link_libraries(Erosion-Project PRIVATE glm::glm)