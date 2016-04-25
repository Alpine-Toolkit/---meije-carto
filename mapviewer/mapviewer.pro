####################################################################################################

TEMPLATE = app
TARGET = mapviewer

! include( ../common.pri ) {
  error( "Couldn't find the common.pri file!" )
}

CONFIG += qml_debug

QT += core gui qml quick

SOURCES += main.cpp

RESOURCES += mapviewer.qrc

#? QML_IMPORT_PATH = ../imports

####################################################################################################
#
# End
#
####################################################################################################
