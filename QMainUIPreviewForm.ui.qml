import QtQuick 2.4
import QtQuick.Controls 2.0

Item {
    width: 1000
    height: 600
    visible: true
    property alias item1: item1

    Image {
        id: image
        anchors.fill: parent
        source: "UIgrade.png"

        Item {
            id: item1
            width: 200
            opacity: 1
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
        }
    }
}
