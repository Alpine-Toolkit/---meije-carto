####################################################################################################

TEMPLATE = lib
TARGET = qmlqtcartoplugin # qml-plugin

!include(../../common.pri) {
  error( "Couldn't find the common.pri file!" )
}

CONFIG += plugin

QT += qml quick positioning

TARGET = $$qtLibraryTarget($$TARGET)

# Fixme : Private QT API
INCLUDEPATH += /opt/Qt-dev-5.7.0/include/QtCore/5.7.0/
INCLUDEPATH += /opt/Qt-dev-5.7.0/include/QtCore/5.7.0/QtCore
INCLUDEPATH += /opt/Qt-dev-5.7.0/include/QtGui/5.7.0/
INCLUDEPATH += /opt/Qt-dev-5.7.0/include/QtGui/5.7.0/QtGui
INCLUDEPATH += /opt/Qt-dev-5.7.0/include/QtQml/5.7.0/QtQml
INCLUDEPATH += /opt/Qt-dev-5.7.0/include/QtQuick/5.7.0/
INCLUDEPATH += /opt/Qt-dev-5.7.0/include/QtQuick/5.7.0/QtQuick

SOURCES += \
  declarative_map_item.cpp \
  geo_coordinate_animation.cpp \
  map_gesture_area.cpp \
  plugin_data.cpp \
  qtcarto.cpp # qml-plugin_plugin

HEADERS += \
  declarative_map_item.h \
  geo_coordinate_animation.h \
  map_gesture_area.h \
  plugin_data.h

# libqtcarto
VPATH += $$PWD/../../src
include($$PWD/../../src/src.pri)
INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src

# unix:!macx: LIBS += -L$$OUT_PWD/../..src/ -lqtcarto

# Install to /home/opt/Qt-dev-5.7.0/qml/fr/alpine_toolkit/QtCarto
#  libqmlqtcartoplugin.so
#  qmldir

DISTFILES = qmldir

# Copies the qmldir file to the build directory
!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

# Copies the qmldir file and the built plugin .so to the QT_INSTALL_QML directory
# uri = fr.alpine_toolkit.QtCarto
uri = QtCarto
qmldir.files = qmldir
unix {
    install_path = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$install_path
    target.path = $$install_path
    INSTALLS += target qmldir
}

####################################################################################################
#
# End
#
####################################################################################################
