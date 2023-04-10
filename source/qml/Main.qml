import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Layouts 1.14

ApplicationWindow {
    id: appWindow
    height: 350
    title: "ARticated"
    visible: true
    width: 600

    StackView {
        id: mainStack
        anchors.fill: parent
        focus: true

        initialItem: MainView {
            id: mainView
            onOpenSettings: {
                mainStack.push("SettingsView.qml", {
                        "settings": settings
                    });
                mainStack.currentItem.onExit.connect(function () {
                        mainStack.pop();
                    });
            }
        }
    }
    Shortcut {
        sequence: "Ctrl+W"

        onActivated: Qt.quit()
    }
}
