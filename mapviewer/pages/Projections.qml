import QtQml 2.2
import QtQuick 2.6
import QtQuick.Window 2.2

import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0

Flickable {
    contentHeight: projection_pane.height

    Pane {
        id: projection_pane
        width: application_window.width / 2
        height: Math.max(list_view.height, application_window.height)

        ListView {
            id: list_view
            anchors.fill: parent // Fixme: ok ?
            model: map.projections
            delegate: Text {
                width: parent.width
                font.pixelSize: 16
                text: modelData
            }

            section.criteria: ViewSection.FirstCharacter
        }
    }

    ScrollIndicator.vertical: ScrollIndicator {}
}
