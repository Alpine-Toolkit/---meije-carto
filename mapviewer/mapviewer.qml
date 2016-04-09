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

    TestItem {
        anchors.fill: parent
        focus: true
        Keys.onPressed: {
            if (event.key == Qt.Key_Left) {
                console.info("move left");
                event.accepted = true;
            }
        }
    }
}
