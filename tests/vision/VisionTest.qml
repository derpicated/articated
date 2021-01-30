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
  title: "Vision Test"

  Vision {
    id: vision
    algorithm: algorithmSelectionDropdown.currentIndex
    playbackRate: playbackRateSlider.value
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
          onClicked: function (){vision.source = ":/debug_samples/3_markers_good.webm"}
        }
        Label {
          Layout.fillWidth: true
          horizontalAlignment: Text.AlignRight
          text: "Test Video Playback Rate:"
        }
        Slider {
          id: playbackRateSlider
          Layout.fillWidth: true
          from: 0
          to: 1
          value: 1
        }
        Label {
          Layout.fillWidth: true
          horizontalAlignment: Text.AlignRight
          text: "Show Augmentation Popout:"
        }
        Switch {
          id: augmentationPopoutSwitch
          property variant popout;
          property variant popout_component;

          onToggled: function () {
            if (augmentationPopoutSwitch.checked) {
              popout_component = Qt.createComponent("AugmentationPopout.qml");
              if (popout_component.status == Component.Ready) {
                finishCreation();
              } else if (popout_component.status == Component.Error) {
                console.log("Error loading component:", popout_component.errorString());
              } else {
                popout_component.statusChanged.connect(finishCreation);
              }
            } else {
              popout.destroy();
            }
          }

          function finishCreation () {
            if (popout_component.status == Component.Ready) {
              // set vision as parent to allow popout to connect to its signals, hacks
              popout = popout_component.createObject(vision);
              if (popout == null) {
                console.log("Error creating object");
              }
              popout.show();
            } else if (popout_component.status == Component.Error) {
              console.log("Error loading component:", popout_component.errorString());
            }
          }
        }
      }
    }
  }
}
