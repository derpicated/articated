#include "acquisition.hpp"

#include <QDebug>

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
    qDebug () << "present!";
    emit FrameAvailable (const_buffer);
    return true;
}
