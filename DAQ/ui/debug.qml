import QtQuick
import QtQuick.Layouts


Rectangle {
    id: mainwindow
    visible: true
    anchors.fill: parent
    color: "black" 

    GridLayout {
        id: grid 

        x: mainwindow.width/30
        y: mainwindow.height/30
        width: mainwindow.width * (14/15)
        height: mainwindow.height * (14/15)
        
        columns: 3
        rows: 3
        columnSpacing: mainwindow.width/38
        rowSpacing: mainwindow.height/18

        uniformCellHeights: true
        uniformCellWidths: true

        Rectangle {
            id: packsocbox
            color: "blue"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Rectangle {
            id: packtempbox
            color: "blue"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Rectangle {
            id: shutdowncircuitbox
            color: "blue"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Rectangle {
            id: mctempbox
            color: "blue"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Rectangle {
            id: motortempbox
            color: "blue"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Rectangle {
            id: mcstatebox
            color: "blue"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Rectangle {
            id: vehiclestatebox
            color: "blue"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Rectangle {
            id: glvvoltbox
            color: "blue"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Rectangle {
            id: freebox
            color: "green"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
} 