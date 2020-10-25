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
    anchors.fill: parent
    initialItem: MainView {
      id: mainView

      onOpenSettings: {
        mainStack.push("Settings.qml", {"algorithms": algorithms, "models": models})
        mainStack.currentItem.onModelSelected.connect(function (model_index) {
            mainView.loadModel(model_index)
            mainStack.pop()
        })
        mainStack.currentItem.onAlgorithmSelected.connect(function (algorithm) {
            mainView.selectAlgorithm(algorithm)
            mainStack.pop()
        })
      }
    }
  }
}
