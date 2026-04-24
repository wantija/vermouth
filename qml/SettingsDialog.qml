import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami

Kirigami.PromptDialog {
    id: dialog
    title: i18n("Settings")
    preferredWidth: Kirigami.Units.gridUnit * 30
    bottomPadding: 30
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
    onAccepted: {
        settingsManager.setUmuPath(umuPathField.text);
        settingsManager.setDefaultPrefixDir(prefixDirField.text);
        settingsManager.setDefaultGamePrefix(gamePrefixField.text);
        defaultRuntimePicker.saveToSettings();
        var vars = [];
        for (var i = 0; i < envModel.count; i++) {
            var k = envModel.get(i).key.trim();
            var v = envModel.get(i).value.trim();
            if (k !== "")
                vars.push(k + "=" + v);
        }
        settingsManager.setGlobalEnvVars(vars);
    }

    function openDialog() {
        umuPathField.text = settingsManager.umuPath;
        prefixDirField.text = settingsManager.defaultPrefixDir;
        gamePrefixField.text = settingsManager.defaultGamePrefix;
        pathsModel.clear();
        var paths = settingsManager.extraProtonPaths;
        for (var i = 0; i < paths.length; i++) {
            pathsModel.append({
                "path": paths[i]
            });
        }
        envModel.clear();
        var vars = settingsManager.globalEnvVars;
        for (var j = 0; j < vars.length; j++) {
            var sep = vars[j].indexOf("=");
            envModel.append({
                "key": sep > 0 ? vars[j].substring(0, sep) : vars[j],
                "value": sep > 0 ? vars[j].substring(sep + 1) : ""
            });
        }
        defaultRuntimePicker.reset();
        dialog.open();
    }

    ListModel {
        id: pathsModel
    }

    ListModel {
        id: envModel
    }

    ColumnLayout {
        spacing: Kirigami.Units.smallSpacing

        RuntimePicker {
            id: defaultRuntimePicker
            Layout.fillWidth: true
            sectionLabel: i18n("Default Runtime")
        }

        Kirigami.FormLayout {
            twinFormLayouts: defaultRuntimePicker.formLayout

            Kirigami.Separator {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: i18n("Appearance")
            }

            RowLayout {
                Kirigami.FormData.label: i18n("Lights Out:")
                QQC2.Switch {
                    checked: settingsManager.lightsOut
                    onToggled: settingsManager.setLightsOut(checked)
                }
            }

            RowLayout {
                Kirigami.FormData.label: i18n("Background Color:")
                opacity: settingsManager.lightsOut ? 1.0 : 0.5

                Rectangle {
                    width: Kirigami.Units.gridUnit * 4
                    height: Kirigami.Units.gridUnit * 1.5
                    color: settingsManager.lightsOutColor
                    radius: Kirigami.Units.cornerRadius
                    border.color: Kirigami.Theme.disabledTextColor
                    border.width: 1

                    MouseArea {
                        anchors.fill: parent
                        enabled: settingsManager.lightsOut
                        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                        onClicked: {
                            lightsOutColorDialog.selectedColor = settingsManager.lightsOutColor;
                            lightsOutColorDialog.open();
                        }
                    }
                }

                QQC2.Button {
                    text: i18n("Reset")
                    enabled: settingsManager.lightsOut
                    onClicked: settingsManager.setLightsOutColor("#0d1b3e")
                }
            }

            Kirigami.Separator {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: i18n("umu-launcher")
            }

            QQC2.Label {
                Kirigami.FormData.label: ""
                text: i18n("umu-launcher runs Proton through the Steam Runtime (pressure-vessel), which significantly improves game compatibility - especially for games with video cutscenes or anti-cheat. Strongly recommended.")
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                Layout.maximumWidth: Kirigami.Units.gridUnit * 26
                font.italic: true
                opacity: 0.8
            }

            RowLayout {
                Kirigami.FormData.label: i18n("umu-run path:")
                QQC2.TextField {
                    id: umuPathField
                    Layout.fillWidth: true
                    placeholderText: i18n("Auto-detect (umu-run in PATH)")
                }
                QQC2.Button {
                    icon.name: "document-open"
                    onClicked: umuFilePicker.open()
                }
                QQC2.Button {
                    icon.name: "download"
                    enabled: !umuDownloader.busy
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.text: umuDownloader.statusText ? umuDownloader.statusText : i18n("Download latest umu-launcher")
                    onClicked: umuDownloader.downloadLatest()
                }
            }

            Connections {
                target: settingsManager
                function onUmuPathChanged() {
                    umuPathField.text = settingsManager.umuPath;
                }
            }

            Kirigami.Separator {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: i18n("Prefixes")
            }

            RowLayout {
                Kirigami.FormData.label: i18n("Default Prefix Parent Folder:")
                QQC2.TextField {
                    id: prefixDirField
                    Layout.fillWidth: true
                    placeholderText: protonScanner.prefixBasePath()
                }
                QQC2.Button {
                    icon.name: "document-open"
                    onClicked: prefixDirFolderDialog.open()
                }
            }

            QQC2.Label {
                Kirigami.FormData.label: ""
                text: i18n("This is the folder where Vermouth stores all the created prefixes by default.")
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                Layout.maximumWidth: Kirigami.Units.gridUnit * 26
                font.italic: true
                opacity: 0.8
            }

            RowLayout {
                Kirigami.FormData.label: i18n("Default App/Game Prefix:")
                QQC2.TextField {
                    id: gamePrefixField
                    Layout.fillWidth: true
                    placeholderText: i18n("Auto-generate per app/game")
                }
                QQC2.Button {
                    icon.name: "document-open"
                    onClicked: gamePrefixFolderDialog.open()
                }
            }

            QQC2.Label {
                Kirigami.FormData.label: ""
                text: i18n("Set this if you want all apps and games to share a single prefix (e.g. one Wine/Proton environment for everything). Leave empty to auto-generate a separate prefix per game. You can still use separate prefixe per game, but you have to set it explicitly.")
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                Layout.maximumWidth: Kirigami.Units.gridUnit * 26
                font.italic: true
                opacity: 0.8
            }

            Kirigami.Separator {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: i18n("Vermouth Proton Folder")
            }

            RowLayout {
                Kirigami.FormData.label: i18n("Download GE Proton to run most games and apps - no Steam or manual setup needed.")
                QQC2.Button {
                    icon.name: "folder-open"
                    text: i18n("Open Vermouth Proton folder")
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.text: protonScanner.localProtonPath()
                    onClicked: Qt.openUrlExternally("file://" + protonScanner.localProtonPath())
                }
                QQC2.Button {
                    text: i18n("Download Latest GE Proton")
                    icon.name: "download"
                    enabled: !protonDownloader.busy
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.text: protonDownloader.statusText ? protonDownloader.statusText : i18n("Download latest GE Proton")
                    onClicked: protonDownloader.downloadLatest()
                }
            }

            Kirigami.Separator {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: i18n("Extra Proton Scan Paths")
            }

            ColumnLayout {
                Kirigami.FormData.label: i18n("Folders to scan for Proton installations (in addition to Steam and local paths).")
                Layout.fillWidth: true
                Repeater {
                    model: pathsModel
                    delegate: RowLayout {
                        Layout.fillWidth: true
                        QQC2.TextField {
                            text: model.path
                            Layout.fillWidth: true
                            readOnly: true
                        }
                        QQC2.Button {
                            icon.name: "list-remove"
                            onClicked: {
                                settingsManager.removeExtraProtonPath(index);
                                pathsModel.remove(index);
                            }
                        }
                    }
                }

                QQC2.Button {
                    text: i18n("Add Path...")
                    icon.name: "list-add"
                    onClicked: protonPathFolderDialog.open()
                }
            }

            Kirigami.Separator {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: i18n("Global Environment Variables")
            }

            ColumnLayout {
                Kirigami.FormData.label: i18n("Applied to every game. Per-game launch options can override these.")
                Layout.fillWidth: true

                Repeater {
                    model: envModel
                    delegate: RowLayout {
                        Layout.fillWidth: true
                        QQC2.TextField {
                            placeholderText: i18n("KEY")
                            text: model.key
                            implicitWidth: Kirigami.Units.gridUnit * 9
                            onTextChanged: envModel.setProperty(index, "key", text)
                        }
                        QQC2.Label {
                            text: "="
                        }
                        QQC2.TextField {
                            placeholderText: i18n("value")
                            text: model.value
                            Layout.fillWidth: true
                            onTextChanged: envModel.setProperty(index, "value", text)
                        }
                        QQC2.Button {
                            icon.name: "list-remove"
                            onClicked: envModel.remove(index)
                        }
                    }
                }

                QQC2.Button {
                    text: i18n("Add Variable")
                    icon.name: "list-add"
                    onClicked: envModel.append({
                        "key": "",
                        "value": ""
                    })
                }
            }
        }
    }

    FileDialog {
        id: umuFilePicker
        title: i18n("Select umu-run binary")
        currentFolder: umuPathField.text !== "" ? "file://" + umuPathField.text.substring(0, umuPathField.text.lastIndexOf("/")) : "file://" + protonScanner.homePath()
        onAccepted: umuPathField.text = decodeURIComponent(selectedFile.toString().replace("file://", ""))
    }

    FolderDialog {
        id: prefixDirFolderDialog
        title: i18n("Select Default Prefix Folder")
        currentFolder: "file://" + protonScanner.homePath()
        onAccepted: prefixDirField.text = decodeURIComponent(selectedFolder.toString().replace("file://", ""))
    }

    FolderDialog {
        id: gamePrefixFolderDialog
        title: i18n("Select Default App/Game Prefix")
        currentFolder: "file://" + protonScanner.prefixBasePath()
        onAccepted: gamePrefixField.text = decodeURIComponent(selectedFolder.toString().replace("file://", ""))
    }

    FolderDialog {
        id: protonPathFolderDialog
        title: i18n("Select Proton Scan Folder")
        currentFolder: "file://" + protonScanner.homePath()
        onAccepted: {
            var path = decodeURIComponent(selectedFolder.toString().replace("file://", ""));
            settingsManager.addExtraProtonPath(path);
            pathsModel.append({
                "path": path
            });
        }
    }

    ColorDialog {
        id: lightsOutColorDialog
        title: i18n("Select Background Color")
        onAccepted: {
            var r = Math.round(selectedColor.r * 255);
            var g = Math.round(selectedColor.g * 255);
            var b = Math.round(selectedColor.b * 255);
            settingsManager.setLightsOutColor("#" + r.toString(16).padStart(2, "0") + g.toString(16).padStart(2, "0") + b.toString(16).padStart(2, "0"));
        }
    }
}
