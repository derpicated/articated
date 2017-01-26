// acquisition.hpp

#ifndef ACQUISITION_HPP
#define ACQUISITION_HPP

#include <QAbstractVideoSurface>
#include <QList>

class acquisition : public QAbstractVideoSurface {
    Q_OBJECT

    public:
    acquisition (QObject* parent);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats (
    QAbstractVideoBuffer::HandleType handleType) const;
    bool present (const QVideoFrame& frame);

    signals:
    void frameAvailable (const QVideoFrame& frame);
};

#endif // ACQUISITION_HPP
