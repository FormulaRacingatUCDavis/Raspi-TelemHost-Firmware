import QtQuick
import QtQuick.Layouts


Rectangle {
    id: mainwindow
    visible: true
    anchors.fill: parent
    color: "black"

    GridLayout {
        id: grid

        columns: 4
        rows: 2
        columnSpacing: mainwindow.width/48
        rowSpacing: mainwindow.height/20

        x: mainwindow.width/30
        y: mainwindow.height/30
        width: mainwindow.width * (14/15)
        height: mainwindow.height * (14/15)
        
        uniformCellHeights: true

        Rectangle {
            id: speedbox
            color: "blue"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 1
        }

        Rectangle {
            id: accel
            color: "blue"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 1
        }

        ColumnLayout {
            id: toprow
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: grid.rowSpacing * 0.75
            Layout.preferredWidth: 2

            Rectangle {
                id: chargestatebox
                color: "green"
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Rectangle {
                id: mctempbox
                color: "green"
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }

        Rectangle {
            id: vechiclestatebox
            color: "red"
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 1
        }

        ColumnLayout {
            id: bottomrow
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: grid.rowSpacing * 0.5
            Layout.preferredWidth: 2

            Rectangle {
                id: glvvoltbox
                color: "gray"
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Rectangle {
                id: laptimebox
                color: "gray"
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    } 
} 