import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.14
import QtQml 2.12

import articated.augmentation.augmentation_view 1.0

Window
{
  id: augmentationPopoutRoot
  visible: true
  width: 600
  height: 300
  title: "Augmentation Popout"

  Connections {
      target: vision
      onFrameProcessed: function (frame_data){ augmentationPopoutView.drawFrame(frame_data) }
  }

  AugmentationView {
    id: augmentationPopoutView
    anchors.fill: parent
  }
}
