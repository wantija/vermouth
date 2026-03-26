import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Dialog {
    id: dialog
    title: "Run EXE in Prefix"
    modal: true
    width: 450
    height: 200
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel

    property int appIndex: -1

    onAccepted: {
        if (appIndex >= 0 && exeField.text !== "") {
            var app = appModel.getApp(appIndex)
            launcher.runInPrefix(app, exeField.text)
        }
    }

    onOpened: exeField.text = ""

    contentItem: ColumnLayout {
        spacing: 10

        Label {
            text: appIndex >= 0 ? "Run an executable using the same prefix as \"" + appModel.getApp(appIndex).name + "\"" : ""
            font.pixelSize: 12
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        Label { text: "Executable (.exe)"; font.pixelSize: 12 }
        RowLayout {
            Layout.fillWidth: true
            TextField {
                id: exeField
                Layout.fillWidth: true
                placeholderText: "/path/to/setup.exe"
            }
            Button {
                text: "Browse"
                onClicked: fileDialog.open()
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Select Executable"
        nameFilters: ["Executables (*.exe)", "All files (*)"]
        onAccepted: exeField.text = selectedFile.toString().replace("file://", "")
    }
}
