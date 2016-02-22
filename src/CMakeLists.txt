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
  map/file_tile_cache.cpp
  map/geo_coordinate.cpp
  map/geoportail_license.cpp
  map/geoportail_wmts_reply.cpp
  map/geoportail_wmts_tile_fetcher.cpp
  map/map_view.cpp
  map/mosaic_painter.cpp
  map/tile_matrix_index.cpp
  map/tile_matrix_set.cpp
  map/tile_spec.cpp
  map/viewport.cpp
  map/wmts_manager.cpp
  map/wmts_reply.cpp
  # map/wmts_request_manager.cpp
  map/wmts_tile_fetcher.cpp
  math/interval.cpp
  math/polygon.cpp
  math/qc_math.cpp
  math/rational.cpp
  math/vector.cpp
)
add_library(qtcarto SHARED ${qtcarto_files})
target_link_libraries(qtcarto Qt5::Core Qt5::Gui Qt5::Network ${PROJ4_LIBRARIES})

####################################################################################################
#
# End
#
####################################################################################################
