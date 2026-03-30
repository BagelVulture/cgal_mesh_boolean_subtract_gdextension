
cmake_minimum_required(VERSION 3.16)
project(cgal_gdextension LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED GODOT_CPP_DIR)
  message(FATAL_ERROR "Set GODOT_CPP_DIR to your godot-cpp directory")
endif()

find_package(CGAL REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED)

add_subdirectory(${GODOT_CPP_DIR} godot-cpp)

file(GLOB SRC_FILES
    src/*.cpp
)

add_library(cgal_gdextension SHARED ${SRC_FILES})

target_include_directories(cgal_gdextension PRIVATE
    ${GODOT_CPP_DIR}/include
)

target_link_libraries(cgal_gdextension
    PRIVATE
    godot-cpp
    CGAL::CGAL
)
