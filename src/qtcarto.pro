####################################################################################################

TARGET = qtcarto
TEMPLATE = lib

# CONFIG += c++11
CONFIG += c++14

QT -= gui

DEFINES += QTCARTO_LIBRARY

SOURCES += geo_coordinate.cpp

HEADERS += qtcarto.h \
           qtcarto_global.h \
	   geo_coordinate.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

####################################################################################################
#
# End
#
####################################################################################################
