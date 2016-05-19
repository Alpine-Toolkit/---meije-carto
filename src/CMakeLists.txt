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

  cache/file_tile_cache.cpp
  cache/offline_cache.cpp
  cache/tile_image.cpp

  configuration/configuration.cpp

  coordinate/geo_coordinate.cpp

  geo_data_format/gpx.cpp
  geo_data_format/route.cpp
  geo_data_format/waypoint.cpp
  geo_data_format/xml_reader.cpp

  geometry/line.cpp
  geometry/path.cpp
  geometry/polygon.cpp
  geometry/vector.cpp

  map/map_event_router.cpp
  map/map_path_editor.cpp
  map/map_view.cpp
  map/viewport.cpp

  math/interval.cpp
  math/qc_math.cpp
  math/rational.cpp

  scene/location_circle_material_shader.cpp
  scene/location_circle_node.cpp
  scene/path_material_shader.cpp
  scene/path_node.cpp
  scene/map_layer_scene.cpp
  scene/map_scene.cpp

  tools/debug_data.cpp
  tools/logger.cpp
  tools/platform.cpp

  wmts/geoportail/geoportail_license.cpp
  wmts/geoportail/geoportail_plugin.cpp
  wmts/geoportail/geoportail_wmts_reply.cpp
  wmts/geoportail/geoportail_wmts_tile_fetcher.cpp
  wmts/osm/osm_plugin.cpp
  wmts/osm/osm_wmts_reply.cpp
  wmts/osm/osm_wmts_tile_fetcher.cpp
  wmts/tile_matrix_index.cpp
  wmts/tile_matrix_set.cpp
  wmts/tile_spec.cpp
  wmts/wmts_manager.cpp
  wmts/wmts_plugin.cpp
  wmts/wmts_plugin_manager.cpp
  wmts/wmts_reply.cpp
  wmts/wmts_request_manager.cpp
  wmts/wmts_tile_fetcher.cpp
  )
add_library(qtcarto SHARED ${qtcarto_files})
target_link_libraries(qtcarto Qt5::Core Qt5::Quick Qt5::Gui Qt5::Network Qt5::Positioning ${PROJ4_LIBRARIES})

####################################################################################################
#
# End
#
####################################################################################################
