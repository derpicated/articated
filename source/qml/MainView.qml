import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Layouts 1.14

import articated.vision 1.0
import articated.augmentation.augmentation_view 1.0

Item {
  Layout.fillHeight: true
  Layout.fillWidth: true

  signal openSettings(var algorithms, int currentAlgorithm, var models, int currentModel, string currentSource, int debugLevels, int currentDebugLevel)
  Vision {
    id: vision
  }

  Connections {
    target: vision
    function  onFrameProcessed(frame_data) { augmentation.drawFrame(frame_data) }
  }

  AugmentationView{
    id: augmentation
    z: 0
    anchors.fill: parent
  }

  ColumnLayout {
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    anchors.rightMargin: height / 40

    width: height / 4
    z: 5 // don't be shy, come closer to people


    RoundButton {
      implicitWidth: parent.width / 1.5
      implicitHeight: width
      Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
      z: 6
      text: vision.isPaused ? "⏵" : "⏸"
      onPressed: vision.isPaused = !vision.isPaused

      background: Rectangle {
        opacity: parent.down ? 0.5 : 0.1
        radius: parent.width
      }
    }
    RoundButton {
      id: referenceButton
      Layout.fillWidth: true
      implicitHeight: width
      z: 6
      onPressed: vision.SetReference();

      background: Rectangle {
        opacity: parent.down ? 0.7 : 0.5
        radius: parent.width
      }
    }
    RoundButton {
      implicitWidth: parent.width / 1.5
      implicitHeight: width
      Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
      z: 6
      text: "⚙️"
      onPressed: openSettings(vision.algorithms, vision.algorithm, augmentation.models,
        augmentation.model, vision.source, vision.maxDebugLevel, vision.debugLevel)

      background: Rectangle {
        opacity: parent.down ? 0.5 : 0.1
        radius: parent.width
      }
    }
  }

  function setDebugLevel(level) {
    vision.debugLevel = level
  }
  function selectSource(source) {
    vision.source = source
  }
  function loadModel(model_index) {
    augmentation.model = model_index
  }
  function selectAlgorithm(algorithm) {
    vision.algorithm = algorithm
  }

  Keys.onPressed: {
    if (event.key == Qt.Key_Space) {
      vision.SetReference()
      referenceButton.down = true
      event.accepted = true;
    } else if (event.key == Qt.Key_Plus) {
      vision.debugLevel++
      event.accepted = true;
    } else if (event.key == Qt.Key_Minus) {
      vision.debugLevel--
      event.accepted = true;
    }
  }
  Keys.onReleased: {
    if (event.key == Qt.Key_Space) {
      referenceButton.down = false
      event.accepted = true;
    }
  }
  Keys.onEscapePressed: openSettings(vision.algorithms, vision.algorithm, augmentation.models,
    augmentation.model, vision.source, vision.maxDebugLevel, vision.debugLevel)
}
