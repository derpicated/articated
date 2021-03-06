#include "acquisition.hpp"

Q_LOGGING_CATEGORY(visionAqcuisitionLog, "vision.acquisition", QtInfoMsg)

Acquisition::Acquisition ()
: QAbstractVideoSurface (nullptr) {
    ;
}

QList<QVideoFrame::PixelFormat> Acquisition::supportedPixelFormats (
QAbstractVideoBuffer::HandleType handleType) const {
    Q_UNUSED (handleType);
    return QList<QVideoFrame::PixelFormat> () << QVideoFrame::Format_RGB24;
}

bool Acquisition::present (const QVideoFrame& const_buffer) {
    qCDebug (visionAqcuisitionLog, "Frame present!");
    emit FrameAvailable (const_buffer);
    return true;
}
