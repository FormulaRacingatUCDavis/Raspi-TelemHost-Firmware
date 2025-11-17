import QtQuick
import QtQuick.Window
import QtQuick.Layouts

Item {

    /* to be implemented to automatically close the popup
    Timer {
        id: closetimer
        interval: 5000
        running: Data.whichPopupVisible != -1 ? true
            : false
        onTriggered: {
            Data.hidePopup()
        }
    }
    */

    Rectangle {
        id: faultpopup
        /* tp be implemented
        visible: Data.whichPopupVisible == 0 ? true
                : false
        */
        anchors.fill: parent

        color: "red"

        Text {
            id: faulttext
            color: "white"
            font.pixelSize: faultpopup.width * 0.1
            text: "fault"

            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.verticalCenter: parent.verticalCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    Rectangle {
        id: torque1popup
        /* to be implemented
        visible: Data.whichPopupVisible == 1 ? true
                : false
        */
        anchors.fill: parent

        GridLayout {
            id: torque1bar
            anchors.fill: parent
            Layout.fillHeight: true
            Layout.fillWidth: true

            columns: 2
            columnSpacing: 0

            Rectangle {
                id: torque1percentagebox
                Layout.fillWidth: true
                Layout.preferredWidth: 0.4
                Layout.fillHeight: true
                color: "green"

                Text {
                    id: torque1text
                    color: "black"
                    font.pixelSize: torque1popup.width * 0.1
                    text: "torque"

                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter
                    anchors.verticalCenter: parent.verticalCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Rectangle {
                id: torque1barbox
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 0.6

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0
                    
                    Rectangle {
                        id: torque1missingspace
                        color: "blue"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredHeight: parent.height * 1.0
                        Layout.minimumHeight: 0
                    }

                    Rectangle {
                        color: "red"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredHeight: parent.height * 0.0
                        Layout.minimumHeight: 0
                    }
                }
            }
        }        
    }

    Rectangle {
        id: torque2popup
        /* 
        visible: Data.whichPopupVisible == 2 ? true
                : false
        */
        anchors.fill: parent

        GridLayout {
            id: torque2bar
            anchors.fill: parent
            Layout.fillHeight: true
            Layout.fillWidth: true

            columns: 2
            columnSpacing: 0

            Rectangle {
                id: torque2percentagebox
                Layout.fillWidth: true
                Layout.preferredWidth: 0.4
                Layout.fillHeight: true
                color: "green"

                Text {
                    id: torque2text
                    color: "black"
                    font.pixelSize: torque1popup.width * 0.1
                    text: "torque"

                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter
                    anchors.verticalCenter: parent.verticalCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Rectangle {
                id: torque2barbox
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 0.6

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0
                    
                    Rectangle {
                        id: torque2missingspace
                        color: "blue"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredHeight: parent.height * 1.0
                        Layout.minimumHeight: 0
                    }

                    Rectangle {
                        color: "red"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredHeight: parent.height * 0.0
                        Layout.minimumHeight: 0
                    }
                }
            }
        }        
    }
}