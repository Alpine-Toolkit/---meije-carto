####################################################################################################

TARGET = test
TEMPLATE = app

! include( ../common.pri ) {
  error( "Couldn't find the common.pri file!" )
}

CONFIG += testcase

QT += testlib

INCLUDEPATH += ../src

LIBS += ../src/libqtcarto.so
PRE_TARGETDEPS += ../src/libqtcarto.so

LIBS += -lproj

SOURCES += test_geo_coordinate.cpp
# more than one main !

# test_interval.cpp \
# test_line.cpp \
# test_polygon.cpp \
# test_tile_matrix_set.cpp \
# test_vector.cpp

####################################################################################################
#
# End
#
####################################################################################################
