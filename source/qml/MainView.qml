import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Layouts 1.14
import articated.vision 1.0
import articated.augmentation.augmentation_view 1.0
import "." as Components

Item {
    property alias settings: settings_instance

    signal openSettings

    function loadModel(model_index) {
        augmentation.model = model_index;
    }
    function selectAlgorithm(algorithm) {
        vision.algorithm = algorithm;
    }
    function selectSource(source) {
        vision.source = source;
    }
    function setDebugLevel(level) {
        vision.debugLevel = level;
    }

    Layout.fillHeight: true
    Layout.fillWidth: true

    Keys.onEscapePressed: openSettings()
    Keys.onPressed: event => {
        if (event.key == Qt.Key_Space) {
            vision.SetReference();
            referenceButton.down = true;
            event.accepted = true;
        } else if (event.key == Qt.Key_Plus) {
            settings_instance.currentDebugLevel = Math.min(settings.debugLevels, settings_instance.currentDebugLevel + 1);
            event.accepted = true;
        } else if (event.key == Qt.Key_Minus) {
            settings_instance.currentDebugLevel = Math.max(0, settings_instance.currentDebugLevel - 1);
            event.accepted = true;
        }
    }
    Keys.onReleased: event => {
        if (event.key == Qt.Key_Space) {
            referenceButton.down = false;
            event.accepted = true;
        }
    }

    Components.Settings {
        id: settings_instance
        algorithms: vision.algorithms
        debugLevels: vision.maxDebugLevel
        models: augmentation.models
    }
    Vision {
        id: vision
        algorithm: settings_instance.currentAlgorithm
        debugLevel: settings_instance.currentDebugLevel
        isPaused: false
        source: settings_instance.currentSource
    }
    Connections {
        function onFrameProcessed(frame_data) {
            augmentation.drawFrame(frame_data);
        }

        target: vision
    }
    AugmentationView {
        id: augmentation
        anchors.fill: parent
        model: settings_instance.currentModel
        z: 0
    }
    ColumnLayout {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: height / 40
        anchors.top: parent.top
        width: height / 4
        z: 5 // don't be shy, come closer to people

        RoundButton {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            implicitHeight: width
            implicitWidth: parent.width / 1.5
            text: vision.isPaused ? "⏵" : "⏸"
            z: 6

            background: Rectangle {
                opacity: parent.down ? 0.5 : 0.1
                radius: parent.width
            }

            onPressed: vision.isPaused = !vision.isPaused
        }
        RoundButton {
            id: referenceButton
            Layout.fillWidth: true
            implicitHeight: width
            z: 6

            background: Rectangle {
                opacity: parent.down ? 0.7 : 0.5
                radius: parent.width
            }

            onPressed: vision.SetReference()
        }
        RoundButton {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            implicitHeight: width
            implicitWidth: parent.width / 1.5
            text: "⚙️"
            z: 6

            background: Rectangle {
                opacity: parent.down ? 0.5 : 0.1
                radius: parent.width
            }

            onPressed: openSettings()
        }
    }
}
