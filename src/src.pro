####################################################################################################

TARGET = qtcarto
TEMPLATE = lib

! include( ../common.pri ) {
  error( "Couldn't find the common.pri file!" )
}

QT += core gui network

DEFINES += QTCARTO_LIBRARY

LIBS += -lproj

SOURCES += \
  map/file_tile_cache.cpp \
  map/geo_coordinate.cpp \
  map/geoportail_license.cpp \
  map/geoportail_wmts_reply.cpp \
  map/geoportail_wmts_tile_fetcher.cpp \
  map/map_view.cpp \
  map/mosaic_painter.cpp \
  map/tile_matrix_index.cpp \
  map/tile_matrix_set.cpp \
  map/tile_spec.cpp \
  map/viewport.cpp \
  map/wmts_manager.cpp \
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
  qtcarto_global.h \
  map/file_tile_cache.h \
  map/geo_coordinate.h \
  map/geoportail_license.h \
  map/geoportail_wmts_reply.h \
  map/geoportail_wmts_tile_fetcher.h \
  map/map_view.h \
  map/mosaic_painter.h \
  map/tile_matrix_index.h \
  map/tile_matrix_set.h \
  map/tile_spec.h \
  map/viewport.h \
  map/wmts_manager.h \
  map/wmts_reply.h \
  map/wmts_request_manager.h \
  map/wmts_tile_fetcher.h \
  math/interval.h \
  math/polygon.h \
  math/qc_math.h \
  math/rational.h \
  math/vector.h

# qtcarto.h \

unix {
    target.path = /usr/lib
    INSTALLS += target
}

####################################################################################################
#
# End
#
####################################################################################################
