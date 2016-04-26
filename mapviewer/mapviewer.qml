import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1

ApplicationWindow {
    id: application_window
    visible: true
    width: 1280
    height: 800
    title: "QtCarto"

    MapComponent {
        anchors.fill: parent
        // color: "black"
    }
}
