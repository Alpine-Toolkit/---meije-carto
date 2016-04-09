####################################################################################################

TARGET = test
TEMPLATE = app

! include( ../common.pri ) {
  error( "Couldn't find the common.pri file!" )
}

CONFIG += testcase

QT += testlib

INCLUDEPATH += ../src

# Fixme: libqtcarto.so.1 => not found
LIBS += ../src/libqtcarto.so
PRE_TARGETDEPS += ../src/libqtcarto.so

LIBS += -lproj

SOURCES += test_geo_coordinate.cpp
# Fixme: more than one main !

####################################################################################################
#
# End
#
####################################################################################################
