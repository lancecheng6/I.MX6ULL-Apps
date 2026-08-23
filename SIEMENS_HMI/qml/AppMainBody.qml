import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: appMainBody
    anchors.fill: parent

    Connections {
        target: plc
        function onConnectedChanged() { statusLabel.text = plc.connected ? "Connected" : "Disconnected" }
    }

    Flickable {
        anchors.fill: parent
        contentHeight: contentCol.implicitHeight + 20
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        ColumnLayout {
            id: contentCol
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            anchors.topMargin: 8
            spacing: 5

            // Header
            RowLayout {
                Text { text: "S7-200 SMART(SR30) HMI"; color: "cyan"; font.pixelSize: 20; font.bold: true }
                Item { Layout.fillWidth: true }
                Text { id: statusLabel; text: plc.connected ? "Connected" : "Disconnected"; color: plc.connected ? "#00ff00" : "gray"; font.pixelSize: 12 }
            }

            // Connection bar
            RowLayout {
                spacing: 6
                TextField {
                    id: ipField
                    text: plc.plcIp()
                    color: "white"
                    implicitWidth: 130
                    font.pixelSize: 12
                    background: Rectangle { color: "#222"; radius: 4 }
                }
                Button {
                    text: plc.connected ? "Disconnect" : "Connect"
                    font.pixelSize: 12
                    onClicked: plc.connected ? plc.disconnectPlc() : plc.connectToPlc(ipField.text)
                }
            }

            // System status
            Rectangle {
                Layout.fillWidth: true; height: 30; radius: 4
                color: (plc.connected && plc.systemState) ? "#1a4d1a" : "#331a1a"
                RowLayout {
                    anchors.centerIn: parent
                    Text { text: "System: "; color: "white"; font.pixelSize: 12 }
                    Rectangle { width: 12; height: 12; radius: 6; color: (plc.connected && plc.systemState) ? "#00ff00" : "#ff0000" }
                    Text { text: (plc.connected && plc.systemState) ? "RUNNING" : "STOPPED"; color: "white"; font.pixelSize: 12; font.bold: true }
                }
            }

            // Sensor data - 2 rows of 4 (values freeze on disconnect)
            GridLayout {
                Layout.fillWidth: true
                columns: 4
                columnSpacing: 6
                rowSpacing: 3

                Repeater {
                    model: [
                        { label: "P.In",  value: plc.pressureIn.toFixed(1) + "B" },
                        { label: "P.Out", value: plc.pressureOut.toFixed(1) + "B" },
                        { label: "T.In",  value: plc.tempIn.toFixed(0) + "\u00B0C" },
                        { label: "T.Out", value: plc.tempOut.toFixed(0) + "\u00B0C" },
                        { label: "B.Prss", value: plc.pressureBoiler.toFixed(1) + "B" },
                        { label: "B.Lvl", value: plc.levelBoiler.toFixed(0) + "%" },
                        { label: "B.Temp", value: plc.tempBoiler.toFixed(0) + "\u00B0C" },
                        { label: "T.Lvl", value: plc.levelTank.toFixed(0) + "%" }
                    ]
                    delegate: Rectangle {
                        Layout.fillWidth: true
                        height: 34; radius: 3; color: "#1a1a2e"
                        border.color: "#333"; border.width: 1
                        RowLayout {
                            anchors.left: parent.left; anchors.leftMargin: 6
                            anchors.verticalCenter: parent.verticalCenter
                            Text { text: modelData.label + ":"; color: "#aaa"; font.pixelSize: 10 }
                            Text { text: modelData.value; color: "white"; font.pixelSize: 12; font.bold: true }
                        }
                    }
                }
            }

            // Separator
            Rectangle { Layout.fillWidth: true; height: 1; color: "#333" }

            // Control section
            Text { text: "Control"; color: "cyan"; font.pixelSize: 13; font.bold: true }

            // Pump controls - disabled when not connected
            GridLayout {
                Layout.fillWidth: true
                columns: 3
                columnSpacing: 6
                rowSpacing: 4
                enabled: plc.connected

                Repeater {
                    model: [
                        { name: "Pump1",    state: plc.feedPump },
                        { name: "Pump2",     state: plc.addPump },
                        { name: "Pump3",     state: plc.airPump },
                        { name: "Pump4",  state: plc.circlePump },
                        { name: "Valve",   state: plc.outValve }
                    ]
                    delegate: Rectangle {
                        Layout.fillWidth: true
                        height: 36; radius: 4
                        color: "#1a1a2e"
                        border.color: "#333"
                        border.width: 1
                        opacity: plc.connected ? 1.0 : 0.5

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            anchors.rightMargin: 8
                            spacing: 6

                            // Indicator light: active(red), idle/disconnected(green)
                            Rectangle {
                                width: 8; height: 8; radius: 4
                                color: (plc.connected && modelData.state) ? "#e74c3c" : "#2ecc71"
                                border.color: (plc.connected && modelData.state) ? "#c0392b" : "#27ae60"
                                border.width: 1
                                Layout.alignment: Qt.AlignVCenter
                            }
                            Text {
                                text: modelData.name
                                color: plc.connected ? "white" : "#888888"
                                font.pixelSize: 11
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                                verticalAlignment: Text.AlignVCenter
                            }
                            Switch {
                                checked: plc.connected && modelData.state
                                enabled: plc.connected
                                onToggled: {
                                    console.log("QML:", modelData.name, "->", checked)
                                    switch(index) {
                                    case 0: plc.feedPumpControl(checked); break;
                                    case 1: plc.addPumpControl(checked); break;
                                    case 2: plc.airPumpControl(checked); break;
                                    case 3: plc.circlePumpControl(checked); break;
                                    case 4: plc.outValveControl(checked); break;
                                    }
                                }
                                Layout.alignment: Qt.AlignVCenter
                                scale: 0.85
                            }
                        }
                    }
                }
            }
        }
    }
}
