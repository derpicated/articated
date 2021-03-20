import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.14
import QtQml 2.12

import articated.augmentation.augmentation_view 1.0
import articated.tests.augmentation.mock_algorithm 1.0

Window
{
  id: root
  width: app_layout.childrenRect.width
  height: app_layout.childrenRect.height
  visible: true
  title: qsTr("AugmentationView Test")

  MockAlgorithm {
    id: mockAlgorithm
    rotationX: rotationX.value
    rotationY: rotationY.value
    rotationZ: rotationZ.value
    translationX: translationX.value
    translationY: translationY.value
    scale: scale.value
  }

  Connections {
    target: mockAlgorithm
    function  onFrameReady(frame_data) { augmentation.drawFrame(frame_data) }
  }

  ColumnLayout {
    id: app_layout
    AugmentationView {
      id: augmentation
      width: 600
      height: width * 0.6
    }
    Rectangle {
      id: controls_container
      Layout.fillWidth: true
      height: childrenRect.height
      color: "white"
      RowLayout {
        ColumnLayout {
          ComboBox {
            model: augmentation.models
            onActivated: {
              augmentation.model = index
            }
          }
          GridLayout {
            id: rotation_controls
            Layout.margins: 5
            columns: 2
            Label { text: "X Rotation" }
            Slider { id: rotationX; from: 0; to: 360 }
            Label { text: "Y Rotation" }
            Slider { id: rotationY; from: 0; to: 360; }
            Label { text: "Z Rotation" }
            Slider { id: rotationZ; from: 0; to: 360; }
          }
        }

        ColumnLayout {
          Button {
            text: "Load Test Texture"
            onClicked: mockAlgorithm.loadTexture();
          }
          GridLayout {
            id: translation_controls
            Layout.margins: 5
            columns: 2
            Label { text: "X Translation" }
            Slider { id: translationX; from: -1; to: 1 }
            Label { text: "Y Translation" }
            Slider { id: translationY; from: -1; to: 1 }
            Label { text: "Scale" }
            Slider { id: scale; from: 0; to: 1; value: 1}
          }
        }
      }
    }
  }
}
