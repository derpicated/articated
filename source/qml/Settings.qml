import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Layouts 1.14

Rectangle {
  id: settingsPage
  Layout.fillHeight: true
  Layout.fillWidth: true
  signal modelSelected (int model_index)
  signal algorithmSelected(int algorithm)
  property var algorithms;
  property var models;
  property int currentAlgorithmIndex;
  property int currentModelIndex;

  color: "lightgray"

  ColumnLayout {
    anchors.fill: parent
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
