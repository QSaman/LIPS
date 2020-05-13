project(lips-test)
cmake_minimum_required(VERSION 2.8)

file(GLOB_RECURSE SRC_LIST src/*.c* src/*.h*)
add_subdirectory("${CMAKE_SOURCE_DIR}/thirdparty/googletest" "thirdparty/googletest")
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)

################################################################################

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} gtest gtest_main lips-lib)

################################################################################
# Enabling C++11. Add these two lines after add_executable
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

################################################################################


