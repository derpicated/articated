import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Layouts 1.14

import articated.vision 1.0
import articated.augmentation.augmentation_view 1.0

Item {
  Layout.fillHeight: true
  Layout.fillWidth: true

  signal openSettings(var algorithms, var models)
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
    z: 5


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
      text: "Settings"
      onPressed: openSettings(vision.algorithms, augmentation.models)

      background: Rectangle {
        opacity: parent.down ? 0.5 : 0.1
        radius: parent.width
      }
    }
  }

  function loadDemo() {
      vision.SetInput (":/debug_samples/3_markers_good.webm")
  }

  function loadModel(model_index) {
      augmentation.LoadObject(augmentation.models[model_index])
  }
  function selectAlgorithm(algorithm) {
      vision.algorithm = algorithm
  }
}
