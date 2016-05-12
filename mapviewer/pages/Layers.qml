import QtQml 2.2
import QtQuick 2.6
import QtQuick.Window 2.2

import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0

Pane {
    id: layer_pane

    Column {
        anchors.fill: parent

        Repeater {
            model: map.plugin_names

            Column {
                property string plugin_name: modelData

                Text {
                    font.pixelSize: 20
                    font.bold: true
                    text: plugin_name
                }

                Column {
                    width: layer_pane.width

                    Repeater {
                        model: map.plugin_layers(plugin_name)

                        Row {
                            property var plugin_layer: modelData

                            CheckBox {
                                id: checkbox
                                anchors.verticalCenter: parent.verticalCenter
                                checked: plugin_layer.status
                            }
                            Binding { target: plugin_layer; property: "status"; value: checkbox.checked }
                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                width: layer_pane.width / 3
                                text: plugin_layer.title
                            }
                            Slider {
                                id: slider
                                anchors.verticalCenter: parent.verticalCenter
                                enabled: checkbox.checked
                                value: plugin_layer.opacity
                            }
                            Binding { target: plugin_layer; property: "opacity"; value: slider.value }
                        }
                    }
                }
            }
        }
    }
}
