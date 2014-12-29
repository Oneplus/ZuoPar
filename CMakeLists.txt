cmake_minimum_required (VERSION 2.8.0)
project ("ZuoPar")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package (Boost REQUIRED)
find_package (sparsehash)

if (SPARSEHASH_FOUND)
    add_definitions (-DUNORDERED_MAP_IMPL=dense_hash_map)
else()
    add_definitions (-DUNORDERED_MAP_IMPL=boost_unordered_map)
endif (SPARSEHASH_FOUND)

add_definitions (-DBOOST_LOG_DYN_LINK)

set (CMAKE_CXX_FLAGS "-std=c++11 -g")
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set (SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

if (SPARSEHASH_FOUND)
    include_directories (${Boost_INCLUDE_DIRS} ${SPARSEHASH_INCLUDE_DIR} ${SOURCE_DIR})
else ()
    include_directories (${Boost_LIBRARY_DIRS} ${SOURCE_DIR})
endif (SPARSEHASH_FOUND)

link_directories (${Boost_LIBRARY_DIRS})

add_subdirectory (src)
