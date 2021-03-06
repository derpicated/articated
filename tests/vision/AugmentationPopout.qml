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
    width: app_layout.childrenRect.width
    height: app_layout.childrenRect.height
    title: "Augmentation Popout"

    Connections {
        target: vision
        onFrameProcessed: function (frame_data){ augmentation.drawFrame(frame_data) }
    }

    ColumnLayout {
        id: app_layout

        AugmentationView {
            id: augmentation
            width: 600
            height: width * 0.6
        }

        Rectangle {
            id: controls_container
            Layout.fillWidth: true
            height: childrenRect.height
            color: "white"

            ComboBox {
                model: augmentation.models
                onActivated: {
                    augmentation.model = index
                }
            }
        }
    }
}
