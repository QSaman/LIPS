# The following lines are copied and modified from https://stackoverflow.com/a/31010221
if (CMAKE_VERSION VERSION_LESS "3.1")
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  endif ()
else ()
  set(CMAKE_CXX_STANDARD 11)
  set(CXX_STANDARD_REQUIRED ON)
endif ()
