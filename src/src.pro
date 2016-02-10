####################################################################################################

TARGET = qtcarto
TEMPLATE = lib

! include( ../common.pri ) {
  error( "Couldn't find the common.pri file!" )
}

QT -= gui

DEFINES += QTCARTO_LIBRARY

LIBS += -lproj

SOURCES += \
  map/geo_coordinate.cpp \
  map/tile_matrix_set.cpp \
  math/polygon.cpp \
  math/qc_math.cpp

HEADERS += \
  map/earth.h \
  map/geo_coordinate.h \
  map/tile_matrix_set.h \
  math/interval.h \
  math/line.h \
  math/polygon.h \
  math/qc_math.h \
  math/segment.h \
  math/vector.h \
  qtcarto.h \
  qtcarto_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

####################################################################################################
#
# End
#
####################################################################################################
