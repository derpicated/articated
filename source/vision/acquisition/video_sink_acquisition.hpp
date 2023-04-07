#pragma once

#include <QLoggingCategory>
#include <QVideoFrame>
#include <QVideoSink>

#include "acquisition_interface.hpp"

Q_DECLARE_LOGGING_CATEGORY (visionAcquisitionLog)

class VideoSinkAcquisition : public AcquisitionInterface {
    Q_OBJECT

    public:
    void setSource (QCamera& camera) final;
    void setSource (QMediaPlayer& player) final;

    private:
    QVideoSink sink_;
};
