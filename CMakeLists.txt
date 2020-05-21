project(lips)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(EnableCpp11)

file(GLOB_RECURSE SRC_LIST src/*.c* src/*.h*)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
include(FindBoost)
find_package(Boost REQUIRED COMPONENTS date_time program_options)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(src)

include(FindOpenSSL)
find_package(OpenSSL REQUIRED)

add_subdirectory(thirdparty/curlpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/curlpp/include)

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
	enable_testing()
	add_library(${PROJECT_NAME}-lib ${SRC_LIST})
	target_link_libraries(${PROJECT_NAME}-lib ${Boost_LIBRARIES} curlpp)
	include(GoogleTest)
	add_subdirectory(test)
endif()

################################################################################

add_executable(${PROJECT_NAME} ${SRC_LIST} cli/main.cpp)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} curlpp)

