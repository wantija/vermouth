import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    width: 800
    height: 800
    minimumWidth: 500
    minimumHeight: 400
    visible: true
    title: "Vermouth"

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            anchors.margins: 8

            Image {
                source: "qrc:/icons/vermouth.svg"
                Layout.preferredWidth: 28
                Layout.preferredHeight: 28
                fillMode: Image.PreserveAspectFit
            }

            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: "Search..."
                onTextChanged: appModel.setFilterString(text)
            }

            Button {
                text: "Add App/Game"
                icon.name: "list-add"
                onClicked: addDialog.openForNew()
                highlighted: true
            }
        }
    }

    AppGridView {
        anchors.fill: parent
        anchors.margins: 12
    }

    AddAppDialog {
        id: addDialog
    }

    RunExeDialog {
        id: runExeDialog
    }

    Connections {
        target: launcher
        function onLaunched(name) {
            launchToast.text = "Launched: " + name
            launchToast.visible = true
            toastTimer.restart()
        }
        function onLaunchError(name, error) {
            launchToast.text = "Error: " + error
            launchToast.visible = true
            toastTimer.restart()
        }
    }

    Label {
        id: launchToast
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 20
        visible: false
        padding: 12
        font.pixelSize: 13
        background: Rectangle {
            color: palette.toolTipBase
            radius: 6
        }
        color: palette.toolTipText
    }

    Timer {
        id: toastTimer
        interval: 3000
        onTriggered: launchToast.visible = false
    }
}
