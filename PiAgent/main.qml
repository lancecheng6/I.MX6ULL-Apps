import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.VirtualKeyboard 2.4
import com.alientek.qmlcomponents 1.0

Window {
    visible: true
    width: Screen.desktopAvailableWidth
    height: Screen.desktopAvailableHeight
    x: 0
    y: 0
    color: "#343541"
    title: "PiAgent"

    Client {
        programmerName: "PiAgent"
        z: 999
    }

    property bool waiting: false
    property int assistantIndex: -1

    ListModel {
        id: chatModel
    }

    ListView {
        id: chatList
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: inputBar.top
        anchors.bottomMargin: 24
        model: chatModel
        clip: true
        spacing: 10
        cacheBuffer: chatList.height * 3
        ScrollBar.vertical: ScrollBar { visible: chatList.contentHeight > chatList.height }
        footer: Item {
            width: chatList.width
            height: 24
        }

        property bool userScrolling: false
        onDragStarted: userScrolling = true
        onDragEnded: userScrolling = false
        onFlickStarted: userScrolling = true
        onFlickEnded: userScrolling = false

        function isNearBottom() {
            return (chatList.contentY + chatList.height >= chatList.contentHeight - 50)
        }
        onCountChanged: {
            if (!userScrolling)
                Qt.callLater(chatList.positionViewAtEnd)
        }
        onContentHeightChanged: {
            if (waiting && !userScrolling)
                Qt.callLater(chatList.positionViewAtEnd)
        }

        delegate: Item {
            width: chatList.width
            height: textContent.implicitHeight + 32
            Rectangle {
                id: bubble
                anchors.right: model.role === "user" ? parent.right : undefined
                anchors.left: model.role === "user" ? undefined : parent.left
                anchors.rightMargin: model.role === "user" ? 12 : 0
                anchors.leftMargin: model.role === "user" ? 0 : 12
                width: Math.min(textContent.implicitWidth + 24, parent.width * 0.75)
                height: textContent.implicitHeight + 24
                color: model.role === "user" ? "#10a37f" : "#444654"
                radius: 8
                Text {
                    id: textContent
                    anchors.margins: 12
                    anchors.fill: parent
                    text: model.text
                    color: "white"
                    wrapMode: Text.Wrap
                    font.pixelSize: 18
                }
            }
        }
    }

    Row {
        id: inputBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: inputPanel.visible ? inputPanel.top : parent.bottom
        anchors.margins: 10
        height: 48
        spacing: 10

        Rectangle {
            id: inputBox
            width: parent.width - sendButton.width - parent.spacing
            height: parent.height
            color: "#40414f"
            radius: 8
            TextInput {
                id: input
                anchors.fill: parent
                anchors.margins: 12
                color: "white"
                font.pixelSize: 18
                clip: true
                verticalAlignment: Text.AlignVCenter
                onAccepted: send()
            }
        }

        Button {
            id: sendButton
            width: 80
            height: parent.height
            text: "Send"
            enabled: !waiting && input.text.trim().length > 0
            onClicked: send()
        }
    }

    InputPanel {
        id: inputPanel
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        visible: Qt.inputMethod.visible
    }

    Text {
        id: statusText
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: inputBar.top
        anchors.bottomMargin: 6
        color: "#8e8ea0"
        font.pixelSize: 14
        text: waiting ? "Pi agent working..." : "Pi agent ready"
    }

    function send() {
        var t = input.text.trim()
        if (t === "") return
        chatModel.append({ role: "user", text: t })
        chatModel.append({ role: "assistant", text: "" })
        assistantIndex = chatModel.count - 1
        input.text = ""
        waiting = true
        Qt.callLater(chatList.positionViewAtEnd)
        piClient.sendMessage(t)
    }

    Connections {
        target: piClient
        onPartialUpdate: {
            if (assistantIndex >= 0)
                chatModel.set(assistantIndex, { role: "assistant", text: text })
        }
        onReplyReceived: {
            if (assistantIndex >= 0)
                chatModel.set(assistantIndex, { role: "assistant", text: reply })
            waiting = false
            assistantIndex = -1
        }
        onReplyError: {
            chatModel.append({ role: "assistant", text: "Error: " + error })
            waiting = false
            assistantIndex = -1
        }
    }

    Component.onCompleted: piClient.startAgent()

    Component.onDestruction: piClient.stopAgent()
}
