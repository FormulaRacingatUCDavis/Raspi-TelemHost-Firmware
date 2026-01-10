import QtQuick
import QtQuick.Window
import VariablesData

Window {
    id: mainpage
    visible: true

    // size of the dashboard but will be unused if we set fullscreen
    width: 800 
    height: 480
    // need to implement when to be full screen
    // visibility: Window.FullScreen
    
    color: "black"

    property var pages: ["drive.qml", "debug.qml", "practice.qml"]
    property int currPage: 0
     MouseArea {
        anchors.fill: parent
        onClicked: {
            currPage = (currPage+1) % 3
        }
    }

    property string green: "#00FF00"
    property string yellow: "#FFFF00"
    property string red: "#FF4500"
    property string blue: "#0000FF"

    Loader {
        id: pageLoader
        anchors.fill: parent
        source: mainpage.pages[mainpage.currPage] 
        //source: Data.dashboardpage
    } 

    Loader {
        id: popuploader
        anchors.fill: parent
        source: "flashes.qml"
    } 
}