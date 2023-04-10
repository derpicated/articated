import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Layouts 1.14
import QtMultimedia 6.4

Page {
  id: settingsPage
  title: "Settings"
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

  // color: "lightgray"

  header: ToolBar {
    RowLayout {
      anchors.fill: parent
      ToolButton {
        text: "‹"
        onClicked: exit()
      }
      Label {
        text: settingsPage.title
        elide: Label.ElideRight
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        Layout.fillWidth: true
      }
    }
  }

  GridLayout {
    anchors.fill: parent
    anchors.margins: 20
    columns: 2

    Button {
      Layout.fillWidth: true
      Layout.columnSpan: 2
      onPressed: sourceSelected(":/debug_samples/3_markers_good.webm")
      text: "Load Demo Video"
    }

    Text { text: "Input:"}
    ComboBox {
      MediaDevices {
        id: devices
      }

      Layout.fillWidth: true
      model: devices.videoInputs
      textRole: "description"
      currentIndex: -1
      delegate: ItemDelegate {
        text: modelData.description
        width: parent.width
      }

      onActivated: index => {
        sourceSelected(model[index].id)
      }
      Component.onCompleted: {
        // find current selected camera, if any
        for (var i = 0; i < model.length; i++) {
          if (model[i].deviceId == currentSource) {
            currentIndex = i;
          }
        }
      }
    }

    Text { text: "Algorithm:"}
    ComboBox {
      Layout.fillWidth: true
      model: algorithms
      currentIndex: currentAlgorithm
      onActivated: index => {
        algorithmSelected(index)
      }
    }

    Text { text: "Model:"}
    ComboBox {
      Layout.fillWidth: true
      model: models
      currentIndex: currentModel
      onActivated: {
        modelSelected(index)
      }
    }

    Text { text: "Debug Level:"}
    SpinBox {
      value: currentDebugLevel
      to: debugLevels
      onValueModified: debugLevelSelected(value)
    }
  }
  Keys.onEscapePressed: exit()
}
