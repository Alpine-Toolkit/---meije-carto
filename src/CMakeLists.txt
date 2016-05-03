# -*- CMake -*-

####################################################################################################
#
# Include directories
#

# include_directories(${CMAKE_CURRENT_SOURCE_DIR})

####################################################################################################

# Generate in source !
add_custom_command (
  OUTPUT map/shaders/location_circle_shader.h
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/map/shaders/make-shader location_circle
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/map/shaders
  DEPENDS
  map/shaders/location_circle_vertex_shader.glsl
  map/shaders/location_circle_fragment_shader.glsl
)

####################################################################################################
#
# Library
#

# geometry/polygon_seidler_triangulation.cpp

set(qtcarto_files
  # map/shaders/location_circle_shader.h
  geo_data_format/gpx.cpp
  geo_data_format/route.cpp
  geo_data_format/waypoint.cpp
  geo_data_format/xml_reader.cpp
  geometry/line.cpp
  geometry/path.cpp
  geometry/polygon.cpp
  geometry/vector.cpp
  map/file_tile_cache.cpp
  map/geo_coordinate.cpp
  map/geoportail/geoportail_license.cpp
  map/geoportail/geoportail_plugin.cpp
  map/geoportail/geoportail_wmts_reply.cpp
  map/geoportail/geoportail_wmts_tile_fetcher.cpp
  map/location_circle_material_shader.cpp
  map/map_scene.cpp
  map/map_view.cpp
  map/offline_cache.cpp
  map/osm/osm_plugin.cpp
  map/osm/osm_wmts_reply.cpp
  map/osm/osm_wmts_tile_fetcher.cpp
  map/tile_image.cpp
  map/tile_matrix_index.cpp
  map/tile_matrix_set.cpp
  map/tile_spec.cpp
  map/viewport.cpp
  map/wmts_manager.cpp
  map/wmts_plugin.cpp
  map/wmts_reply.cpp
  map/wmts_request_manager.cpp
  map/wmts_tile_fetcher.cpp
  math/interval.cpp
  math/qc_math.cpp
  math/rational.cpp
  tools/logger.cpp
  )
add_library(qtcarto SHARED ${qtcarto_files})
target_link_libraries(qtcarto Qt5::Core Qt5::Quick Qt5::Gui Qt5::Network Qt5::Positioning ${PROJ4_LIBRARIES})

####################################################################################################
#
# End
#
####################################################################################################
