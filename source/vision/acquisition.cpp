#include "acquisition.hpp"

Acquisition::Acquisition (QObject* parent)
: QAbstractVideoSurface (parent) {
    ;
}

QList<QVideoFrame::PixelFormat> Acquisition::supportedPixelFormats (
QAbstractVideoBuffer::HandleType handleType) const {
    Q_UNUSED (handleType);
    return QList<QVideoFrame::PixelFormat> () << QVideoFrame::Format_RGB24;
}

bool Acquisition::present (const QVideoFrame& const_buffer) {
    emit FrameAvailable (const_buffer);
    return true;
}
