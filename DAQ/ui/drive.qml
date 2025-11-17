import QtQuick
import QtQuick.Layouts
import VariablesData 


Rectangle {
    id: mainwindow
    visible: true
    anchors.fill: parent
    color: "black"

    property string typeface: "Helvetica"
    property real fullfontsize: mainwindow.width * 0.1
    property real smallerfontsize: mainwindow.width * 0.08
    property real titlefontsize: mainwindow.width * 0.04

    property int borderwidth: 5
    
    GridLayout {
        id: grid

        x: mainwindow.width/30
        y: 45 // mainwindow.height/15
        width: mainwindow.width * (14/15)
        height: mainwindow.height * (13/15)

        columns: 2
        rows: 2
        columnSpacing: mainwindow.width/38
        rowSpacing: 48 // mainwindow.height/15

        uniformCellWidths: true
        uniformCellHeights: true

        Rectangle {
            id: speedbox
            color: "blue"
            Layout.fillWidth: true
            Layout.fillHeight: true

            Text {
                id: speedvalue
                text: "" + Data.mph
                font.pixelSize: mainwindow.fullfontsize
                font.family: mainwindow.typeface
                color: "white" 
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter 
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                id: speedtext
                text: "MPH"
                font.pixelSize: mainwindow.titlefontsize
                font.family: mainwindow.typeface
                color: "yellow" 
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter 
                anchors.bottom: speedbox.top
                y: 45
            }
        }

        ColumnLayout {
            id: stack
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: grid.rowSpacing * 0.2

            Rectangle {
                id: chargebox
                color: "green"
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: chargevalue
                    text: "charge"
                    font.pixelSize: mainwindow.smallerfontsize
                    font.family: mainwindow.typeface
                    color: "white" 
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter 
                    verticalAlignment: Text.AlignVCenter
                }

                Text {
                    id: chargetext
                    text: "PACK SOCIT"
                    font.pixelSize: mainwindow.titlefontsize
                    font.family: mainwindow.typeface
                    color: "yellow" 
                    anchors.left: parent.left
                    anchors.right: parent.right
                    horizontalAlignment: Text.AlignHCenter 
                    anchors.bottom: chargebox.top
                    y: 45
                    }
            }

            Rectangle {
                id: mctempbox
                color: "green"
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: temptext
                    text: "temp" + "C"
                    font.pixelSize: mainwindow.smallerfontsize
                    font.family: mainwindow.typeface
                    color: "white"
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter 
                    verticalAlignment: Text.AlignVCenter
                } 
            }
        }

        Rectangle {
            id: vehiclestatebox
            color: "green"
            Layout.fillWidth: true
            Layout.fillHeight: true
        
            Text {
                id: vehiclestateavalue
                text: "" + Data.vehiclestate
                font.pixelSize: mainwindow.fullfontsize
                font.family: mainwindow.typeface
                color: "white" 
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter 
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                id: vehiclestatetext
                text: "STATE:"
                font.pixelSize: mainwindow.titlefontsize
                font.family: mainwindow.typeface
                color: "yellow" 
                horizontalAlignment: Text.AlignHCenter
                anchors.bottom: vehiclestatebox.top
                y: 45
            }
        }

        Rectangle {
            id: glvvoltbox
            color: "green"
            Layout.fillWidth: true
            Layout.fillHeight: true

            Text {
                id: glvvoltagevalue
                text: "volt"
                font.pixelSize: mainwindow.fullfontsize
                font.family: mainwindow.typeface
                color: "white" 
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter 
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                id: glvvoltagetext
                text: "GLV V"
                font.pixelSize: mainwindow.titlefontsize
                font.family: mainwindow.typeface
                color: "yellow" 
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter 
                anchors.bottom: glvvoltbox.top
                y: 45
            }
        }
    }
}
