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
  map/mosaic_painter.cpp
  map/tile_matrix_set.cpp
  map/viewport.cpp
  math/interval.cpp
  math/polygon.cpp
  math/qc_math.cpp
  math/rational.cpp
  math/vector.cpp
)
add_library(qtcarto SHARED ${qtcarto_files})
target_link_libraries(qtcarto Qt5::Core ${PROJ4_LIBRARIES})

####################################################################################################
#
# End
#
####################################################################################################
