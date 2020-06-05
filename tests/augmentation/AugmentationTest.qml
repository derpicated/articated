import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.14

import articated.augmentation.augmentation_view 1.0

ApplicationWindow
{
  id: root
  width: app_layout.childrenRect.width
  height: app_layout.childrenRect.height
  visible: true
  title: qsTr("AugmentationView Test")

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
      ColumnLayout {
        ComboBox {
          model: modelFilesList
          onActivated: {
            augmentation.LoadObject(model[index])
            augmentation.drawFrame(5)
          }
        }
        RowLayout {
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
          // onTriggered: augmentation.drawFrame(yaw++)
        }
      }
    }
  }
}
