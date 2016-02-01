# -*- CMake -*-

####################################################################################################
#
# Customizable settings
#

# -D <var>:<type>=<value>: Create a cmake cache entry.

# Ipl path
# -D IPL_PATH:PATH=

# Install path prefix, prepended onto install directories.
# -D CMAKE_INSTALL_PREFIX:PATH=/usr/local/stow/bar

# Choose the type of build, options are: None(CMAKE_CXX_FLAGS or
# CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.
# -D CMAKE_BUILD_TYPE:STRING=Debug

# If this value is on, makefiles will be generated without the
# .SILENT directive, and all commands will be echoed to the console
# during the make.  This is useful for debugging only.
# -D CMAKE_VERBOSE_MAKEFILE:BOOL=ON

####################################################################################################

project(QtCarto)

set(QTCARTO_VERSION_MAJOR 1)
set(QTCARTO_VERSION_MINOR 0)
set(QTCARTO_VERSION_PATCH 0)

set(QTCARTO_VERSION ${QTCARTO_VERSION_MAJOR}.${QTCARTO_VERSION_MINOR}.${QTCARTO_VERSION_PATCH})

####################################################################################################

# check cmake version
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

if(POLICY CMP0990)
  cmake_policy(SET CMP0990 NEW)
endif()

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

####################################################################################################
#
# Test
#

enable_testing()

####################################################################################################
#
# Compiler Options
#

SET(CMAKE_BUILD_TYPE RELEASE)
SET(CMAKE_VERBOSE_MAKEFILE ON)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
if(COMPILER_SUPPORTS_CXX14)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(CMAKE_CXX_FLAGS_COMMON  "-Wall -DSystemLinux")
  set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_COMMON} -g -O0 -DDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_COMMON} $ENV{GCC_OPTIMISATION}")
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
endif(CMAKE_SYSTEM_NAME MATCHES "Windows")

####################################################################################################
#
# Find Qt
#

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Test REQUIRED)

# INCLUDE_DIRECTORIES(${Qt5Core_INCLUDES})
# ADD_DEFINITIONS(${Qt5Core_DEFINITIONS})
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")

####################################################################################################
#
# Sub directories
#

INCLUDE_DIRECTORIES(src)

add_subdirectory(src)
add_subdirectory(test)

####################################################################################################
#
# End
#
####################################################################################################
