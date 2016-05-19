import QtQml 2.2
import QtQuick 2.6
import QtQuick.Window 2.2

import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0

Pane {
    id: layer_pane
    width: application_window.width / 2

    // Fixme: width, vertical slider, title
    Column {
        anchors.fill: parent

        Repeater {
            model: map.plugins

            Column {
                property var plugin_data: modelData

                Text {
                    font.pixelSize: 20
                    font.bold: true
                    text: plugin_data.title
                }

                Column {
                    width: layer_pane.width

                    Repeater {
                        model: map.plugin_layers(plugin_data.name)

                        Row {
                            property var plugin_layer: modelData

                            CheckBox {
                                id: checkbox
                                anchors.verticalCenter: parent.verticalCenter
                                Component.onCompleted: checked = plugin_layer.status
                                onCheckedChanged: plugin_layer.status = checked
                            }
                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                width: layer_pane.width / 3
                                text: plugin_layer.title
                            }
                            Slider {
                                id: slider
                                anchors.verticalCenter: parent.verticalCenter
                                enabled: checkbox.checked
                                Component.onCompleted: value = plugin_layer.opacity
                                onValueChanged: plugin_layer.opacity = value
                            }
                        }
                    }
                }
            }
        }
    }
}
