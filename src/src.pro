####################################################################################################

TARGET = qtcarto
TEMPLATE = lib

!include(../common.pri) {
  error("Couldn't find the common.pri file!")
}

# TARGET = $$qtLibraryTarget($$TARGET) # for windows ?

CONFIG += dll

include(src.pri)

unix|android {
  target.path = $$[QT_INSTALL_LIBS] # Fixme doesn't work ???
  # message(Install to $$target.path)
  INSTALLS += target
}

####################################################################################################
#
# End
#
####################################################################################################
