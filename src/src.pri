# -*- mode: cmake -*-

####################################################################################################

QT *= core quick gui network positioning

DEFINES += QTCARTO_LIBRARY
# DEFINES -= WITH_PROJ4

# LIBS += -lproj

SOURCES += \
  geo_data_format/gpx.cpp \
  geo_data_format/route.cpp \
  geo_data_format/waypoint.cpp \
  geo_data_format/xml_reader.cpp \
  geometry/line.cpp \
  geometry/path.cpp \
  geometry/polygon.cpp \
  geometry/vector.cpp \
  map/file_tile_cache.cpp \
  map/geo_coordinate.cpp \
  map/geoportail/geoportail_license.cpp \
  map/geoportail/geoportail_plugin.cpp \
  map/geoportail/geoportail_wmts_reply.cpp \
  map/geoportail/geoportail_wmts_tile_fetcher.cpp \
  map/location_circle_material_shader.cpp \
  map/map_scene.cpp \
  map/map_view.cpp \
  map/offline_cache.cpp \
  map/osm/osm_plugin.cpp \
  map/osm/osm_wmts_reply.cpp \
  map/osm/osm_wmts_tile_fetcher.cpp \
  map/tile_image.cpp \
  map/tile_matrix_index.cpp \
  map/tile_matrix_set.cpp \
  map/tile_spec.cpp \
  map/viewport.cpp \
  map/wmts_manager.cpp \
  map/wmts_plugin.cpp \
  map/wmts_reply.cpp \
  map/wmts_request_manager.cpp \
  map/wmts_tile_fetcher.cpp \
  math/interval.cpp \
  math/qc_math.cpp \
  math/rational.cpp \
  tools/logger.cpp

# PUBLIC_HEADERS += \

HEADERS += \
  geo_data_format/gpx.h \
  geo_data_format/route.h \
  geo_data_format/waypoint.h \
  geo_data_format/xml_reader.h \
  geometry/line.h \
  geometry/path.h \
  geometry/polygon.h \
  geometry/vector.h \
  map/file_tile_cache.h \
  map/geo_coordinate.h \
  map/geoportail/geoportail_license.h \
  map/geoportail/geoportail_plugin.h \
  map/geoportail/geoportail_wmts_reply.h \
  map/geoportail/geoportail_wmts_tile_fetcher.h \
  map/location_circle_material_shader.h \
  map/map_scene.h \
  map/map_view.h \
  map/offline_cache.h \
  map/osm/osm_plugin.h \
  map/osm/osm_wmts_reply.h \
  map/osm/osm_wmts_tile_fetcher.h \
  map/tile_image.h \
  map/tile_matrix_index.h \
  map/tile_matrix_set.h \
  map/tile_spec.h \
  map/viewport.h \
  map/wmts_manager.h \
  map/wmts_plugin.h \
  map/wmts_reply.h \
  map/wmts_request_manager.h \
  map/wmts_tile_fetcher.h \
  math/interval.h \
  math/qc_math.h \
  math/rational.h \
  tools/logger.h \
  qtcarto_global.h \
  qtcarto.h

####################################################################################################
#
# End
#
####################################################################################################
