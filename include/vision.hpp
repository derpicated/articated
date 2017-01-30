// vision.hpp

#ifndef VISION_HPP
#define VISION_HPP

#include <QAbstractVideoBuffer>
#include <QCamera>
#include <QCameraImageCapture>
#include <QCameraInfo>
#include <QMediaObject>
#include <QMediaPlayer>
#include <QMutex>
#include <QStatusBar>

#include "acquisition.hpp"
#include "augmentation_widget.hpp"

class vision : public QObject {
    Q_OBJECT

    public:
    vision (QStatusBar& statusbar, augmentation_widget& augmentation, QObject* parent);

    void set_debug_mode (const int mode);
    void set_input (const QCameraInfo& cameraInfo);
    void set_input (const QString& resource_path);
    void set_paused (bool paused);
    void set_focus ();
    void set_reference ();

    public slots:
    void frame_callback (const QVideoFrame& const_buffer);

    private:
    points_t _markers;
    points_t _reference;
    int _failed_frames_counter;
    int _debug_mode;
    augmentation_widget& _augmentation;
    QCamera* _cam;
    QMediaPlayer* _video_player;
    acquisition _acquisition;
    operators _operators;
    QStatusBar& _statusbar;
    QMutex _markers_mutex;
};

#endif // VISION_HPP
