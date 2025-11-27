import QtQuick
import QtQuick.Layouts
import VariablesData


Rectangle {
    id: mainwindow
    visible: true
    anchors.fill: parent
    color: "black"

    // for debugging
    MouseArea {
        anchors.fill: parent
        onClicked: {
            mainpage.currPage = (mainpage.currPage+1) % 3
        }
    }

    GridLayout {
        id: grid

        x: mainwindow.width/30
        y: mainwindow.height * (3/40)
        width: mainwindow.width * (14/15)
        height: mainwindow.height * (43/48)

        columns: 4
        rows: 2
        columnSpacing: mainwindow.width/48
        rowSpacing: grid.y
        
        uniformCellHeights: true

        property string typeface: "Helvetica"
        property real fullfontsize: mainwindow.width * 0.075
        property real smallerfontsize: mainwindow.width * 0.06
        property real titlefontsize: mainwindow.width/34

        Rectangle {
            id: speedbox
            color: mainpage.blue
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 1

            Text {
                id: speedvalue
                text: (Data.mph == -1) ? "" : "" + Data.mph
                font.pixelSize: grid.fullfontsize
                font.family: grid.typeface
                color: "black" 
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter 
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                id: speedtext
                text: "MPH"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
                color: "yellow" 
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter 
                anchors.bottom: parent.top
                y: grid.y * 0
            }
        }

        Rectangle {
            id: accel
            color: "light blue"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 1

            Text {
                id: accelvalue
                text: "temp"
                font.pixelSize: grid.fullfontsize
                font.family: grid.typeface
                color: "black" 
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter 
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                id: acceltext
                text: "Acccleration"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
                color: "yellow" 
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter 
                anchors.bottom: parent.top
                y: grid.y * 0
            }
        }

        ColumnLayout {
            id: toprow
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: grid.rowSpacing * 0.75
            Layout.preferredWidth: 2

            Rectangle {
                id: chargebox
                color: mainpage.red
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                id: chargevalue
                text: (Data.soc == -1) ? "" : Data.soc + "%"
                font.pixelSize: grid.smallerfontsize
                font.family: grid.typeface
                color: "black" 
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter 
                verticalAlignment: Text.AlignVCenter
                }

                Text {
                    id: chargetext
                    text: "State of Charge"
                    font.pixelSize: grid.titlefontsize
                    font.family: grid.typeface
                    color: "yellow" 
                    anchors.left: parent.left
                    anchors.right: parent.right
                    horizontalAlignment: Text.AlignHCenter 
                    anchors.bottom: parent.top
                    y: grid.y * 0
                }
            }

            Rectangle {
                id: mctempbox
                color: Data.mctemp_color
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                id: mctempvalue
                text: (Data.mctemp == -1) ? "" : Data.mctemp + "C"
                font.pixelSize: grid.smallerfontsize
                font.family: grid.typeface
                color: "black" 
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter 
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                id: mctemptext
                text: "MC Temperature"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
                color: "yellow" 
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter 
                anchors.bottom: parent.top
                y: grid.y * 0
            }
            }
        }

        Rectangle {
            id: vechiclestatebox
            color: Data.vehiclestate_color
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 1

            Text {
                id: vehiclestatevalue
                text: Data.vehiclestate
                font.pixelSize: grid.fullfontsize
                font.family: grid.typeface
                color: "black" 
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter 
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                id: vehiclestatetext
                text: "Vehicle State"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
                color: "yellow" 
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter 
                anchors.bottom: parent.top
                y: grid.y * 0
            }
        }

        ColumnLayout {
            id: bottomrow
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: grid.rowSpacing * 0.75
            Layout.preferredWidth: 2

            Rectangle {
                id: glvvbox
                color: Data.glvv_color
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                id: glvvvalue
                text: (Data.glvv == -1) ? "" : "" + Data.glvv
                font.pixelSize: grid.smallerfontsize
                font.family: grid.typeface
                color: "white" 
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter 
                verticalAlignment: Text.AlignVCenter
                }

                Text {
                    id: glvvtext
                    text: "GLV Voltage"
                    font.pixelSize: grid.titlefontsize
                    font.family: grid.typeface
                    color: "yellow" 
                    anchors.left: parent.left
                    anchors.right: parent.right
                    horizontalAlignment: Text.AlignHCenter 
                    anchors.bottom: parent.top
                    y: grid.y * 0
                }
            }

            Rectangle {
                id: laptimebox
                color: "white"
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                id: laptimevalue
                text: "temp"
                font.pixelSize: grid.smallerfontsize
                font.family: grid.typeface
                color: "black" 
                anchors.fill:parent
                horizontalAlignment: Text.AlignHCenter 
                verticalAlignment: Text.AlignVCenter
                }

                Text {
                    id: laptimetext
                    text: "Lap Time"
                    font.pixelSize: grid.titlefontsize
                    font.family: grid.typeface
                    color: "yellow" 
                    anchors.left: parent.left
                    anchors.right: parent.right
                    horizontalAlignment: Text.AlignHCenter 
                    anchors.bottom: parent.top
                    y: grid.y * 0
                }
            }
        }
    } 
} 