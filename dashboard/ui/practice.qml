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
        columns: 4
        rows: 2
        columnSpacing: mainwindow.width / 48
        rowSpacing: grid.y

        property string typeface: "Helvetica"
        property real fullfontsize: mainwindow.width * 0.075
        property real smallerfontsize: mainwindow.width * 0.06
        property real titlefontsize: mainwindow.width / 34

        Rectangle {
            id: speedbox
            color: mainpage.blue
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 4
            Layout.preferredHeight: grid.height / 2

            Text {
                id: speedvalue
                text: (Data.mph == -1) ? "" : "" + Data.mph
                font.pixelSize: grid.fullfontsize
                font.family: grid.typeface
                font.bold: true
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
                font.bold: true
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                anchors.bottom: parent.top
                y: 0
            }

        }

        Rectangle {
            id: mctempbox
            color: Data.mctemp_color
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 4
            Layout.preferredHeight: grid.height / 2

            Text {
                id: mctempvalue
                text: (Data.mctemp == -1) ? "" : Data.mctemp + "C"
                font.pixelSize: grid.fullfontsize
                font.family: grid.typeface
                font.bold: true
                color: "black"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                id: mctemptext
                text: "MC TEMP"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
                font.bold: true
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter
                anchors.bottom: parent.top
                y: 0
            }

        }

        ColumnLayout {
            id: toprow
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 2
            Layout.preferredHeight: grid.height / 2
            spacing: grid.rowSpacing * 0.75

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
                    font.bold: true
                    color: "black"
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                Text {
                    id: chargetext
                    text: "PACK SOCIT"
                    font.pixelSize: grid.titlefontsize
                    font.family: grid.typeface
                    font.bold: true
                    color: "yellow"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    horizontalAlignment: Text.AlignHCenter
                    anchors.bottom: parent.top
                    y: 0
                }

            }

            Rectangle {
                id: best_laptimebox
                color: (Data.best_lap_time == 0.0) ? "white" : mainpage.green
                // white until record get a best lap time
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: best_laptimevalue
                    text: "" // (Data.best_lap_time == 0.0) ? "" : "" + Data.best_lap_time
                    font.pixelSize: grid.smallerfontsize
                    font.family: grid.typeface
                    font.bold: true
                    color: "black"
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                Text {
                    id: best_laptimetext
                    text: "BEST LAP TIME"
                    font.pixelSize: grid.titlefontsize
                    font.family: grid.typeface
                    font.bold: true
                    color: "yellow"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    horizontalAlignment: Text.AlignHCenter
                    anchors.bottom: parent.top
                    y: 0
                }

            }
        }

        ColumnLayout {
            id: left_bottomrow
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 2
            Layout.preferredHeight: grid.height / 2
            spacing: grid.rowSpacing * 0.75

            Rectangle {
                id: vechiclestatebox
                color: Data.vehiclestate_color
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: vehiclestatevalue
                    text: Data.vehiclestate
                    font.pixelSize: grid.smallerfontsize
                    font.family: grid.typeface
                    font.bold: true
                    color: "black"
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                Text {
                    id: vehiclestatetext
                    text: "STATE"
                    font.pixelSize: grid.titlefontsize
                    font.family: grid.typeface
                    font.bold: true
                    color: "yellow"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    horizontalAlignment: Text.AlignHCenter
                    anchors.bottom: parent.top
                    y: 0
                }

            }

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
                    font.bold: true
                    color: "black"
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                Text {
                    id: glvvtext
                    text: "GLV V"
                    font.pixelSize: grid.titlefontsize
                    font.family: grid.typeface
                    font.bold: true
                    color: "yellow"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    horizontalAlignment: Text.AlignHCenter
                    anchors.bottom: parent.top
                    y: 0
                }

            }
        }

        ColumnLayout {
            id: right_bottomrow
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: grid.width / 2
            Layout.preferredHeight: grid.height / 2
            spacing: grid.rowSpacing * 0.75

            Rectangle {
                id: prev_laptimebox
                color: {
                    if (Data.prev_lap_time === 0.0)                   return "white";
                    if (Data.prev_lap_time === Data.best_lap_time)    return mainpage.green;
                    if (Data.best_lap_time - Data.prev_lap_time <= 1) return mainpage.yellow;
                    return mainpage.red;
                }
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: prev_laptimevalue
                    text: "" // (Data.prev_lap_time == 0.0) ? "" : "" + Data.prev_lap_time
                    font.pixelSize: grid.smallerfontsize
                    font.family: grid.typeface
                    font.bold: true
                    color: "black"
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                Text {
                    id: prev_laptimetext
                    text: "PREVIOUS LAP TIME"
                    font.pixelSize: grid.titlefontsize
                    font.family: grid.typeface
                    font.bold: true
                    color: "yellow"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    horizontalAlignment: Text.AlignHCenter
                    anchors.bottom: parent.top
                    y: 0
                }

            }

            Rectangle {
                id: curr_laptimebox
                color: "white"
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: curr_laptimevalue
                    text: "" // Data.curr_lap_time
                    font.pixelSize: grid.smallerfontsize
                    font.family: grid.typeface
                    font.bold: true
                    color: "black"
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                Text {
                    id: curr_laptimetext
                    text: "LAP TIME"
                    font.pixelSize: grid.titlefontsize
                    font.family: grid.typeface
                    font.bold: true
                    color: "yellow"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    horizontalAlignment: Text.AlignHCenter
                    anchors.bottom: parent.top
                    y: 0
                }

            }
        }
    }
}