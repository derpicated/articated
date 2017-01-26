// vision.hpp

#ifndef VISION_HPP
#define VISION_HPP

#include <QAbstractVideoBuffer>
#include <QCamera>
#include <QCameraImageCapture>
#include <QMediaObject>

#include "augmentation_widget.hpp"

class vision : public QObject {
    Q_OBJECT

    public:
    vision (augmentation_widget& augmentation, QObject* parent);

    void set_debug_mode (int mode);
    void set_input ();
    void set_focus ();

    public slots:
    void execute_frame ();
    void frame_callback (int id, const QVideoFrame& buffer);

    private:
    int _failed_frames_counter;
    int _debug_mode;
    augmentation_widget& _augmentation;
    QCamera _cam;
    QCameraImageCapture _cam_cap;
};

#endif // VISION_HPP