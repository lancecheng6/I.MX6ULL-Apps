import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import com.alientek.qmlcomponents 1.0

Window {
    id: root
    visible: true
    width: Screen.desktopAvailableWidth
    height: Screen.desktopAvailableHeight
    x: 0
    y: 0
    color: "#1a1a2e"
    title: "SIEMENS HMI"

    // Main UI: z=0 for touch events
    AppMainBody {
        anchors.fill: parent
        z: 0
    }

    // SystemUI Client: z=99 keeps floating back button on top
    Client {
        programmerName: "HeatingSCADA"
        anchors.fill: parent
        z: 99
    }
}
