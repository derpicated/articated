import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Layouts 1.14
import QtMultimedia 5.14

Rectangle {
  id: settingsPage
  Layout.fillHeight: true
  Layout.fillWidth: true
  signal exit()
  signal debugLevelSelected(int level)
  signal sourceSelected(string source)
  signal modelSelected (int modelIndex)
  signal algorithmSelected(int algorithm)
  property var algorithms;
  property var models;
  property string currentSource;
  property int currentAlgorithm;
  property int currentModel;
  property int debugLevels;
  property int currentDebugLevel;

  color: "lightgray"

  ColumnLayout {
    anchors.fill: parent

    Button {
      Layout.fillWidth: true
      onPressed: sourceSelected(":/debug_samples/3_markers_good.webm")
      text: "Load Demo Video"
    }
    ComboBox {
      Layout.fillWidth: true
      model: QtMultimedia.availableCameras
      textRole: "displayName"
      currentIndex: -1
      onActivated: {
        sourceSelected(model[index].deviceId)
      }
      Component.onCompleted: {
        // find current selected camera, if any
        for (var i = 0; i < model.length; i++) {
          console.error(currentSource + " compared to " + model[i].deviceId)
          if (model[i].deviceId == currentSource) {
            console.error("truueee omg")
            currentIndex = i;
          }
        }
      }
    }
    ComboBox {
      Layout.fillWidth: true
      model: algorithms
      currentIndex: currentAlgorithm
      onActivated: {
        algorithmSelected(index)
      }
    }
    ComboBox {
      Layout.fillWidth: true
      model: models
      currentIndex: currentModel
      onActivated: {
        modelSelected(index)
      }
    }
    SpinBox {
      value: currentDebugLevel
      to: debugLevels
      onValueModified: debugLevelSelected(value)
    }
  }
  Keys.onEscapePressed: exit()
}
