#pragma once

class Qt5CameraList : public QObject {
    Q_OBJECT

    public:
    virtual ~Qt5CameraList () = default;

    QMap<QString, QByteArray> getList () final;
    QCamera getDefault () final;
    QCamera getCameraFromId (QByteArray& player) final;
};
