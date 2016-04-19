import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1

// import QtLocation 5.6
import QtPositioning 5.5

import QtCarto 1.0

ApplicationWindow {
    id: application_window
    visible: true
    width: 1280
    height: 800
    title: "QtCarto"
    // color: "black"

    Location {
        id: location

        coordinate {
            latitude: 44.6900
            longitude: 6.1639
        }
    }

    QcMapItem {
        id: map

        anchors.fill: parent
        focus: true
        // opacity: 1.

        center: location.coordinate
        zoom_level: 16

        Keys.onPressed: {
            if (event.key == Qt.Key_Left) {
                map.pan(-10, 0);
                event.accepted = true;
            } else if (event.key == Qt.Key_Right) {
                map.pan(10, 0);
                event.accepted = true;
            } else if (event.key == Qt.Key_Up) {
                map.pan(0, -10);
                event.accepted = true;
            } else if (event.key == Qt.Key_Down) {
                map.pan(0, 100);
                event.accepted = true;
            } else if (event.key == Qt.Key_Plus) {
                map.zoom_level += 1;
                event.accepted = true;
            } else if (event.key == Qt.Key_Minus) {
                map.zoom_level -= 1;
                event.accepted = true;
            }
        }
    }
}
