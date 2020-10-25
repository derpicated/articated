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
    width: height / 4
    z: 5
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    anchors.topMargin: parent.width / 40
    anchors.bottomMargin: parent.width / 40
    anchors.rightMargin: parent.width / 20

    RoundButton {
      Layout.fillHeight: true
      Layout.fillWidth: true
      z: 6
      text: vision.paused ? "⏸" : "⏵"
      onPressed: vision.SetInput (":/debug_samples/3_markers_good.webm");
    }
    RoundButton {
      Layout.fillHeight: true
      Layout.fillWidth: true
      z: 6
      width: implicitHeight
      onPressed: vision.SetReference();
    }
    RoundButton {
      Layout.fillHeight: true
      Layout.fillWidth: true
      z: 6
      text: "Settings"
      onPressed: openSettings(vision.algorithms, augmentation.models)
    }
  }

  function loadModel(model_index) {
      augmentation.LoadObject(augmentation.models[model_index])
  }
  function selectAlgorithm(algorithm) {
      vision.algorithm = algorithm
  }
}
