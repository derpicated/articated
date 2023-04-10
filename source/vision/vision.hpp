#pragma once

#include <QAtomicInteger>
#include <QCamera>
#include <QLoggingCategory>
#include <QMediaCaptureSession>
#include <QMediaPlayer>
#include <QMutex>
#include <QOffscreenSurface>
#include <QStringList>
#include <QVideoFrame>
#include <QVideoSink>
#include <memory>

#include "algorithms/gpu/algorithm_gpu.hpp"
#include "algorithms/original/algorithm_original.hpp"
#include "algorithms/random/algorithm_random.hpp"
#include "shared/movement3d/movement3d.hpp"
#include "vision/algorithms/algorithm_interface.hpp"

Q_DECLARE_LOGGING_CATEGORY (visionLog)

class Vision : public QObject {
    Q_OBJECT

    Q_PROPERTY (QString source MEMBER source_ WRITE SetSource NOTIFY sourceChanged)
    Q_PROPERTY (QStringList algorithms MEMBER algorithms_ NOTIFY algorithmsChanged)
    Q_PROPERTY (int algorithm MEMBER selected_algorithm_ WRITE SetAlgorithm NOTIFY algorithmChanged)
    Q_PROPERTY (bool isPaused MEMBER is_paused_ WRITE SetPaused NOTIFY isPausedChanged)
    Q_PROPERTY (int debugLevel WRITE SetDebugLevel READ DebugLevel NOTIFY debugLevelChanged)
    Q_PROPERTY (int maxDebugLevel READ MaxDebugLevel NOTIFY maxDebugLevelChanged)

    public:
    Vision ();
    ~Vision ();

    void SetAlgorithm (int idx);
    int MaxDebugLevel ();
    void SetDebugLevel (const int& level);
    int DebugLevel ();
    void SetPaused (bool paused);
    void SetSource (const QString& source);
    void SetSourceCamera (const QString& camera_device_id);
    void SetSourceVideo (const QString& resource_path);

    public slots:
    void SetReference ();
    int GetAndClearFailedFrameCount ();
    void VideoPlayerStatusChanged (QMediaPlayer::MediaStatus new_status);
    void FrameCallback (const QVideoFrame& const_buffer);

    signals:
    void sourceChanged ();
    void algorithmsChanged ();
    void algorithmChanged ();
    void isPausedChanged ();
    void debugLevelChanged ();
    void maxDebugLevelChanged ();

    void frameProcessed (FrameData framedata);

    private:
    void InitializeOpenGL ();

    QOffscreenSurface dummy_surface_;
    QOpenGLContext opengl_context_;
    QVideoSink video_sink_;
    AlgorithmInterface* vision_algorithm_{ nullptr };
    QCamera* camera_{ nullptr };
    QMediaCaptureSession capture_session_;
    QMediaPlayer* video_player_{ nullptr };
    QMutex vision_mutex_;
    QAtomicInteger<int> failed_frames_counter_{ 0 };
    QStringList algorithms_{ "Original (CPU)", "Original (GPU)", "Random Movement" };
    int selected_algorithm_{ -1 };
    bool is_paused_{ false };
    QString source_{};
    QString default_camera_{};
};
