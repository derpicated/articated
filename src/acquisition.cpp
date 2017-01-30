#include "acquisition.hpp"

acquisition::acquisition (QObject* parent)
: QAbstractVideoSurface (parent) {
    ;
}

QList<QVideoFrame::PixelFormat> acquisition::supportedPixelFormats (
QAbstractVideoBuffer::HandleType handleType) const {
    Q_UNUSED (handleType);
    return QList<QVideoFrame::PixelFormat> () << QVideoFrame::Format_RGB24;
}

bool acquisition::present (const QVideoFrame& const_buffer) {
    emit frameAvailable (const_buffer);
    return true;
}
