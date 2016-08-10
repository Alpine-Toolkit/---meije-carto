####################################################################################################

TEMPLATE = app
TARGET = mapviewer-atk

!include(../common.pri) {
  error( "Couldn't find the common.pri file!" )
}

CONFIG += qml_debug

QT += core gui qml quick

INCLUDEPATH += ../src

SOURCES += main.cpp

RESOURCES += mapviewer.qrc

# Fixme: imports ?
linux:!android {
LIBS += -lproj
}
android {
INCLUDEPATH += $$PWD/../third-parties/proj4/proj.4.git/src
LIBS += -L$$PWD/../android-cmake-build/third-parties/proj4 -lqtcarto_proj4
# LIBS += -lcrypto -lssl
}

# cf. http://doc.qt.io/qtcreator/creator-project-qmake-libraries.html
unix: LIBS += -L$$OUT_PWD/../imports/QtCarto -lqmlqtcartoplugin
DEPENDPATH += $$PWD/../imports/QtCarto

#? QML_IMPORT_PATH = ../imports

contains(ANDROID_TARGET_ARCH, armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
    $$PWD/../android-cmake-build/third-parties/proj4/libqtcarto_proj4.so \
    $$PWD/../third-parties/openssl-1.0.2h/libcrypto.so \
    $$PWD/../third-parties/openssl-1.0.2h/libssl.so
    # /opt/Qt-dev-5.7.0-android/lib/libqtcarto.so
}

# android {
#     proj4_data.path = /assets/proj4_data
#     proj4_data.files += $$PWD/../third-parties/proj4/proj.4.git/nad/epsg
#     INSTALLS += proj4_data
# }

####################################################################################################
#
# End
#
####################################################################################################
