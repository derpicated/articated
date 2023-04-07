import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Layouts 1.14
import QtMultimedia 5.14

import "." as Components

Page {
  id: settingsPage

  property Components.Settings settings

  signal exit

  title: "Settings"
  Layout.fillHeight: true
  Layout.fillWidth: true

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
      onPressed: settings.currentSource = ":/debug_samples/3_markers_good.webm"
      text: "Load Demo Video"
    }

    Text { text: "Input:"}
    ComboBox {
      Layout.fillWidth: true
      model: settings.cameras
      textRole: "displayName"
      currentIndex: -1
      onActivated: {
        settings.currentSource = model[index].deviceId
      }
      Component.onCompleted: {
        // find current selected camera, if any
        for (var i = 0; i < model.length; i++) {
          if (model[i].deviceId == settings.currentSource) {
            currentIndex = i;
          }
        }
      }
    }

    Text { text: "Algorithm:"}
    ComboBox {
      Layout.fillWidth: true
      model: settings.algorithms
      currentIndex: settings.currentAlgorithm
      onActivated: {
        settings.currentAlgorithm = index
      }
    }

    Text { text: "Model:"}
    ComboBox {
      Layout.fillWidth: true
      model: settings.models
      currentIndex: settings.currentModel
      onActivated: {
        settings.currentModel = index
      }
    }

    Text { text: "Debug Level:"}
    SpinBox {
      value: settings.currentDebugLevel
      to: settings.debugLevels
      onValueModified: {
        settings.currentDebugLevel = value
      }
    }
  }
  Keys.onEscapePressed: exit()
}
