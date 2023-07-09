project(variant_tests)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(lib)

set(gtest_SOURCE_DIR "${PROJECT_SOURCE_DIR}/lib/googletest")

set(SOURCES variant_tests.cpp)
set(HEADERS VariantTest.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

message("CMake source directory = ${CMAKE_SOURCE_DIR}")

target_include_directories(${PROJECT_NAME} PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} gtest gtest_main)
target_link_libraries(${PROJECT_NAME} gmock gmock_main)

