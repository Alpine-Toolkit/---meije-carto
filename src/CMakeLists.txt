# -*- CMake -*-

####################################################################################################
#
# Include directories
#

# include_directories(${CMAKE_CURRENT_SOURCE_DIR})

####################################################################################################
#
# Library
#

set(qtcarto_files
  map/geo_coordinate.cpp
  map/tile_matrix_set.cpp
  math/qc_math.cpp
  math/polygon.cpp
)
add_library(qtcarto SHARED ${qtcarto_files})
target_link_libraries(qtcarto Qt5::Core ${PROJ4_LIBRARIES})

####################################################################################################
#
# End
#
####################################################################################################
