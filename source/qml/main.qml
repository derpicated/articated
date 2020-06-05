import QtQuick 2.12
import QtQuick.Controls 2.12
import articated.augmentation.augmentation_view 1.0

ApplicationWindow
{
    visible: true
    width: 600
    height: 350
    // color: "black"
    title: qsTr("ARticated")

    AugmentationView{
      id: augmentation
      anchors.fill: parent
      Timer {
        interval: 500; running: true; repeat: true
        property var yaw: 0;
        onTriggered: augmentation.drawFrame(yaw++)
      }
    }
    // Squircle {
    //   width: 640
    //   height: 480
    //   SequentialAnimation on t {
    //       NumberAnimation { to: 1; duration: 2500; easing.type: Easing.InQuad }
    //       NumberAnimation { to: 0; duration: 2500; easing.type: Easing.OutQuad }
    //       loops: Animation.Infinite
    //       running: true
    //   }
    // }

    Rectangle {
        id: page
        width: 320; height: 480
        visible: false
        color: "lightgray"

        Text {
            id: helloText
            text: "Hello world!"
            y: 30
            anchors.horizontalCenter: page.horizontalCenter
            font.pointSize: 24; font.bold: true
        }
    }
}
