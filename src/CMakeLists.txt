# -*- CMake -*-

####################################################################################################
#
# Include directories
#

# include_directories(${CMAKE_CURRENT_SOURCE_DIR})

####################################################################################################

# Generate in source !
foreach(shader_name location_circle path)
  add_custom_command(
    OUTPUT map/shaders/${shader_name}_shader.h
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/map/shaders/make-shader ${shader_name}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/map/shaders
    DEPENDS
    map/shaders/${shader_name}_vertex_shader.glsl
    map/shaders/${shader_name}_fragment_shader.glsl
    )
endforeach(shader_name)

####################################################################################################
#
# Library
#

# geometry/polygon_seidler_triangulation.cpp

set(qtcarto_files
  cache/file_tile_cache.cpp
  cache/offline_cache.cpp
  cache/offline_cache_database.cpp
  cache/tile_image.cpp

  configuration/configuration.cpp

  coordinate/mercator.cpp
  coordinate/projection.cpp
  coordinate/sexagesimal_angle.cpp
  coordinate/wgs84.cpp
  coordinate/debug_tools.cpp

  geo_data_format/gpx.cpp
  geo_data_format/route.cpp
  geo_data_format/waypoint.cpp
  geo_data_format/xml_reader.cpp

  geometry/line.cpp
  geometry/path.cpp
  geometry/polygon.cpp
  geometry/polygon_seidler_triangulation.cpp
  geometry/vector.cpp

  map/location_circle_data.cpp
  map/map_event_router.cpp
  map/map_path_editor.cpp
  map/map_view.cpp
  map/path_property.cpp
  map/viewport.cpp
  map/decorated_path.cpp

  math/interval.cpp
  math/qc_math.cpp
  math/rational.cpp

  scene/location_circle_material_shader.cpp
  scene/location_circle_node.cpp
  scene/map_layer_scene.cpp
  scene/map_scene.cpp
  scene/path_material_shader.cpp
  scene/path_node.cpp
  scene/point_material_shader.cpp

  tools/debug_data.cpp
  tools/logger.cpp
  tools/platform.cpp

  wmts/elevation_service_reply.cpp
  wmts/location_service_query.cpp
  wmts/location_service_reply.cpp
  wmts/network_reply.cpp
  wmts/providers/artic_web_map/artic_web_map_plugin.cpp
  wmts/providers/austria/austria_plugin.cpp
  wmts/providers/geoportail/geoportail_elevation_service_reply.cpp
  wmts/providers/geoportail/geoportail_license.cpp
  wmts/providers/geoportail/geoportail_location_service_reply.cpp
  wmts/providers/geoportail/geoportail_plugin.cpp
  wmts/providers/germany/germany_plugin.cpp
  wmts/providers/osm/osm_plugin.cpp
  wmts/providers/spain/spain_plugin.cpp
  wmts/providers/swiss_confederation/swiss_confederation_plugin.cpp
  wmts/tile_matrix_index.cpp
  wmts/tile_matrix_set.cpp
  wmts/tile_spec.cpp
  wmts/wmts_manager.cpp
  wmts/wmts_network_reply.cpp
  wmts/wmts_network_tile_fetcher.cpp
  wmts/wmts_plugin.cpp
  wmts/wmts_plugin_manager.cpp
  wmts/wmts_reply.cpp
  wmts/wmts_request_manager.cpp
  wmts/wmts_tile_fetcher.cpp
  )
add_library(qtcarto SHARED ${qtcarto_files})
target_link_libraries(qtcarto Qt5::Core Qt5::Quick Qt5::Gui Qt5::Network Qt5::Positioning Qt5::Sql ${PROJ4_LIBRARIES})

####################################################################################################
#
# End
#
####################################################################################################
