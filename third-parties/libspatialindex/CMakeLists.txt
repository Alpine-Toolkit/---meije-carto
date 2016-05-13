# -*- CMake -*-

####################################################################################################

set(LIBSPATIALINDEX_SOURCE_PATH "libspatialindex.git")

####################################################################################################

SET(SIDX_VERSION_MAJOR "1")
SET(SIDX_VERSION_MINOR "8")
SET(SIDX_VERSION_PATCH "5")
SET(SIDX_LIB_VERSION "4.1.0")
SET(SIDX_LIB_SOVERSION "4")

set(SIDX_VERSION_STRING "${SIDX_VERSION_MAJOR}.${SIDX_VERSION_MINOR}.${SIDX_VERSION_PATCH}")

# Name of C++ library
set(SIDX_LIB_NAME spatialindex)
set(SIDX_C_LIB_NAME spatialindex_c)

####################################################################################################

set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

include(CheckFunctionExists)
check_function_exists(srand48 HAVE_SRAND48)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(memset HAVE_MEMSET)
check_function_exists(memcpy HAVE_MEMCPY)
check_function_exists(bcopy HAVE_BCOPY)

INCLUDE(CheckIncludeFiles)
check_include_files(pthread.h HAVE_PTHREAD_H)

if (HAVE_SRAND48)
  add_definitions(-DHAVE_SRAND48=1)
endif()

if (HAVE_GETTIMEOFDAY)
  add_definitions(-DHAVE_GETTIMEOFDAY=1)
endif()

if (HAVE_BZERO)
  add_definitions(-DHAVE_BZERO=1)
endif()

if (HAVE_MEMSET)
  add_definitions(-DHAVE_MEMSET=1)
endif()

if (HAVE_MEMCPY)
  add_definitions(-DHAVE_MEMCPY=1)
endif()

if (HAVE_BCOPY)
  add_definitions(-DHAVE_BCOPY=1)
endif()

if (HAVE_PTHREAD_H)
  add_definitions(-DHAVE_PTHREAD_H=1)
endif()

####################################################################################################

# Recommended C++ compilation flags
# -Weffc++
set(SIDX_COMMON_CXX_FLAGS
  "-pedantic -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wredundant-decls -Wno-long-long")

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SIDX_COMMON_CXX_FLAGS} -std=c++98 -ansi")
  set(SIDX_COMPILER_GCC 1)
endif()

####################################################################################################
# Source files specification
#
# Naming format:
#   SIDX_CPP - all the .cpp files
#   SIDX_HPP - all the .hpp files
#   SIDX_<dirname>_CPP - all the .cpp files for the given subdir/namespace
#   ...

set(SIDX_HEADERS_DIR "${LIBSPATIALINDEX_SOURCE_PATH}/include/spatialindex/")
set(SIDX_HEADERS_CAPI_DIR "${LIBSPATIALINDEX_SOURCE_PATH}/include/spatialindex/capi")
set(SIDX_HPP "")
set(SIDX_CPP "")

set(SIDX_SRC_DIR "${LIBSPATIALINDEX_SOURCE_PATH}/src")
set(SIDX_CAPI_DIR "${LIBSPATIALINDEX_SOURCE_PATH}/src/capi")

#
# base
#
set(SIDX_BASE_HPP
  "${SIDX_HEADERS_DIR}/LineSegment.h"
  "${SIDX_HEADERS_DIR}/MovingPoint.h"
  "${SIDX_HEADERS_DIR}/MVRTree.h"
  "${SIDX_HEADERS_DIR}/Point.h"
  "${SIDX_HEADERS_DIR}/Region.h"
  "${SIDX_HEADERS_DIR}/RTree.h"
  "${SIDX_HEADERS_DIR}/SpatialIndex.h"
  "${SIDX_HEADERS_DIR}/TimePoint.h"
  "${SIDX_HEADERS_DIR}/TimeRegion.h"
  "${SIDX_HEADERS_DIR}/TPRTree.h"
  "${SIDX_HEADERS_DIR}/Version.h"
  )
list(APPEND SIDX_HPP ${SIDX_BASE_HPP})

set(SIDX_CAPI_HPP
  "${SIDX_HEADERS_CAPI_DIR}/BoundsQuery.h"
  "${SIDX_HEADERS_CAPI_DIR}/CountVisitor.h"
  "${SIDX_HEADERS_CAPI_DIR}/CustomStorage.h"
  "${SIDX_HEADERS_CAPI_DIR}/DataStream.h"
  "${SIDX_HEADERS_CAPI_DIR}/Error.h"
  "${SIDX_HEADERS_CAPI_DIR}/IdVisitor.h"
  "${SIDX_HEADERS_CAPI_DIR}/Index.h"
  "${SIDX_HEADERS_CAPI_DIR}/LeafQuery.h"
  "${SIDX_HEADERS_CAPI_DIR}/ObjVisitor.h"
  "${SIDX_HEADERS_CAPI_DIR}/sidx_api.h"
  "${SIDX_HEADERS_CAPI_DIR}/sidx_config.h"
  "${SIDX_HEADERS_CAPI_DIR}/sidx_impl.h"
  "${SIDX_HEADERS_CAPI_DIR}/Utility.h"
  )
list(APPEND SIDX_HPP ${SIDX_CAPI_HPP})

set(SIDX_CAPI_CPP
  "${SIDX_CAPI_DIR}/BoundsQuery.cc"
  "${SIDX_CAPI_DIR}/CountVisitor.cc"
  "${SIDX_CAPI_DIR}/CustomStorage.cc"
  "${SIDX_CAPI_DIR}/DataStream.cc"
  "${SIDX_CAPI_DIR}/Error.cc"
  "${SIDX_CAPI_DIR}/IdVisitor.cc"
  "${SIDX_CAPI_DIR}/Index.cc"
  "${SIDX_CAPI_DIR}/LeafQuery.cc"
  "${SIDX_CAPI_DIR}/ObjVisitor.cc"
  "${SIDX_CAPI_DIR}/sidx_api.cc"
  "${SIDX_CAPI_DIR}/Utility.cc"
  )

set(SIDX_SPATIALINDEX_CPP
  "${SIDX_SRC_DIR}/spatialindex/LineSegment.cc"
  "${SIDX_SRC_DIR}/spatialindex/MovingPoint.cc"
  "${SIDX_SRC_DIR}/spatialindex/MovingRegion.cc"
  "${SIDX_SRC_DIR}/spatialindex/Point.cc"
  "${SIDX_SRC_DIR}/spatialindex/Region.cc"
  "${SIDX_SRC_DIR}/spatialindex/SpatialIndexImpl.cc"
  "${SIDX_SRC_DIR}/spatialindex/TimePoint.cc"
  "${SIDX_SRC_DIR}/spatialindex/TimeRegion.cc"
  )
list(APPEND SIDX_CPP ${SIDX_SPATIALINDEX_CPP})

set(SIDX_MVRTREE_CPP
  "${SIDX_SRC_DIR}/mvrtree/Index.cc"
  "${SIDX_SRC_DIR}/mvrtree/Index.h"
  "${SIDX_SRC_DIR}/mvrtree/Leaf.cc"
  "${SIDX_SRC_DIR}/mvrtree/Leaf.h"
  "${SIDX_SRC_DIR}/mvrtree/MVRTree.cc"
  "${SIDX_SRC_DIR}/mvrtree/MVRTree.h"
  "${SIDX_SRC_DIR}/mvrtree/Node.cc"
  "${SIDX_SRC_DIR}/mvrtree/Node.h"
  "${SIDX_SRC_DIR}/mvrtree/PointerPoolNode.h"
  "${SIDX_SRC_DIR}/mvrtree/Statistics.cc"
  "${SIDX_SRC_DIR}/mvrtree/Statistics.h"
  )
list(APPEND SIDX_CPP ${SIDX_MVRTREE_CPP})

set(SIDX_RTREE_CPP
  "${SIDX_SRC_DIR}/rtree/BulkLoader.cc"
  "${SIDX_SRC_DIR}/rtree/BulkLoader.h"
  "${SIDX_SRC_DIR}/rtree/Index.cc"
  "${SIDX_SRC_DIR}/rtree/Index.h"
  "${SIDX_SRC_DIR}/rtree/Leaf.cc"
  "${SIDX_SRC_DIR}/rtree/Leaf.h"
  "${SIDX_SRC_DIR}/rtree/Node.cc"
  "${SIDX_SRC_DIR}/rtree/Node.h"
  "${SIDX_SRC_DIR}/rtree/PointerPoolNode.h"
  "${SIDX_SRC_DIR}/rtree/RTree.cc"
  "${SIDX_SRC_DIR}/rtree/RTree.h"
  "${SIDX_SRC_DIR}/rtree/Statistics.cc"
  "${SIDX_SRC_DIR}/rtree/Statistics.h"
  )
list(APPEND SIDX_CPP ${SIDX_RTREE_CPP})

set(SIDX_STORAGEMANAGER_CPP
  "${SIDX_SRC_DIR}/storagemanager/Buffer.cc"
  "${SIDX_SRC_DIR}/storagemanager/DiskStorageManager.cc"
  "${SIDX_SRC_DIR}/storagemanager/DiskStorageManager.h"
  "${SIDX_SRC_DIR}/storagemanager/MemoryStorageManager.cc"
  "${SIDX_SRC_DIR}/storagemanager/MemoryStorageManager.h"
  "${SIDX_SRC_DIR}/storagemanager/RandomEvictionsBuffer.cc"
  "${SIDX_SRC_DIR}/storagemanager/RandomEvictionsBuffer.h"
  )
list(APPEND SIDX_CPP ${SIDX_STORAGEMANAGER_CPP})

set(SIDX_RAND48)
if (NOT HAVE_SRAND48)
  set(SIDX_RAND48 "${SIDX_SRC_DIR}/tools/rand48.cc")
endif()

set(SIDX_TOOLS_CPP
  ${SIDX_RAND48}
  "${SIDX_SRC_DIR}/tools/Tools.cc"
  )
list(APPEND SIDX_CPP ${SIDX_TOOLS_CPP})

set(SIDX_TOOLS_CPP
  "${SIDX_SRC_DIR}/tprtree/Index.cc"
  "${SIDX_SRC_DIR}/tprtree/Index.h"
  "${SIDX_SRC_DIR}/tprtree/Leaf.cc"
  "${SIDX_SRC_DIR}/tprtree/Leaf.h"
  "${SIDX_SRC_DIR}/tprtree/Node.cc"
  "${SIDX_SRC_DIR}/tprtree/Node.h"
  "${SIDX_SRC_DIR}/tprtree/PointerPoolNode.h"
  "${SIDX_SRC_DIR}/tprtree/Statistics.cc"
  "${SIDX_SRC_DIR}/tprtree/Statistics.h"
  "${SIDX_SRC_DIR}/tprtree/TPRTree.cc"
  "${SIDX_SRC_DIR}/tprtree/TPRTree.h"
  )
list(APPEND SIDX_CPP ${SIDX_TOOLS_CPP})

####################################################################################################

# Standard include directory of SIDX library
include_directories("${LIBSPATIALINDEX_SOURCE_PATH}/include")

set(APPS_CPP_DEPENDENCIES "${SIDX_LIB_NAME}" CACHE INTERNAL "libraries to link")

####################################################################################################
# Targets settings

set(SIDX_SOURCES
  ${SIDX_HPP}
  ${SIDX_CPP}
  )

add_library(${SIDX_LIB_NAME} SHARED ${SIDX_SOURCES})
set_target_properties(${SIDX_LIB_NAME}
  PROPERTIES VERSION "${SIDX_LIB_VERSION}"
  SOVERSION "${SIDX_LIB_SOVERSION}")

add_library(${SIDX_C_LIB_NAME} SHARED ${SIDX_CAPI_CPP})
target_link_libraries(${SIDX_C_LIB_NAME}
  ${SIDX_LIB_NAME}
  )
set_target_properties(${SIDX_C_LIB_NAME}
  PROPERTIES VERSION "${SIDX_LIB_VERSION}"
  SOVERSION "${SIDX_LIB_SOVERSION}" )

####################################################################################################

add_executable(test_libspatialindex test.cpp)
target_link_libraries(test_libspatialindex ${SIDX_LIB_NAME} ${SIDX_C_LIB_NAME})
target_compile_options(test_libspatialindex PRIVATE "--std=c++0x")

####################################################################################################
#
# End
#
####################################################################################################
