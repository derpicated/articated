import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Layouts 1.14
import QtMultimedia 6.4
import "." as Components

Page {
    id: settingsPage

    property Components.Settings settings

    signal exit

    Layout.fillHeight: true
    Layout.fillWidth: true
    title: "Settings"

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                text: "‹"

                onClicked: exit()
            }
            Label {
                Layout.fillWidth: true
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                text: settingsPage.title
                verticalAlignment: Qt.AlignVCenter
            }
        }
    }

    Keys.onEscapePressed: exit()

    GridLayout {
        anchors.fill: parent
        anchors.margins: 20
        columns: 2

        Button {
            Layout.columnSpan: 2
            Layout.fillWidth: true
            text: "Load Demo Video"

            onPressed: settings.currentSource = ":/debug_samples/3_markers_good.webm"
        }
        Text {
            text: "Input:"
        }
        ComboBox {
            Layout.fillWidth: true
            currentIndex: -1
            model: devices.videoInputs
            textRole: "description"

            delegate: ItemDelegate {
                text: modelData.description
                width: parent.width
            }

            Component.onCompleted: {
                // find current selected camera, if any
                for (var i = 0; i < model.length; i++) {
                    if (model[i].deviceId == settings.currentSource) {
                        currentIndex = i;
                    }
                }
            }
            onActivated: index => {
                settings.currentSource = model[index].id;
            }

            MediaDevices {
                id: devices
            }
        }
        Text {
            text: "Algorithm:"
        }
        ComboBox {
            Layout.fillWidth: true
            currentIndex: settings.currentAlgorithm
            model: settings.algorithms

            onActivated: index => {
                settings.currentAlgorithm = index;
            }
        }
        Text {
            text: "Model:"
        }
        ComboBox {
            Layout.fillWidth: true
            currentIndex: settings.currentModel
            model: settings.models

            onActivated: inxed => {
                settings.currentModel = index;
            }
        }
        Text {
            text: "Debug Level:"
        }
        SpinBox {
            to: settings.debugLevels
            value: settings.currentDebugLevel

            onValueModified: {
                settings.currentDebugLevel = value;
            }
        }
    }
}
