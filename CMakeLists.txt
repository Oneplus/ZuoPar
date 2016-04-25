cmake_minimum_required (VERSION 2.8.0)
project ("ZuoPar")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set (SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set (THIRDPARTY_DIR ${PROJECT_SOURCE_DIR}/thirdparty)

# look for Boost
if(DEFINED ENV{BOOST_ROOT})
      set(Boost_NO_SYSTEM_PATHS ON)
endif()

set(Boost_REALPATH ON)
if(WIN32)
    find_package(Boost COMPONENTS regex program_options serialization log_setup log thread system filesystem date_time timer chrono locale REQUIRED)
else()
    add_definitions (-DBOOST_LOG_DYN_LINK)
    find_package(Boost COMPONENTS regex program_options serialization log_setup log thread system timer REQUIRED)
endif()

# look for SparseHash
find_package (sparsehash)
if (SPARSEHASH_FOUND)
    add_definitions (-DUNORDERED_MAP_IMPL=use_dense_hash_map)
else()
    add_definitions (-DUNORDERED_MAP_IMPL=use_unordered_map)
endif (SPARSEHASH_FOUND)

if (MSVC)
    set (CMAKE_CXX_FLAGS "/EHsc")
else()
    set (CMAKE_CXX_FLAGS "-std=c++11 -g")
endif()

if (SPARSEHASH_FOUND)
    include_directories (${Boost_INCLUDE_DIRS} ${SPARSEHASH_INCLUDE_DIR} ${SOURCE_DIR})
else ()
    include_directories (${Boost_INCLUDE_DIRS} ${SOURCE_DIR})
endif (SPARSEHASH_FOUND)

if(WIN32)
    set(LIBS ${LIBS} ${Boost_LIBRARIES})
else()
    set(LIBS ${LIBS} ${Boost_LIBRARIES} pthread)
endif()

add_subdirectory (src)
