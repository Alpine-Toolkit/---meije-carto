####################################################################################################

TARGET = qtcarto
TEMPLATE = lib

! include( ../common.pri ) {
  error( "Couldn't find the common.pri file!" )
}

CONFIG += dll

QT += core quick gui network positioning

TARGET = $$qtLibraryTarget($$TARGET)

DEFINES += QTCARTO_LIBRARY
# DEFINES -= WITH_PROJ4

# LIBS += -lproj

SOURCES += \
  geo_data_format/gpx.cpp \
  geo_data_format/route.cpp \
  geo_data_format/waypoint.cpp \
  geo_data_format/xml_reader.cpp \
  map/file_tile_cache.cpp \
  map/geo_coordinate.cpp \
  map/geoportail/geoportail_license.cpp \
  map/geoportail/geoportail_plugin.cpp \
  map/geoportail/geoportail_wmts_reply.cpp \
  map/geoportail/geoportail_wmts_tile_fetcher.cpp \
  map/location_circle_material_shader.cpp \
  map/map_scene.cpp \
  map/map_view.cpp \
  map/osm/osm_plugin.cpp \
  map/osm/osm_wmts_reply.cpp \
  map/osm/osm_wmts_tile_fetcher.cpp \
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
  math/polygon.cpp \
  math/qc_math.cpp \
  math/rational.cpp \
  math/vector.cpp

# PUBLIC_HEADERS += \

HEADERS += \
  geo_data_format/gpx.h \
  geo_data_format/route.h \
  geo_data_format/waypoint.h \
  geo_data_format/xml_reader.h \
  map/cache3q.h \
  map/earth.h \
  map/file_tile_cache.h \
  map/geo_coordinate.h \
  map/geoportail/geoportail_license.h \
  map/geoportail/geoportail_plugin.h \
  map/geoportail/geoportail_wmts_reply.h \
  map/geoportail/geoportail_wmts_tile_fetcher.h \
  map/location_circle_material_shader.h \
  map/map_scene.h \
  map/map_view.h \
  map/osm/osm_plugin.h \
  map/osm/osm_wmts_reply.h \
  map/osm/osm_wmts_tile_fetcher.h \
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
  math/line.h \
  math/polygon.h \
  math/qc_math.h \
  math/rational.h \
  math/segment.h \
  math/vector.h \
  qtcarto_global.h \
  qtcarto.h

unix|android {
  target.path = $$[QT_INSTALL_LIBS] # Fixme doesn't work ???
  message(Install to $$target.path)
  INSTALLS += target
}

####################################################################################################
#
# End
#
####################################################################################################
