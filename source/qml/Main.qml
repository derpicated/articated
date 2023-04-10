import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Layouts 1.14


ApplicationWindow
{
  id: appWindow
  visible: true
  width: 600
  height: 350
  title: "ARticated"

  StackView{
    id: mainStack
    focus: true
    anchors.fill: parent
    initialItem: MainView {
      id: mainView

      onOpenSettings: (algorithms, currentAlgorithm, models, currentModel, currentSource, debugLevels, currentDebugLevel) => {
        mainStack.push("Settings.qml", {
          "algorithms": algorithms,
          "currentAlgorithm": currentAlgorithm,
          "models": models,
          "currentModel": currentModel,
          "currentSource": currentSource,
          "debugLevels": debugLevels,
          "currentDebugLevel": currentDebugLevel
        })
        mainStack.currentItem.onExit.connect(function () {
            mainStack.pop()
        })
        mainStack.currentItem.onSourceSelected.connect(function (source) {
            mainView.selectSource(source)
        })
        mainStack.currentItem.onDebugLevelSelected.connect(function (level) {
            mainView.setDebugLevel(level)
        })
        mainStack.currentItem.onModelSelected.connect(function (modelIndex) {
            mainView.loadModel(modelIndex)
        })
        mainStack.currentItem.onAlgorithmSelected.connect(function (algorithm) {
            mainView.selectAlgorithm(algorithm)
        })
      }
    }
  }

  Shortcut {
    sequence: "Ctrl+W"
    onActivated: Qt.quit()
  }
}
