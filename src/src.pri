# -*- mode: qmake -*-

####################################################################################################

QT *= core quick gui network positioning

DEFINES += QTCARTO_LIBRARY

# DEFINES += WITH_PROJ4
linux:!android {
LIBS += -lproj
}
android {
INCLUDEPATH += $$PWD/../third-parties/proj4/proj.4.git/src
LIBS += -L$$PWD/../android-cmake-build/third-parties/proj4 -lqtcarto_proj4
}

# contains(ANDROID_TARGET_ARCH, armeabi-v7a) {}

SOURCES += \
  cache/file_tile_cache.cpp \
  cache/offline_cache.cpp \
  cache/tile_image.cpp

SOURCES += \
  configuration/configuration.cpp

SOURCES += \
  coordinate/mercator.cpp \
  coordinate/projection.cpp \
  coordinate/sexagesimal_angle.cpp \
  coordinate/wgs84.cpp \
  coordinate/debug_tools.cpp

SOURCES += \
  geo_data_format/gpx.cpp \
  geo_data_format/route.cpp \
  geo_data_format/waypoint.cpp \
  geo_data_format/xml_reader.cpp

SOURCES += \
  geometry/line.cpp \
  geometry/path.cpp \
  geometry/polygon.cpp \
  geometry/vector.cpp

SOURCES += \
  map/location_circle_data.cpp \
  map/map_event_router.cpp \
  map/map_path_editor.cpp \
  map/map_view.cpp \
  map/viewport.cpp

SOURCES += \
  math/interval.cpp \
  math/qc_math.cpp \
  math/rational.cpp

SOURCES += \
  scene/location_circle_material_shader.cpp \
  scene/location_circle_node.cpp \
  scene/path_material_shader.cpp \
  scene/path_node.cpp \
  scene/map_layer_scene.cpp \
  scene/map_scene.cpp

SOURCES += \
  tools/debug_data.cpp \
  tools/logger.cpp \
  tools/platform.cpp

SOURCES += \
  wmts/elevation_service_reply.cpp \
  wmts/location_service_query.cpp \
  wmts/location_service_reply.cpp \
  wmts/network_reply.cpp \
  wmts/providers/artic_web_map/artic_web_map_plugin.cpp \
  wmts/providers/austria/austria_plugin.cpp \
  wmts/providers/geoportail/geoportail_elevation_service_reply.cpp \
  wmts/providers/geoportail/geoportail_license.cpp \
  wmts/providers/geoportail/geoportail_location_service_reply.cpp \
  wmts/providers/geoportail/geoportail_plugin.cpp \
  wmts/providers/germany/germany_plugin.cpp \
  wmts/providers/osm/osm_plugin.cpp \
  wmts/providers/spain/spain_plugin.cpp \
  wmts/providers/swiss_confederation/swiss_confederation_plugin.cpp \
  wmts/tile_matrix_index.cpp \
  wmts/tile_matrix_set.cpp \
  wmts/tile_spec.cpp \
  wmts/wmts_manager.cpp \
  wmts/wmts_network_reply.cpp \
  wmts/wmts_network_tile_fetcher.cpp \
  wmts/wmts_plugin.cpp \
  wmts/wmts_plugin_manager.cpp \
  wmts/wmts_reply.cpp \
  wmts/wmts_request_manager.cpp \
  wmts/wmts_tile_fetcher.cpp

HEADERS += \
  cache/file_tile_cache.h \
  cache/offline_cache.h \
  cache/tile_image.h

HEADERS += \
  configuration/configuration.h

HEADERS += \
  coordinate/mercator.h \
  coordinate/projection.h \
  coordinate/sexagesimal_angle.h \
  coordinate/wgs84.h \
  coordinate/debug_tools.h

HEADERS += \
  geo_data_format/gpx.h \
  geo_data_format/route.h \
  geo_data_format/waypoint.h \
  geo_data_format/xml_reader.h

HEADERS += \
  geometry/line.h \
  geometry/path.h \
  geometry/polygon.h \
  geometry/vector.h

HEADERS += \
  map/map_event_router.h \
  map/map_path_editor.h \
  map/map_view.h \
  map/viewport.h

HEADERS += \
  math/interval.h \
  math/qc_math.h \
  math/rational.h

HEADERS += \
  scene/location_circle_material_shader.h \
  scene/location_circle_node.h \
  scene/path_material_shader.h \
  scene/path_node.h \
  scene/map_scene.h

HEADERS += \
  tools/debug_data.h \
  tools/logger.h \
  tools/platform.h

HEADERS += \
  wmts/elevation_service_reply.h \
  wmts/location_service_query.h \
  wmts/location_service_reply.h \
  wmts/network_reply.h \
  wmts/providers/artic_web_map/artic_web_map_plugin.h \
  wmts/providers/austria/austria_plugin.h \
  wmts/providers/geoportail/geoportail_elevation_service_reply.h \
  wmts/providers/geoportail/geoportail_license.h \
  wmts/providers/geoportail/geoportail_location_service_reply.h \
  wmts/providers/geoportail/geoportail_plugin.h \
  wmts/providers/germany/germany_plugin.h \
  wmts/providers/osm/osm_plugin.h \
  wmts/providers/spain/spain_plugin.h \
  wmts/providers/swiss_confederation/swiss_confederation_plugin.h \
  wmts/tile_matrix_index.h \
  wmts/tile_matrix_set.h \
  wmts/tile_spec.h \
  wmts/wmts_manager.h \
  wmts/wmts_network_reply.h \
  wmts/wmts_network_tile_fetcher.h \
  wmts/wmts_plugin.h \
  wmts/wmts_plugin_manager.h \
  wmts/wmts_reply.h \
  wmts/wmts_request_manager.h \
  wmts/wmts_tile_fetcher.h

####################################################################################################
#
# End
#
####################################################################################################
