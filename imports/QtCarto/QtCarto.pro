####################################################################################################

TARGET = qmlqtcartoplugin
TEMPLATE = lib

! include( ../../common.pri ) {
  error( "Couldn't find the common.pri file!" )
}

CONFIG += plugin

QT += qml # quick network core gui

# INCLUDEPATH += ../../location
# INCLUDEPATH *= $$PWD

# HEADERS += \
#  qdeclarativegeomap_p.h

SOURCES += \
  qtcarto.cpp # \
#  qdeclarativegeomap.cpp

# include(declarativeplaces/declarativeplaces.pri)

# load(qml_plugin)

# LIBS_PRIVATE += -L$$MODULE_BASE_OUTDIR/lib

# OTHER_FILES += \
#   plugin.json \
#   qmldir

####################################################################################################
#
# End
#
####################################################################################################
