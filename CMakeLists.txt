cmake_minimum_required (VERSION 2.8.0)
project ("ZuoPar")

find_package (Boost REQUIRED)
add_definitions (-DBOOST_LOG_DYN_LINK)

set (CMAKE_CXX_FLAGS "-std=c++11")
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set (SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

include_directories (${Boost_INCLUDE_DIRS} ${SOURCE_DIR})
link_directories (${Boost_LIBRARY_DIRS})

add_subdirectory (src)
