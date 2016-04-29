import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1

// import QtLocation 5.6
import QtPositioning 5.5

import QtCarto 1.0 // fr.alpine_toolkit.

QcMapItem {
    id: map

    anchors.fill: parent

    property bool followme: false

    property int press_x : -1
    property int press_y : -1
    property int last_x : -1
    property int last_y : -1
    property int jitter_threshold : 30

    // Enable pan, flick, and pinch gestures to zoom in and out
    gesture.accepted_gestures: MapGestureArea.PanGesture | MapGestureArea.FlickGesture | MapGestureArea.PinchGesture
    gesture.flick_deceleration: 3000
    gesture.enabled: true
    focus: true

    // opacity: 1.

    center {
        latitude: 44.6900
        longitude: 6.1639
    }
    // zoomLevel: (maximum_zoom_level - minimum_zoom_level) / 2
    zoom_level: 10 // 16

    // PositionSource{
    //     id: position_source
    //     active: follow_me

    //     onPositionChanged: {
    //         map.center = position_source.position.coordinate
    //     }
    // }

    // onCenterChanged:{
    //     if (map.follow_me) {
    //         if (map.center != position_source.position.coordinate) {
    //             map.follow_me = false
    //         }
    //     }
    // }

    // onZoomLevelChanged:{
    //     if (map.followme) {
    //         map.center = position_source.position.coordinate
    //     }
    // }

    Keys.onPressed: {
        if (event.key == Qt.Key_Left) {
            map.pan(-100, 0) // map.width / 4
            event.accepted = true // ?
        } else if (event.key == Qt.Key_Right) {
            map.pan(100, 0)
            event.accepted = true
        } else if (event.key == Qt.Key_Up) {
            map.pan(0, -100) // map.height / 4
            event.accepted = true
        } else if (event.key == Qt.Key_Down) {
            map.pan(0, 100)
            event.accepted = true
        } else if (event.key == Qt.Key_Plus) {
            map.zoom_level++
            event.accepted = true
        } else if (event.key == Qt.Key_Minus) {
            map.zoom_level--
            event.accepted = true
        }
    }

    MouseArea {
        id: mouse_area
        anchors.fill: parent

        property variant last_coordinate

        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onPressed : {
            map.press_x = mouse.x
            map.press_y = mouse.y
            map.last_x = mouse.x
            map.last_y = mouse.y
            last_coordinate = map.to_coordinate(Qt.point(mouse.x, mouse.y))
            console.info("onPressed", last_coordinate)
        }

        onPositionChanged: {
            if (mouse.button == Qt.LeftButton) {
                map.last_x = mouse.x
                map.last_y = mouse.y
            }
        }

        onDoubleClicked: {
            console.info("onDoubleClicked")
            var position_px = Qt.point(mouse.x, mouse.y);
            var zoom_increment = 0;
            if (mouse.button === Qt.LeftButton) {
                zoom_increment = 1;
            } else if (mouse.button === Qt.RightButton) {
                zoom_increment = -1;
            }
            map.stable_zoom_by_increment(position_px, zoom_increment);

            last_x = -1;
            last_y = -1;
        }

        onPressAndHold:{
            // Check move distance is small enough
            if (Math.abs(map.press_x - mouse.x ) < map.jitter_threshold
            &&  Math.abs(map.press_y - mouse.y ) < map.jitter_threshold) {
                console.info("onPressAndHold", last_coordinate)
            }
        }
    }
}
