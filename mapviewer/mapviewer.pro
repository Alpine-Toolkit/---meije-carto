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

contains(ANDROID_TARGET_ARCH, armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
    /opt/Qt-dev-5.7.0-android/lib/libqtcarto.so
    # /opt/Qt-dev-5.7.0-android/qml/QtCarto/libqmlqtcartoplugin.so
    # fr/alpine_toolkit/
}

####################################################################################################
#
# End
#
####################################################################################################
