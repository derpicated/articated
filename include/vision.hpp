// vision.hpp

#ifndef VISION_HPP
#define VISION_HPP

#include <QAbstractVideoBuffer>
#include <QCamera>
#include <QCameraImageCapture>
#include <QCameraInfo>
#include <QMediaObject>

#include "acquisition.hpp"
#include "augmentation_widget.hpp"

class vision : public QObject {
    Q_OBJECT

    public:
    vision (augmentation_widget& augmentation, QObject* parent);

    void set_debug_mode (const int mode);
    void set_input (const QCameraInfo& cameraInfo);
    void set_input (const QString& path);
    void set_focus ();

    public slots:
    void execute_frame ();
    void frame_callback (const QVideoFrame& const_buffer);

    private:
    int _failed_frames_counter;
    int _debug_mode;
    augmentation_widget& _augmentation;
    QCamera* _cam;
    acquisition _acquisition;
};

#endif // VISION_HPP
