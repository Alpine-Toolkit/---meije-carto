import QtQuick 2.6
import QtQuick.Window 2.2

import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0

// import QtLocation 5.3

ApplicationWindow {
    id: application_window
    visible: true
    width: 1280 // set window size for desktop test
    height: 800
    title: "Alpine Toolkit"

    Component.onCompleted: {
        console.info("screen",
                     Screen.height, Screen.width,
                     Screen.desktopAvailableHeight, Screen.desktopAvailableWidth,
                     Screen.pixelDensity, Screen.devicePixelRatio);
    }

    header: ToolBar {
        id: app_bar

        states: [
        State { name: "BACK" }
        ]

        RowLayout {
            spacing: 20
            anchors.fill: parent

            ToolButton {
                id: nav_icon
                visible: true // app_bar.state == ""
                contentItem: Image {
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: "qrc:/icons/menu-white.png"
                }
                onClicked: drawer.open()
            }

            ToolButton {
                id: back_icon
                visible: false // app_bar.state == "BACK"
                contentItem: Image {
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: "qrc:/icons/arrow-back-white.png"
                }
                onClicked: {
                    app_bar.state == ""
                    back_icon.visible = false
                    nav_icon.visible = true
                    stack_view.pop(StackView.Transition)
                }
            }

            Label {
                id: title_label
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
                font.pixelSize: 20
                elide: Label.ElideRight
                color: "white"
                text: "Alpine Toolkit"
            }

            ToolButton {
                id: menu_icon
                contentItem: Image {
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: "qrc:/icons/more-vert-white.png"
                }
                onClicked: options_menu.open()

                Menu {
                    id: options_menu
                    x: parent.width - width
                    transformOrigin: Menu.TopRight

                    MenuItem {
                        text: "Settings"
                    }
                    MenuItem {
                        text: "About"
                        onTriggered: about_dialog.open()
                    }
                }
            }
        }
    }

    Drawer {
        id: drawer
        width: Math.min(application_window.width, application_window.height) / 3 * 2
        height: application_window.height

        ListView {
            id: list_view
            currentIndex: -1
            anchors.fill: parent

            delegate: ItemDelegate {
                width: parent.width
                font.pixelSize: 16
                text: model.title
                highlighted: ListView.isCurrentItem
                onClicked: {
                    // if (list_view.currentIndex != index) {
                    list_view.currentIndex = index
                    app_bar.state == "BACK"
                    back_icon.visible = true
                    nav_icon.visible = false
                    title_label.text = model.title
                    stack_view.push(model.source)
                    // }
                    drawer.close()
                }
            }

            model: ListModel {
                ListElement {
                    title: qsTr("Layers")
                    icon: ""
                    source: "qrc:/pages/Layers.qml"
                }
            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }

    MapComponent {
        id: map
        anchors.fill: parent // anchored on header bottom
        focus: true

        // Component.onCompleted: {
        //     console.info(application_window.width, application_window.height, header.height)
        //    console.info(map.x, map.y, map.width, map.height)
        // }
    }

    StackView {
        id: stack_view
        anchors.fill: parent
        focus: true

        // initialItem: null
        // Fixme: hack
        initialItem: Pane {
            id: pane
            width: 0
            height: 0
        }

        // Map {
        //     id: map
        //     plugin: Plugin {
        //         name: "osm"
        //     }
        //     gesture.enabled: true
        // }
    }

    Popup {
        id: about_dialog
        modal: true
        focus: true
        x: (application_window.width - width) / 2
        y: application_window.height / 6
        width: Math.min(application_window.width, application_window.height) / 3 * 2
        contentHeight: about_column.height

        Column {
            id: about_column
            spacing: 20

            Label {
                font.bold: true
                text: "About"
            }

            Label {
                width: about_dialog.availableWidth
                wrapMode: Label.Wrap
                font.pixelSize: 12
                text: "Lorem lipsum ..."
            }
        }
    }
}
