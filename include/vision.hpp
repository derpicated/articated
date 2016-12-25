// vision.hpp

#ifndef VISION_HPP
#define VISION_HPP

#include <QAbstractVideoBuffer>
#include <QCameraImageCapture>

class vision {
    Q_OBJECT

    public:
    vision ();
    ~vision ();

    void set_debug_mode (int mode);
    void set_input ();
    void set_focus ();

    public slots:
    void execute_frame ();
    void frame_callback (int id, const QVideoFrame& buffer);

    private:
    int _debug_mode;
    QCameraImageCapture _cam_cap;
};

#endif // VISION_HPP
