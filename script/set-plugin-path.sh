####################################################################################################
#
# $QT/qml/QtCarto/
#  qmldir
#  libqmlqtcartoplugin.so

cat > imports/QtCarto/qmldir <<EOF
module QtCarto
plugin qmlqtcartoplugin
classname QtCartoDeclarativeModule
EOF

export QML2_IMPORT_PATH=$PWD/imports

####################################################################################################
#
# End
#
####################################################################################################
