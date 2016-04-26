####################################################################################################

TEMPLATE = lib
TARGET = qmlqtcartoplugin # qml-plugin

! include( ../../common.pri ) {
  error( "Couldn't find the common.pri file!" )
}

CONFIG += plugin

QT += qml quick positioning

TARGET = $$qtLibraryTarget($$TARGET)

# Fixme : Private QT API
INCLUDEPATH += /opt/Qt-dev-5.7.0/include/QtQuick/5.7.0/QtQuick
INCLUDEPATH += /opt/Qt-dev-5.7.0/include/QtQml/5.7.0/QtQml
INCLUDEPATH += /opt/Qt-dev-5.7.0/include/QtCore/5.7.0/QtCore
INCLUDEPATH += /opt/Qt-dev-5.7.0/include/QtCore/5.7.0

SOURCES += \
  declarative_map_item.cpp \
  geo_coordinate_animation.cpp \
  map_gesture_area.cpp \
  qtcarto.cpp # qml-plugin_plugin

HEADERS += \
  declarative_map_item.h \
  geo_coordinate_animation.h \
  map_gesture_area.h

############
# libqtcarto

QT *= core quick gui network positioning

DEFINES += QTCARTO_LIBRARY

SOURCES += \
  ../../src/geo_data_format/gpx.cpp \
  ../../src/geo_data_format/route.cpp \
  ../../src/geo_data_format/waypoint.cpp \
  ../../src/geo_data_format/xml_reader.cpp \
  ../../src/map/file_tile_cache.cpp \
  ../../src/map/geo_coordinate.cpp \
  ../../src/map/geoportail/geoportail_license.cpp \
  ../../src/map/geoportail/geoportail_plugin.cpp \
  ../../src/map/geoportail/geoportail_wmts_reply.cpp \
  ../../src/map/geoportail/geoportail_wmts_tile_fetcher.cpp \
  ../../src/map/location_circle_material_shader.cpp \
  ../../src/map/map_scene.cpp \
  ../../src/map/map_view.cpp \
  ../../src/map/osm/osm_plugin.cpp \
  ../../src/map/osm/osm_wmts_reply.cpp \
  ../../src/map/osm/osm_wmts_tile_fetcher.cpp \
  ../../src/map/tile_matrix_index.cpp \
  ../../src/map/tile_matrix_set.cpp \
  ../../src/map/tile_spec.cpp \
  ../../src/map/viewport.cpp \
  ../../src/map/wmts_manager.cpp \
  ../../src/map/wmts_plugin.cpp \
  ../../src/map/wmts_reply.cpp \
  ../../src/map/wmts_request_manager.cpp \
  ../../src/map/wmts_tile_fetcher.cpp \
  ../../src/math/interval.cpp \
  ../../src/math/polygon.cpp \
  ../../src/math/qc_math.cpp \
  ../../src/math/rational.cpp \
  ../../src/math/vector.cpp

HEADERS += \
  ../../src/geo_data_format/gpx.h \
  ../../src/geo_data_format/route.h \
  ../../src/geo_data_format/waypoint.h \
  ../../src/geo_data_format/xml_reader.h \
  ../../src/map/cache3q.h \
  ../../src/map/earth.h \
  ../../src/map/file_tile_cache.h \
  ../../src/map/geo_coordinate.h \
  ../../src/map/geoportail/geoportail_license.h \
  ../../src/map/geoportail/geoportail_plugin.h \
  ../../src/map/geoportail/geoportail_wmts_reply.h \
  ../../src/map/geoportail/geoportail_wmts_tile_fetcher.h \
  ../../src/map/location_circle_material_shader.h \
  ../../src/map/map_scene.h \
  ../../src/map/map_view.h \
  ../../src/map/osm/osm_plugin.h \
  ../../src/map/osm/osm_wmts_reply.h \
  ../../src/map/osm/osm_wmts_tile_fetcher.h \
  ../../src/map/tile_matrix_index.h \
  ../../src/map/tile_matrix_set.h \
  ../../src/map/tile_spec.h \
  ../../src/map/viewport.h \
  ../../src/map/wmts_manager.h \
  ../../src/map/wmts_plugin.h \
  ../../src/map/wmts_reply.h \
  ../../src/map/wmts_request_manager.h \
  ../../src/map/wmts_tile_fetcher.h \
  ../../src/math/interval.h \
  ../../src/math/line.h \
  ../../src/math/polygon.h \
  ../../src/math/qc_math.h \
  ../../src/math/rational.h \
  ../../src/math/segment.h \
  ../../src/math/vector.h \
  ../../src/qtcarto_global.h \
  ../../src/qtcarto.h

# libqtcarto
############

# unix:!macx: LIBS += -L$$OUT_PWD/../..src/ -lqtcarto

INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src

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
