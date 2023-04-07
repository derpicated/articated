#pragma once

#include <QCamera>
#include <QMediaPlayer>
#include <memory>

class AcquisitionInterface : public QObject {
    Q_OBJECT

    public:
    virtual ~AcquisitionInterface () = default;

    virtual void setSource (QCamera& camera)      = 0;
    virtual void setSource (QMediaPlayer& player) = 0;

    signals:
    void frameAvailable (const QVideoFrame& frame);
};

std::unique_ptr<AcquisitionInterface> createAcquisitionInstance ();
