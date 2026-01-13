import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import VariablesData

Item {

    Timer {
        id: closetimer
        interval: 5000
        running: Data.whichPopupVisible != -1 ? true
            : false
        onTriggered: {
            Data.hidePopup()
        }
    }

    Rectangle {
        id: faultpopup
        visible: Data.whichPopupVisible == 0 ? true
                : false
        anchors.fill: parent

        color: (Data.vehiclestate == "UNCALIBRTD" || Data.vehiclestate == "BDSP TRIPD") ? mainpage.yellow : mainpage.red

        Text {
            id: faulttext
            color: "black"
            font.pixelSize: (Data.vehiclestate.length > 9) ? faultpopup.width * 0.1 : faultpopup.width * 0.15
            font.bold: true
            text: Data.vehiclestate
            onTextChanged: closetimer.restart()

            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.verticalCenter: parent.verticalCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    Rectangle {
        id: torquepopup
        visible: Data.whichPopupVisible == 1 ? true
                : false
        anchors.fill: parent

        GridLayout {
            id: torquebar
            anchors.fill: parent
            Layout.fillHeight: true
            Layout.fillWidth: true

            columns: 2
            columnSpacing: 0

            Rectangle {
                id: torquepercentagebox
                Layout.fillWidth: true
                Layout.preferredWidth: 0.4
                Layout.fillHeight: true
                color: mainpage.red

                Text {
                    id: torqueval
                    color: "black"
                    font.pixelSize: torquepopup.width * 0.15
                    font.bold: true
                    text: (Data.torquelimit == -1) ? "" : "" + Data.torquelimit + "%"
                    onTextChanged: closetimer.restart()

                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter
                    anchors.verticalCenter: parent.verticalCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Rectangle {
                id: torquebarbox
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 0.6

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0
                    
                    Rectangle {
                        id: torquemissingspace
                        color: "black"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredHeight: parent.height * (1 - (Data.torquelimit / 100.0))
                        Layout.minimumHeight: 0
                    }

                    Rectangle {
                        color: "white"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredHeight: parent.height * (Data.torquelimit / 100.0)
                        Layout.minimumHeight: 0
                    }
                }
            }
        }        
    }

    Rectangle {
        id: launchcontrolpopup
        visible: Data.whichPopupVisible == 2 ? true
                : false
        anchors.fill: parent

        GridLayout {
            id: launchcontrolbar
            anchors.fill: parent
            Layout.fillHeight: true
            Layout.fillWidth: true

            columns: 2
            columnSpacing: 0

            Rectangle {
                id: launchcontrolpercentagebox
                Layout.fillWidth: true
                Layout.preferredWidth: 0.4
                Layout.fillHeight: true
                color: mainpage.blue

                Text {
                    id: launchcontrolval
                    color: "black"
                    font.pixelSize: launchcontrolpopup.width * 0.15
                    font.bold: true
                    text: (Data.launchcontrol == -1) ? "" : Data.launchcontrol + "%"
                    onTextChanged: closetimer.restart()

                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter
                    anchors.verticalCenter: parent.verticalCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Rectangle {
                id: launchcontrolbarbox
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 0.6

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0
                    
                    Rectangle {
                        id: launchcontrolmissingspace
                        color: "black"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredHeight: parent.height * (1- (Data.launchcontrol / 100.0))
                        Layout.minimumHeight: 0
                    }

                    Rectangle {
                        color: "white"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredHeight: parent.height * (Data.launchcontrol / 100.0)
                        Layout.minimumHeight: 0
                    }
                }
            }
        }        
    }
}