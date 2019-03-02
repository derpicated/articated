// vision.hpp

#ifndef VISION_HPP
#define VISION_HPP

#include <QAtomicInteger>
#include <QCamera>
#include <QCameraImageCapture>
#include <QCameraInfo>
#include <QMediaObject>
#include <QMediaPlayer>
#include <QMutex>
#include <QStatusBar>
#include <QStringList>
#include <QVideoFrame>

#include "acquisition.hpp"
#include "algorithms/algorithm_gpu/algorithm_gpu.hpp"
#include "algorithms/algorithm_original.hpp"
#include "algorithms/algorithm_random.hpp"
#include "algorithms/vision_algorithm.hpp"
#include "augmentation_widget/augmentation_widget.hpp"
#include "movement3d/movement3d.hpp"

class vision : public QObject {
    Q_OBJECT

    public:
    vision (QStatusBar& statusbar, augmentation_widget& augmentation, QObject* parent);
    ~vision ();

    void set_algorithm (int idx);
    QStringList algorithm_list ();
    int max_debug_level ();
    void set_debug_level (const int& level);
    int debug_level ();
    void set_input (const QCameraInfo& cameraInfo);
    void set_input (const QString& resource_path);
    void set_paused (bool paused);
    void set_focus ();
    void set_reference ();

    public slots:
    void initialize_opengl ();
    int get_and_clear_failed_frame_count ();
    void video_player_status_changed (QMediaPlayer::MediaStatus new_status);
    void frame_callback (const QVideoFrame& const_buffer);

    private:
    QOpenGLContext _opengl_context;
    acquisition _acquisition;
    vision_algorithm* _vision_algorithm;
    QCamera* _cam;
    QMediaPlayer* _video_player;
    QStatusBar& _statusbar;
    augmentation_widget& _augmentation;
    QMutex _vision_mutex;
    QAtomicInteger<int> _failed_frames_counter;
};

#endif // VISION_HPP
