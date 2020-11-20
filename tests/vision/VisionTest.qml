import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.14
import QtQml 2.12

import articated.vision 1.0
import articated.tests.frameDataLister 1.0

Window
{
  id: root
  width: app_layout.childrenRect.width
  height: app_layout.childrenRect.height
  visible: true
  title: qsTr("Vision Test")

  Vision {
    id: vision
    algorithm: algorithmSelectionDropdown.currentIndex
  }

  Connections {
    target: vision
    function  onFrameProcessed(frame_data) { frameDataLister.addFrameData(frame_data) }
  }

  FrameDataLister {
    id: frameDataLister
    length: 15
  }

  ColumnLayout {
    id: app_layout
    Rectangle {
      id: data
      width: 600
      height: width * 0.6
      color: "black"

      Column {
        Repeater {
          model: frameDataLister.data
          delegate: Text {
            text: modelData
            color: "white"
          }
        }
      }
    }
    Rectangle {
      id: controls_container
      Layout.fillWidth: true
      height: childrenRect.height
      color: "white"

      GridLayout {
        width: parent.width
        Layout.margins: 10
        columns: 2
        ComboBox {
          id: algorithmSelectionDropdown
          Layout.fillWidth: true
          model: vision.algorithms
          onActivated: {
            console.log(model[index])
          }
        }
        Button {
          Layout.fillWidth: true
          text: "Set Reference"
          onClicked: vision.SetReference()
        }
        Button {
          Layout.fillWidth: true
          text: vision.isPaused ? "Pause" : "Unpause"
          onClicked: vision.isPaused = !vision.isPaused
        }
        Button {
          Layout.fillWidth: true
          text: "Load Test Video"
          onClicked: vision.SetInput (":/debug_samples/3_markers_good.webm")
        }
      }
    }
  }
}
