project(lips)
cmake_minimum_required(VERSION 2.8)

file(GLOB_RECURSE SRC_LIST src/*.c* src/*.h*)

################################################################################

add_executable(${PROJECT_NAME} ${SRC_LIST})

################################################################################
# Enabling C++11. Add these two lines after add_executable
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

################################################################################
