import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

ApplicationWindow {
    visible: true
    width: Screen.width
    height: Screen.height
    visibility: "FullScreen"
    flags: Qt.Window | Qt.FramelessWindowHint

    SwipeView {
        id: swipeView
        anchors.fill: parent

        Loader {
            source: "aiassist.qml"
        }
        Loader {
            source: "digitalauto.qml"
        }
    }
}
