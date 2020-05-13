project(lips)
cmake_minimum_required(VERSION 3.1)

file(GLOB_RECURSE SRC_LIST src/*.c* src/*.h*)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS date_time)
include_directories(${Boost_INCLUDE_DIRS})

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
	enable_testing()
	add_library(${PROJECT_NAME}-lib ${SRC_LIST})
	target_link_libraries(${PROJECT_NAME}-lib ${Boost_LIBRARIES})
	include(GoogleTest)
	add_subdirectory(test)
endif()

################################################################################

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

################################################################################
# Enabling C++11. Add these two lines after add_executable
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

################################################################################


