#pragma once

#include <QAbstractVideoBuffer>
#include <QAbstractVideoSurface>
#include <QList>
#include <QLoggingCategory>
#include <QVideoFrame>

#include "acquisition_interface.hpp"

Q_DECLARE_LOGGING_CATEGORY (visionAcquisitionLog)

class VideoSurfaceAcquisition : public AcquisitionInterface, public QAbstractVideoSurface {
    Q_OBJECT

    public:
    VideoSurfaceAcquisition ();

    QList<QVideoFrame::PixelFormat> supportedPixelFormats (
    QAbstractVideoBuffer::HandleType handleType) const;
    bool present (const QVideoFrame& frame);

    void setSource (QCamera& camera) final;
    void setSource (QMediaPlayer& player) final;
};
