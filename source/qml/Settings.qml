import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Layouts 1.14
import QtMultimedia 5.14

Rectangle {
  id: settingsPage
  Layout.fillHeight: true
  Layout.fillWidth: true
  signal sourceSelected(string source)
  signal modelSelected (int modelIndex)
  signal algorithmSelected(int algorithm)
  property var algorithms;
  property var models;
  property int currentAlgorithmIndex;
  property int currentModelIndex;

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
    }
    ComboBox {
      Layout.fillWidth: true
      model: algorithms
      currentIndex: currentAlgorithmIndex
      onActivated: {
        algorithmSelected(index)
      }
    }
    ComboBox {
      Layout.fillWidth: true
      model: models
      currentIndex: currentModelIndex
      onActivated: {
        modelSelected(index)
      }
    }
  }
}
