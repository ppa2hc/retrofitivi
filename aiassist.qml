import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Rectangle {
    color: "red"
    width: parent.width
    height: parent.height
    
    Rectangle {
        id: bg
        color: "#212121"
        anchors.fill: parent
    }

    Image {
        id: pendulum
        source: "resource/bot.png"  // Replace with your image path
        width: 700*2/3
        height: 825*2/3
        //anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        transformOrigin: Item.Center
        // Pendulum swing animation
        SequentialAnimation on rotation {
            loops: Animation.Infinite
            PropertyAnimation {
                from: -5
                to: 5
                duration: 1000
                easing.type: Easing.InOutQuad
            }
            PropertyAnimation {
                from: 5
                to: -5
                duration: 1000
                easing.type: Easing.InOutQuad
            }
        }
    }

    Rectangle {
        id: textContainer
        width: parent.width * 0.6
        height: 200
        x: 450
        //anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        //anchors.bottom: parent.bottom
        //anchors.bottomMargin: 20
        radius: 10
        color: "#333"
        border.color: "#777"
        border.width: 2
        opacity: 0.8

        TextInput {
            id: text2speechField
            width: parent.width - 20
            height: parent.height
            anchors.centerIn: parent
            font.pixelSize: 30
            font.bold: true
            text: textDisplay // Initial text, replace with your text
            color: "white"
            wrapMode: TextEdit.Wrap 
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
    }

    Timer {
        id: typingTimer
        interval: 50 // Adjust typing speed (in milliseconds)
        repeat: true
        onTriggered: {
            if (currentIndex < fullText.length) {
                textDisplay += fullText.charAt(currentIndex)
                currentIndex++
            } else {
                typingTimer.stop()
            }
        }
    }

    property string fullText: "Hi Stefan. Welcome to your digital automotive space. A custom coffee is being prepared. Fasten seat belt, relax, and enjoy your exceptional journey!"
    property string textDisplay: ""
    property int currentIndex: 0

    Component.onCompleted: {
        typingTimer.start()
    }
}