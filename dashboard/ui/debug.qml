import QtQuick
import QtQuick.Layouts
import VariablesData

Rectangle {
    id: mainwindow
    visible: true
    anchors.fill: parent
    color: "black"

    MouseArea {
        anchors.fill: parent
        onClicked: {
            mainpage.currPage = (mainpage.currPage+1) % 3
        }
    }

    GridLayout {
        id: grid
        x: mainwindow.width / 30
        y: mainwindow.height * (3/40)
        width: mainwindow.width * (14/15)
        height: mainwindow.height * (43/48)
        columns: 3
        rows: 3
        columnSpacing: mainwindow.width / 38
        rowSpacing: grid.y

        property real titlefontsize: mainwindow.width / 32
        property real valuefontsize: mainwindow.width * 0.04
        property string typeface: "Helvetica"

        Rectangle {
            id: packsocbox
            color: mainpage.red
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 3
            Layout.preferredHeight: grid.height / 3

            Text {
                id: packsocvalue
                color: "black"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: (Data.soc == -1) ? "" : Data.soc + "%"
                font.pixelSize: grid.valuefontsize
                font.family: grid.typeface
                font.bold: true
            }

            Text {
                id: packsoctext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                anchors.bottom: packsocbox.top
                y: grid.y
                text: "PACK SOC"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
                font.bold: true
            }
        }

        Rectangle {
            id: packtempbox
            color: "white"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 3
            Layout.preferredHeight: grid.height / 3

            Text {
                id: packtempvalue
                color: "black"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: (Data.packtemp == -1) ? "" : Data.packtemp + "C"
                font.pixelSize: grid.valuefontsize
                font.family: grid.typeface
                font.bold: true
            }

            Text {
                id: packtemptext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                anchors.bottom: parent.top
                y: grid.y
                text: "PACK TEMP"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
                font.bold: true
            }
        }

        Rectangle {
            id: shutdowncircuitbox
            color: "white"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 3
            Layout.preferredHeight: grid.height / 3

            Text {
                id: shutdowncircuitvalue
                color: "black"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: Data.shutdowncircuit
                font.pixelSize: (Data.shutdowncircuit.length < 11) ? grid.valuefontsize : grid.valuefontsize * 0.7
                font.family: grid.typeface
                font.bold: true
            }

            Text {
                id: shutdowncircuittext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                anchors.bottom: parent.top
                y: grid.y
                text: "SHUTDOWN CIRCUIT"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
                font.bold: true
            }
        }

        Rectangle {
            id: mctempbox
            color: Data.mctemp_color
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 3
            Layout.preferredHeight: grid.height / 3

            Text {
                id: mctempvalue
                color: "black"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: (Data.mctemp == -1) ? "" : Data.mctemp + "C"
                font.pixelSize: grid.valuefontsize
                font.family: grid.typeface
                font.bold: true
            }

            Text {
                id: mctemptext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                anchors.bottom: parent.top
                y: grid.y
                text: "MC TEMP"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
                font.bold: true
            }
        }

        Rectangle {
            id: motortempbox
            color: "white"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 3
            Layout.preferredHeight: grid.height / 3

            Text {
                id: motortempvalue
                color: "black"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: (Data.motortemp == -1) ? "" : Data.motortemp + "C"
                font.pixelSize: grid.valuefontsize
                font.family: grid.typeface
                font.bold: true
            }

            Text {
                id: motortemptext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                anchors.bottom: parent.top
                y: grid.y
                text: "MOTOR TEMP"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
                font.bold: true
            }
        }

        Rectangle {
            id: mcstatebox
            color: "white"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 3
            Layout.preferredHeight: grid.height / 3

            Text {
                id: mcstatevalue
                color: "black"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: Data.mcfault
                font.pixelSize: grid.valuefontsize * 0.45
                font.family: grid.typeface
                font.bold: true
            }

            Text {
                id: mcstatetext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                anchors.bottom: parent.top
                y: grid.y
                text: "MC STATE"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
                font.bold: true
            }
        }

        Rectangle {
            id: vehiclestatebox
            color: Data.vehiclestate_color
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 3
            Layout.preferredHeight: grid.height / 3

            Text {
                id: vehiclestatevalue
                color: "black"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: Data.vehiclestate
                font.pixelSize: (Data.vehiclestate.length < 10) ? grid.valuefontsize : grid.valuefontsize * 0.8
                font.family: grid.typeface
                font.bold: true
            }

            Text {
                id: vehiclestatetext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                anchors.bottom: parent.top
                y: grid.y
                text: "STATE"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
                font.bold: true
            }
        }

        Rectangle {
            id: glvvoltbox
            color: Data.glvv_color
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 3
            Layout.preferredHeight: grid.height / 3

            Text {
                id: glvvvalue
                color: "black"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: (Data.glvv == -1) ? "" : "" + Data.glvv
                font.pixelSize: grid.valuefontsize
                font.family: grid.typeface
                font.bold: true
            }

            Text {
                id: glvvtext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                anchors.bottom: parent.top
                y: grid.y
                text: "GLV V"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
                font.bold: true
            }
        }

        Rectangle {
            id: powerbox
            color: "white"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 3
            Layout.preferredHeight: grid.height / 3

            Text {
                id: powervalue
                color: "black"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: (Data.accum_power == -1) ? "" : "" + Data.accum_power
                font.pixelSize: grid.valuefontsize
                font.family: grid.typeface
                font.bold: true
            }

            Text {
                id: powertext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                anchors.bottom: parent.top
                y: grid.y
                text: "POWER"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
                font.bold: true
            }
        }
    }
}