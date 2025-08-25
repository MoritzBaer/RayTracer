cmake_minimum_required(VERSION 4.1)

project(RAY_TRACER)

set(CMAKE_CXX_STANDARD 23)

add_executable(RayTracer src/app.cpp)

target_include_directories(RayTracer PUBLIC "${CMAKE_SOURCE_DIR}/dependencies/SDL/SDL2-include")

target_link_libraries(RayTracer PUBLIC "${CMAKE_SOURCE_DIR}/dependencies/SDL/SDL2.lib")