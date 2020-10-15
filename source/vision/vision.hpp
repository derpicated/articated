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
#include <QOffscreenSurface>
#include <QStatusBar>
#include <QStringList>
#include <QVideoFrame>

#include "acquisition.hpp"
#include "algorithms/gpu/algorithm_gpu.hpp"
#include "algorithms/original/algorithm_original.hpp"
#include "algorithms/random/algorithm_random.hpp"
#include "algorithms/vision_algorithm.hpp"
#include "shared/movement3d/movement3d.hpp"

class Vision : public QObject {
    Q_OBJECT

    Q_PROPERTY (QStringList algorithms MEMBER algorithms_ NOTIFY algorithmsChanged)
    Q_PROPERTY (int algorithm MEMBER selected_algorithm_ WRITE SetAlgorithm NOTIFY algorithmChanged)
    Q_PROPERTY (bool isPaused MEMBER is_paused_ WRITE SetPaused NOTIFY isPausedChanged)

    public:
    Vision ();
    ~Vision ();

    void SetAlgorithm (int idx);
    QStringList AlgorithmList ();
    int MaxDebugLevel ();
    void SetDebugLevel (const int& level);
    int DebugLevel ();
    void SetInput (const QCameraInfo& cameraInfo);
    void SetFocus ();
    void SetPaused (bool paused);

    public slots:
    void SetInput (const QString& resource_path);
    void SetReference ();
    int GetAndClearFailedFrameCount ();
    void VideoPlayerStatusChanged (QMediaPlayer::MediaStatus new_status);
    void FrameCallback (const QVideoFrame& const_buffer);

    signals:
    void algorithmsChanged ();
    void algorithmChanged ();
    void isPausedChanged ();

    void frameProcessed (const FrameData framedata);

    private:
    void InitializeOpenGL ();

    QOffscreenSurface dummy_surface_;
    QOpenGLContext opengl_context_;
    Acquisition acquisition_;
    VisionAlgorithm* vision_algorithm_{ nullptr };
    QCamera* camera_;
    QMediaPlayer* video_player_{ nullptr };
    QMutex vision_mutex_;
    QAtomicInteger<int> failed_frames_counter_{ 0 };
    QStringList algorithms_{ "Original (CPU)", "Original (GPU)", "Random Movement" };
    int selected_algorithm_;
    bool is_paused_{ false };
};

#endif // VISION_HPP
