import QtQuick 2.6
import QtQuick.Window 2.2

import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import QtPositioning 5.5

import QtCarto 1.0 // fr.alpine_toolkit.

QcMapItem {
    id: map

    property bool position_locked: false
    property bool bearing_locked: false

    property int press_x : -1
    property int press_y : -1
    property int last_x : -1
    property int last_y : -1
    property int jitter_threshold : 30

    color: "white"
    // opacity: 1.

    // Enable pan, flick, and pinch gestures to zoom in and out
    gesture.accepted_gestures: MapGestureArea.PanGesture | MapGestureArea.FlickGesture | MapGestureArea.PinchGesture
    gesture.flick_deceleration: 3000
    gesture.enabled: true
    focus: true

    function enter_measure_state() {
        console.info("Enter Measure state")
        map.map_event_router.push_client("path-editor")
    }

    states: [
    State {
        name: "Measure"
        StateChangeScript {
            name: "enter_measure_state"
            script: enter_measure_state()
        }
    }
    ]

    Component.onCompleted: {
        map.plugin_layers('osm')[0].status = true
        set_scale()
    }

    center {
        // latitude: 0
        // longitude: 0
        // latitude: 44.6900
        // longitude: 6.1639
        latitude: 45.956
        longitude: 6.311
    }
    // zoomLevel: (maximum_zoom_level - minimum_zoom_level) / 2
    zoom_level: 10

    PositionSource {
        id: position_source
        active: position_locked

        onPositionChanged: {
            var coordinate = position_source.position.coordinate
            if (coordinate.isValid)
                center = map.cast_QGeoCoordinate(coordinate)
        }
    }

    onCenterChanged: {
        if (position_locked)
            if (center != map.cast_QGeoCoordinate(position_source.position.coordinate))
                position_locked = false
    }

    function set_scale() {
        var max_width = map.width
        if (max_width > 500)
            max_width = 2 / 3. * max_width
        else
            max_width = .9 * max_width
        var map_scale = map.make_scale(max_width)
        console.info(map_scale)
        scale_rule.width = map_scale.length_px
        var length_m = map_scale.length
        if (length_m >= 1000)
            scale_text.text = length_m / 1000 + " km"
        else
            scale_text.text = length_m + " m"
    }

    onZoom_levelChanged: {
        // if (position_locked) {
        //     center = map.cast_QGeoCoordinate(position_source.position.coordinate)
        // }
        set_scale()
    }

    onWidthChanged: {
        set_scale()
    }

    Rectangle {
        id: scale_rule
        y: map.y + .95 * map.height // Fixme: map.bottom
        anchors.horizontalCenter: map.horizontalCenter
        color: "black"
        height: 10
        width: 100
    }

    Text {
        id: scale_text
        anchors.bottom: scale_rule.top
        anchors.horizontalCenter: map.horizontalCenter
        font.pixelSize: 20
        font.bold: true
        text: ""
    }

    Keys.onPressed: {
        console.log(event)
        if (event.key == Qt.Key_Left) {
            pan(-100, 0) // width / 4
            event.accepted = true // ?
        } else if (event.key == Qt.Key_Right) {
            pan(100, 0)
            event.accepted = true
        } else if (event.key == Qt.Key_Up) {
            pan(0, -100) // height / 4
            event.accepted = true
        } else if (event.key == Qt.Key_Down) {
            pan(0, 100)
            event.accepted = true
        } else if (event.key == Qt.Key_Plus) {
            zoom_level++
            event.accepted = true
        } else if (event.key == Qt.Key_Minus) {
            zoom_level--
            event.accepted = true
        }
    }

    MouseArea {
        id: mouse_area
        anchors.fill: parent

        property variant last_coordinate

        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onPressed : {
            press_x = mouse.x
            press_y = mouse.y
            last_x = mouse.x
            last_y = mouse.y
            last_coordinate = to_coordinate(Qt.point(mouse.x, mouse.y))
            console.info("onPressed", last_coordinate)
        }

        onPositionChanged: {
            if (mouse.button == Qt.LeftButton) {
                last_x = mouse.x
                last_y = mouse.y
            }
        }

        onDoubleClicked: {
            console.info("onDoubleClicked")
            var position_px = Qt.point(mouse.x, mouse.y);
            var zoom_increment = 0;
            if (mouse.button === Qt.LeftButton)
                zoom_increment = 1;
            else if (mouse.button === Qt.RightButton)
                zoom_increment = -1;
            stable_zoom_by_increment(position_px, zoom_increment);

            last_x = -1;
            last_y = -1;
        }

        onPressAndHold:{
            // Check move distance is small enough
            if (Math.abs(press_x - mouse.x ) < jitter_threshold
            &&  Math.abs(press_y - mouse.y ) < jitter_threshold) {
                console.info("onPressAndHold", last_coordinate)
                // var map_event1 = new QcMapEvent(mouse.button, mouse.buttons, mouse.modifiers, mouse.wasHeld, last_coordinate)
                // console.info(map_event1)
                var map_event = map.map_event_router.create_map_event(mouse.button, mouse.buttons, mouse.modifiers, mouse.wasHeld, last_coordinate)
                console.info(map_event)
                map.map_event_router.handle_mouse_press_and_hold_event(map_event)
                map.update()
            }
        }
    }

    ToolBar {
        anchors.top: parent.top
        anchors.right: parent.right
        ColumnLayout {
            ToolButton {
                id: bearing_lock_icon
                // background: Rectangle {
                //     radius: 10
                //     color: "white"
                //     opacity: .5
                // }
                contentItem: Image {
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: bearing_locked ? "qrc:/icons/explore-black.png" : "qrc:/icons/explore-black.png"
                }
                onClicked: bearing_locked = !bearing_locked
            }
            ToolButton {
                id: position_lock_icon
                // background: Rectangle {
                //     radius: 10
                //     color: "white"
                //     // color: position_locked ? "green" : "white"
                //     opacity: .5
                // }
                contentItem: Image {
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: position_locked ? "qrc:/icons/gps-fixed-black.png" : "qrc:/icons/gps-not-fixed-black.png"
                }
                onClicked: position_locked = !position_locked
            }
            ToolButton {
                id: ruler_tool_icon
                checkable: true
                contentItem: Image {
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: "qrc:/icons/svg/ruler-black.png"
                }
                onClicked: {
                    map.state = "Measure"
                }
            }
        }
    }
}
