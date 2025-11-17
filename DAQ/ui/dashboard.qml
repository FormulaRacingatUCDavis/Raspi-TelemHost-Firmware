import QtQuick
import QtQuick.Window
// import Variables.Data

Window {
    id: mainpage
    visible: true

    // size of the dahsboard but unused as we set the visibility to full screen
    width: 800 
    height: 480
    // visibility: Window.FullScreen
    
    color: "black"

    /* to be implemented - need to load the correct page */
    Loader {
        id: pageLoader
        anchors.fill: parent
        source: "drive.qml"
        /*
        source: (Data.dashboardPage == 0) ? "drive.qml"
            : (Data.dashboardPage == 1) ? "debug.qml"
            : "practice.qml"
        */
    } 

    /*
    Loader {
        id: popuploader
        anchors.fill: parent
        source: "flashes.qml"
    }
    */
}