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

      onOpenSettings: {
        mainStack.push("Settings.qml", {
          "settings": settings
        })
        mainStack.currentItem.onExit.connect(function () {
            mainStack.pop()
        })
      }
    }
  }

  Shortcut {
    sequence: "Ctrl+W"
    onActivated: Qt.quit()
  }
}
