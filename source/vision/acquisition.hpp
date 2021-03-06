// acquisition.hpp

#ifndef ACQUISITION_HPP
#define ACQUISITION_HPP

#include <QAbstractVideoBuffer>
#include <QAbstractVideoSurface>
#include <QList>
#include <QVideoFrame>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(visionAcquisitionLog)

class Acquisition : public QAbstractVideoSurface {
    Q_OBJECT

    public:
    Acquisition ();

    QList<QVideoFrame::PixelFormat> supportedPixelFormats (
    QAbstractVideoBuffer::HandleType handleType) const;
    bool present (const QVideoFrame& frame);

    signals:
    void FrameAvailable (const QVideoFrame& frame);
};

#endif // ACQUISITION_HPP
