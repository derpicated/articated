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
#include "algorithms/vision_algorithm.hpp"
#include "shared/movement3d/movement3d.hpp"

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
    QStringList AlgorithmList ();
    int MaxDebugLevel ();
    void SetDebugLevel (const int& level);
    int DebugLevel ();
    void SetFocus ();
    void SetPaused (bool paused);
    void SetSource (const QString& source);
    void SetSourceCamera (const QString& camera_device);
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

    void frameProcessed (const FrameData framedata);

    private:
    void InitializeOpenGL ();

    QOffscreenSurface dummy_surface_;
    QOpenGLContext opengl_context_;
    Acquisition acquisition_;
    VisionAlgorithm* vision_algorithm_{ nullptr };
    QCamera* camera_{ nullptr };
    QMediaPlayer* video_player_{ nullptr };
    QMutex vision_mutex_;
    QAtomicInteger<int> failed_frames_counter_{ 0 };
    QStringList algorithms_{ "Original (CPU)", "Original (GPU)", "Brute Force", "Random Movement" };
    int selected_algorithm_{ -1 };
    bool is_paused_{ false };
    QString source_{ "" };
};

#endif // VISION_HPP
