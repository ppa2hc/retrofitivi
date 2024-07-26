import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import AiassistAsync 1.0

Rectangle {
    color: "red"
    width: parent.width
    height: parent.height
    
    Rectangle {
        id: bg
        color: "#212121"
        anchors.fill: parent
    }

    //property string fullText: "Hi Stefan. Welcome to your digital automotive space."
    property string fullText: ""
    property string textDisplay: ""
    property int currentIndex: 0

    //Component.onCompleted: {
    //    typingTimer.start()
    //}

    AiassistAsync {
        id: aiassistAsync

        onUpdateTextToSpeech: (msg)=> {
            console.log("tts: ", msg)
            fullText = msg
            typingTimer.start()
        }
    }

    property int iconSize: 100
    property int iconSpacing: 350

    // First logo
    Image {
        id: bgswlogo
        y: 10
        x: 20
        source: "resource/bgswlogo.png"
        width: 110
        height: 110
        fillMode: Image.PreserveAspectFit
    }
    // Second logo
    Image {
        id: digitalautologo
        y: -30
        x: iconSpacing
        //source: "resource/digitalautologo.jpeg" 
        source: "resource/logo2.png" 
        width: 200
        height: 200
        fillMode: Image.PreserveAspectFit
    }
    // Third logo
    Image {
        id: etaslogo
        y: -30
        x: iconSpacing*2
        //source: "resource/etaslogo.jpg" 
        source: "resource/logo3.png" 
        width: 200
        height: 200
        fillMode: Image.PreserveAspectFit
    }
    // Fourth logo
    Image {
        id: boschlogo
        y: -30
        x: iconSpacing*3
        //source: "resource/boschlogo.png"
        source: "resource/logo4.png" 
        width: 200
        height: 200
        fillMode: Image.PreserveAspectFit
    }


    Image {
        id: pendulum
        source: "resource/bot.png"
        width: 700*2/3
        height: 825*2/3
        anchors.horizontalCenter: parent.horizontalCenter
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

        MouseArea {
                anchors.fill: parent
                onClicked: {
                    //console.log("Logo 1 clicked")
                    //aiassistAsync.setTextToSpeech("Hello Stefan. Welcome to your digital automotive space. Your custom coffee is being prepared. Fasten seat belt, relax, and enjoy your exceptional journey")
                }
            }
    }

    Rectangle {
        id: textContainer
        width: parent.width * 0.6
        height: 50
        //x: 450
        y: pendulum.y + pendulum.height
        anchors.horizontalCenter: parent.horizontalCenter
        //anchors.verticalCenter: parent.verticalCenter
        //anchors.bottom: parent.bottom
        //anchors.bottomMargin: 20
        radius: 10
        color: "transparent"
        //color: "#333"
        //border.color: "#777"
        //border.width: 2
        //opacity: 0.8

        Text {
            id: text2speechField
            width: parent.width - 20
            height: parent.height
            anchors.centerIn: parent
            font.pixelSize: 30
            font.bold: true
            text: textDisplay
            color: "white"
            wrapMode: TextEdit.Wrap 
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            //lineHeight: 6
            lineHeight: 1.1
            lineHeightMode: Text.ProportionalHeight
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
                currentIndex++
                if (currentIndex > (fullText.length + 50)) {
                    typingTimer.stop()
                    textDisplay = ""
                    currentIndex = 0
                }                
            }
        }
    }

}