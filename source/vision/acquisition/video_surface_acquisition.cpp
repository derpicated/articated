#include "acquisition.hpp"

Q_LOGGING_CATEGORY (visionAqcuisitionSurfaceLog, "vision.acquisition.surface", QtInfoMsg)

VideoSurfaceAcquisition::VideoSurfaceAcquisition ()
: QAbstractVideoSurface (nullptr) {
    ;
}

QList<QVideoFrame::PixelFormat> VideoSurfaceAcquisition::supportedPixelFormats (
QAbstractVideoBuffer::HandleType handleType) const {
    Q_UNUSED (handleType);
    return QList<QVideoFrame::PixelFormat> () << QVideoFrame::Format_RGB24;
}

bool VideoSurfaceAcquisition::present (const QVideoFrame& const_buffer) {
    qCDebug (visionAqcuisitionSurfaceLog, "Frame present!");
    emit FrameAvailable (const_buffer);
    return true;
}

void VideoSurfaceAcquisition::setSource (QCamera& camera) {
    camera->setViewfinder (&acquisition_);
}

void VideoSurfaceAcquisition::setSource (QMediaPlayer& player) {
    player->setVideoOutput (&acquisition_);
}


std::unique_ptr<AcquisitionInterface> createAcquisitionInstance () {
    return std::make_unique<VideoSurfaceAcquisition> ();
}
