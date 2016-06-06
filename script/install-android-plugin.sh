####################################################################################################
#
# Run from build directory
#

QT_ANDROID_PATH="/opt/Qt-dev-5.7.0-android"
QML_PLUGIN_PATH="${QT_ANDROID_PATH}/qml/QtCarto"

mkdir ${QML_PLUGIN_PATH}
# cp imports/QtCarto/{*.so,qmldir} ${QML_PLUGIN_PATH}
cp imports/QtCarto/qmldir ${QML_PLUGIN_PATH}
ln -sf imports/QtCarto/libqmlqtcartoplugin.so ${QML_PLUGIN_PATH}

####################################################################################################
#
# End
#
####################################################################################################
