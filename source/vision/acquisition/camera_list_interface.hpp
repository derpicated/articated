#pragma once

#include <QByteArray>
#include <QCamera>
#include <QString>
#include <memory>

class CameraListInterface : public QObject {
    Q_OBJECT

    public:
    virtual ~CameraListInterface () = default;

    virtual QMap<QString, QByteArray> getList ()         = 0;
    virtual QCamera getDefault ()                        = 0;
    virtual QCamera getCameraFromId (QByteArray& player) = 0;
};

std::unique_ptr<CameraListInterface> createCameraListInstance ();
