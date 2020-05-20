project(lips)
cmake_minimum_required(VERSION 2.8)

# The following macro is copied and modified from https://stackoverflow.com/a/31010221
macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif ()
  else ()
    set(CMAKE_CXX_STANDARD 11)
    set(CXX_STANDARD_REQUIRED ON)
  endif ()
endmacro(use_cxx11)

use_cxx11()

file(GLOB_RECURSE SRC_LIST src/*.c* src/*.h*)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
include(FindBoost)
find_package(Boost REQUIRED COMPONENTS date_time program_options)
include_directories(${Boost_INCLUDE_DIRS})

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

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} curlpp)

