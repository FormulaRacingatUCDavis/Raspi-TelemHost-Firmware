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

    Loader {
        id: pageLoader
        anchors.fill: parent
        source: mainpage.pages[mainpage.currPage] //Data.dashboardpage
    } 

    /*
    Loader {
        id: popuploader
        anchors.fill: parent
        source: "flashes.qml"
    } 
    */
}