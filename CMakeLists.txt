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

####################################################################################################

# check cmake version
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

if(POLICY CMP0990)
  cmake_policy(SET CMP0990 NEW)
endif()

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

include(qtcarto_functions)

####################################################################################################

if(ANDROID)
  message(STATUS "Android build")
endif()

####################################################################################################
#
# Test
#

if(NOT ANDROID)
  enable_testing()
endif(NOT ANDROID)

####################################################################################################
#
# Version
#

set(QTCARTO_VERSION_MAJOR 1)
set(QTCARTO_VERSION_MINOR 0)
set(QTCARTO_VERSION_PATCH 0)

set(QTCARTO_VERSION ${QTCARTO_VERSION_MAJOR}.${QTCARTO_VERSION_MINOR}.${QTCARTO_VERSION_PATCH})

####################################################################################################
#
# Target Platform
#

if(ANDROID)
 ADD_DEFINITIONS(-DON_ANDROID)
endif(ANDROID)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  ADD_DEFINITIONS(-DON_LINUX)
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  ADD_DEFINITIONS(-DON_WINDOWS)
endif(CMAKE_SYSTEM_NAME MATCHES "Windows")

####################################################################################################
#
# Compiler Options
#

if(NOT ANDROID)
  # SET(CMAKE_BUILD_TYPE RELEASE)
  SET(CMAKE_BUILD_TYPE DEBUG)
endif(NOT ANDROID)

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
  set(CMAKE_CXX_FLAGS_COMMON  "-Wall -DSystemLinux -fdiagnostics-color=auto")
  set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_COMMON} -g -O0 -DDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_COMMON} $ENV{GCC_OPTIMISATION}")
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
endif(CMAKE_SYSTEM_NAME MATCHES "Windows")

####################################################################################################
#
# Find Qt
#

if(NOT ANDROID)
  # Find includes in corresponding build directories
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  # Instruct CMake to run moc automatically when needed.
  set(CMAKE_AUTOMOC ON)
  # Instruct CMake to run rcc automatically when needed.
  set(CMAKE_AUTORCC ON)

  find_package(Qt5Core REQUIRED)
  find_package(Qt5Gui REQUIRED)
  find_package(Qt5Network REQUIRED)
  find_package(Qt5Positioning REQUIRED)
  find_package(Qt5Qml REQUIRED)
  find_package(Qt5Quick REQUIRED)
  find_package(Qt5QuickWidgets REQUIRED)
  find_package(Qt5Sensors REQUIRED)
  find_package(Qt5Sql REQUIRED)
  find_package(Qt5Svg REQUIRED)
  find_package(Qt5Test REQUIRED)

  # find_package(Qt5Xml REQUIRED)
  # find_package(Qt5WebView REQUIRED)

  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_QML_DEBUG") # Fixme: right way to do?

  # INCLUDE_DIRECTORIES(${Qt5Core_INCLUDES})
  # ADD_DEFINITIONS(${Qt5Core_DEFINITIONS})
  # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
endif(NOT ANDROID)

####################################################################################################
#
# Find Proj4
#

if(NOT ANDROID)
  include(FindProj4)
  if(PROJ4_FOUND)
    add_definitions(-DWITH_PROJ4)
    INCLUDE_DIRECTORIES(PROJ4_INCLUDE_DIR)
  endif(PROJ4_FOUND)
endif(NOT ANDROID)

####################################################################################################

configure_file(config.h.in config.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

####################################################################################################
#
# Sub directories
#

if(ANDROID)
  add_subdirectory(third-parties)
endif(ANDROID)

if(NOT ANDROID)
  INCLUDE_DIRECTORIES(src)
endif(NOT ANDROID)

if(NOT ANDROID)
  add_subdirectory(src)
  add_subdirectory(imports)
  add_subdirectory(mapviewer)
endif(NOT ANDROID)

if(NOT ANDROID)
  add_subdirectory(tools)
  add_subdirectory(test)
endif(NOT ANDROID)

####################################################################################################
#
# End
#
####################################################################################################
