cmake_minimum_required(VERSION 3.25)
project(variant)

set(CMAKE_CXX_STANDARD 14)

set(SOURCES main.cpp)
set(HEADERS Variant.h)

add_executable(variant ${SOURCES} ${HEADERS})

add_subdirectory(test)
