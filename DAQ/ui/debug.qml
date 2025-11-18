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
        
        columns: 3
        rows: 3
        columnSpacing: mainwindow.width/38
        rowSpacing: grid.y

        uniformCellHeights: true
        uniformCellWidths: true

        property real titlefontsize: mainwindow.width/32
        property real valuefontsize: mainwindow.width * 0.04
        property string typeface: "Helvetica"

        
        Rectangle {
            id: packsocbox
            color: "red"
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            Text {
                id: packsocvalue
                color: "white"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: Data.soc
                font.pixelSize: grid.valuefontsize
                font.family: grid.typeface
            }
            
            Text {
                id: packsoctext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter 
                anchors.bottom: packsocbox.top
                y: grid.y
                text: "Pack SOC"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
            }
        }
        
        Rectangle {
            id: packtempbox
            color: "blue"
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            Text {
                id: packtempvalue
                color: "white"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: "temp"
                font.pixelSize: grid.valuefontsize
                font.family: grid.typeface
            }

            Text {
                id: packtemptext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter 
                anchors.bottom: parent.top
                y: grid.y
                text: "Pack Temperature"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
            }
        }

        Rectangle {
            id: shutdowncircuitbox
            color: "blue"
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            Text {
                id: shutdowncircuitvalue
                color: "white"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: "temp"
                font.pixelSize: grid.valuefontsize
                font.family: grid.typeface
            }

            Text {
                id: shutdowncircuittext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter 
                anchors.bottom: parent.top
                y: grid.y
                text: "Shutdown Circuit"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
            } 
        }

        Rectangle {
            id: mctempbox
            color: Data.mctemp_color
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            Text {
                id: mctempvalue
                color: "white"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: "" + Data.mctemp
                font.pixelSize: grid.valuefontsize
                font.family: grid.typeface
            }

            Text {
                id: mctemptext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter 
                anchors.bottom: parent.top
                y: grid.y
                text: "MC Temperature"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
            } 
        }

        Rectangle {
            id: motortempbox
            color: "blue"
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            Text {
                id: motortempvalue
                color: "white"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: "temp"
                font.pixelSize: grid.valuefontsize
                font.family: grid.typeface
            }

            Text {
                id: motortemptext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter 
                anchors.bottom: parent.top
                y: grid.y
                text: "Motor Temperature"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
            } 
        }

        Rectangle {
            id: mcstatebox
            color: "blue"
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            Text {
                id: mcstatevalue
                color: "white"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: "temp"
                font.pixelSize: grid.valuefontsize
                font.family: grid.typeface
            }

            Text {
                id: mcstatetext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter 
                anchors.bottom: parent.top
                y: grid.y
                text: "MC State"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
            }
        }

        Rectangle {
            id: vehiclestatebox
            color: Data.vehiclestate_color
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            Text {
                id: vehiclestatevalue
                color: "white"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: Data.vehiclestate
                font.pixelSize: grid.valuefontsize
                font.family: grid.typeface
            }

            Text {
                id: vehiclestatetext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter 
                anchors.bottom: parent.top
                y: grid.y
                text: "Vehicle State"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
            } 
        }

        Rectangle {
            id: glvvoltbox
            color: Data.glvv_color
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            Text {
                id: glvvvalue
                color: "white"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: "" + Data.glvv
                font.pixelSize: grid.valuefontsize
                font.family: grid.typeface
            }

            Text {
                id: glvvtext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter 
                anchors.bottom: parent.top
                y: grid.y
                text: "GLV Voltage"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
            } 
        }

        Rectangle {
            id: freebox
            color: "blue"
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            Text {
                id: freeboxvalue
                color: "white"
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: "temp"
                font.pixelSize: grid.valuefontsize
                font.family: grid.typeface
            }

            Text {
                id: freeboxtext
                color: "yellow"
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Text.AlignHCenter 
                anchors.bottom: parent.top
                y: grid.y
                text: "(free slot)"
                font.pixelSize: grid.titlefontsize
                font.family: grid.typeface
            } 
        }
    }
} 