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
    title: "DeepSeek"

    Client {
        programmerName: "DeepSeek"
        z: 999
    }

    property bool waiting: false

    ListModel {
        id: chatModel
    }

    // ─── 對話框（需求 3/4/5）───
    ListView {
        id: chatList
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: inputBar.top
        anchors.bottomMargin: 10
        model: chatModel
        clip: true
        spacing: 10
        ScrollBar.vertical: ScrollBar { visible: chatList.contentHeight > chatList.height }
        onCountChanged: Qt.callLater(chatList.positionViewAtEnd)

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
                    font.pixelSize: 16
                }
            }
        }
    }

    // ─── 輸入區（需求 6）───
    Rectangle {
        id: inputBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: inputPanel.visible ? inputPanel.top : parent.bottom
        anchors.bottomMargin: inputPanel.visible ? 0 : 10
        height: 90
        color: "#40414f"

        TextArea {
            id: inputField
            anchors.left: parent.left
            anchors.right: enterBtn.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.leftMargin: 12
            anchors.rightMargin: 8
            anchors.topMargin: 12
            anchors.bottomMargin: 12
            color: "white"
            placeholderText: "Message DeepSeek..."
            placeholderTextColor: "#8e8ea0"
            font.pixelSize: 18
            wrapMode: TextEdit.Wrap
            ScrollBar.vertical: ScrollBar { visible: inputField.contentHeight > inputField.height }
        }

        Button {
            id: enterBtn
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 12
            width: 80
            height: 48
            text: "Enter"
            enabled: !waiting && inputField.text.trim().length > 0
            onClicked: send()
        }
    }

    // ─── 虛擬鍵盤（需求 2）───
    InputPanel {
        id: inputPanel
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        visible: Qt.inputMethod.visible
    }

    function send() {
        var text = inputField.text.trim()
        if (text.length === 0) return
        chatModel.append({ role: "user", text: text })
        inputField.text = ""
        waiting = true
        chatClient.sendMessage(text)
    }

    Connections {
        target: chatClient
        onReplyReceived: {
            chatModel.append({ role: "assistant", text: reply })
            waiting = false
        }
        onReplyError: {
            chatModel.append({ role: "assistant", text: "[Error] " + error })
            waiting = false
        }
    }
}
