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

    property real fullfontsize: mainwindow.width * 0.08
    property real smallerfontsize: mainwindow.width * 0.065
    property real titlefontsize: mainwindow.width * 0.04
    
    GridLayout {
        id: grid
        x: mainwindow.width / 30
        y: mainwindow.height * (3/32)
        width: mainwindow.width * (14/15)
        height: mainwindow.height * (13/15)
        columns: 2
        rows: 2
        columnSpacing: mainwindow.width / 39
        rowSpacing: grid.y

        Rectangle {
            id: speedbox
            color: (Data.overtake == 0) ? mainpage.blue : mainpage.orange
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 2
            Layout.preferredHeight: grid.height / 2
            border.weight: loggerindicator ? 2 : 0
            border.color: "#FFFFFF"

            Text {
                id: speedvalue
                text: (Data.mph == -1) ? "" : "" + Data.mph
                font.pixelSize: mainwindow.fullfontsize
                font.family: mainpage.typeface
                font.bold: true
                color: "black"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                id: speedtext
                text: "MPH"
                font.pixelSize: mainwindow.titlefontsize
                font.family: mainpage.typeface
                font.bold: true
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                anchors.bottom: parent.top
                y: grid.y
            }
        }

        ColumnLayout {
            id: stack
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 2
            Layout.preferredHeight: grid.height / 2
            spacing: grid.rowSpacing * 0.2

            Rectangle {
                id: chargebox
                color: mainpage.red
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: chargevalue
                    text: (Data.soc == -1) ? "" : Data.soc.toFixed(2) + "%"
                    font.pixelSize: mainwindow.smallerfontsize
                    font.family: mainpage.typeface
                    font.bold: true
                    color: "black"
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                Text {
                    id: chargetext
                    text: "PACK SOCIT"
                    font.pixelSize: mainwindow.titlefontsize
                    font.family: mainpage.typeface
                    font.bold: true
                    color: "yellow"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    horizontalAlignment: Text.AlignHCenter
                    anchors.bottom: parent.top
                    y: grid.y
                }
            }

            Rectangle {
                id: mctempbox
                color: Data.mctemp_color
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: mctemptext
                    text: (Data.mctemp == -1) ? "" : Data.mctemp + "C"
                    font.pixelSize: mainwindow.smallerfontsize
                    font.family: mainpage.typeface
                    font.bold: true
                    color: "black"
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        Rectangle {
            id: vehiclestatebox
            color: Data.vehiclestate_color
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 2
            Layout.preferredHeight: grid.height / 2

            Text {
                id: vehiclestateavalue
                text: Data.vehiclestate
                fontSizeMode: Text.Fit
                font.pixelSize: mainwindow.fullfontsize
                font.family: mainpage.typeface
                font.bold: true
                color: "black"
                anchors.fill: parent
                anchors.margin: parent.width * 0.5
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                id: vehiclestatetext
                text: "STATE"
                font.pixelSize: mainwindow.titlefontsize
                font.family: mainpage.typeface
                font.bold: true
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                anchors.bottom: parent.top
                y: grid.y
            }
        }

        Rectangle {
            id: glvvbox
            color: Data.glvv_color
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 2
            Layout.preferredHeight: grid.height / 2

            Text {
                id: glvvvalue
                text: (Data.glvv == -1) ? "" : "" + Data.glvv.toFixed(2)
                font.pixelSize: mainwindow.fullfontsize
                font.family: mainpage.typeface
                font.bold: true
                color: "black"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                id: glvvtext
                text: "GLV V"
                font.pixelSize: mainwindow.titlefontsize
                font.family: mainpage.typeface
                font.bold: true
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                anchors.bottom: parent.top
                y: grid.y
            }
        }
    }
}
