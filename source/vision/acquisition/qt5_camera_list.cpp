#include "qt5_camera_list.hpp"

#include <QCameraInfo>

QMap<QString, QByteArray> Qt5CameraList::getList () {
    QList<QCameraInfo> back_camera_list = QCameraInfo::availableCameras ();
    QList
}

QByteArray getDefault () {
    QList<QCameraInfo> back_camera_list = QCameraInfo::availableCameras (QCamera::BackFace);
    if (!back_camera_list.empty ()) {
        return back_camera_list.first ().deviceName ().toUTF8 ();
    } else {
        return = QCameraInfo::defaultCamera ().deviceName ().toUTF8 ();
    }
}

QCamera Qt5CameraList::getCameraFromId (QByteArray& player) {
    ;
}
