###############################################################################
# CMake module to search for PROJ.4 library
#
# On success, the macro sets the following variables:
# PROJ4_FOUND       = if the library found
# PROJ4_VERSION     = version number of PROJ.4 found
# PROJ4_LIBRARIES   = full path to the library
# PROJ4_INCLUDE_DIR = where to find the library headers
# Also defined, but not for general use are
# PROJ4_LIBRARY, where to find the PROJ.4 library.
#
# Copyright (c) 2009 Mateusz Loskot <mateusz@loskot.net>
# Copyright (c) 2016 Fabrice Salvaire <fabrice.salvaire@orange.fr>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
###############################################################################

# PATHS ${PROJ4_PATH}/include
find_path(PROJ4_INCLUDE_DIR proj_api.h
  DOC "Path to PROJ.4 library include directory")

if (PROJ4_INCLUDE_DIR)
  # Extract version from proj_api.h (ex: 480)
  file(STRINGS ${PROJ4_INCLUDE_DIR}/proj_api.h
    PJ_VERSIONSTR
    REGEX "#define[ ]+PJ_VERSION[ ]+[0-9]+")
  string(REGEX MATCH "[0-9]+" PJ_VERSIONSTR ${PJ_VERSIONSTR})

  # TODO: Should be formatted as 4.8.0?
  set(PROJ4_VERSION ${PJ_VERSIONSTR})
endif()

# PATHS ${PROJ4_PATH}/lib
find_library(PROJ4_LIBRARY
  NAMES proj
  DOC "Path to PROJ.4 library file")

if(PROJ4_LIBRARY)
  set(PROJ4_LIBRARIES ${PROJ4_LIBRARY})
endif()

# Handle the QUIETLY and REQUIRED arguments and set PROJ4_FOUND to TRUE
# if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PROJ4 DEFAULT_MSG
  PROJ4_LIBRARY
  PROJ4_INCLUDE_DIR)

####################################################################################################
#
# End
#
####################################################################################################
