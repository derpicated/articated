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
#include "algorithms/gpu/algorithm_gpu.hpp"
#include "algorithms/original/algorithm_original.hpp"
#include "algorithms/random/algorithm_random.hpp"
#include "algorithms/vision_algorithm.hpp"
#include "augmentation_widget/augmentation_widget.hpp"
#include "shared/movement3d/movement3d.hpp"

class Vision : public QObject {
    Q_OBJECT

    public:
    Vision (QStatusBar& statusbar, AugmentationWidget& augmentation, QObject* parent);
    ~Vision ();

    void SetAlgorithm (int idx);
    QStringList AlgorithmList ();
    int MaxDebugLevel ();
    void SetDebugLevel (const int& level);
    int DebugLevel ();
    void SetInput (const QCameraInfo& cameraInfo);
    void SetInput (const QString& resource_path);
    void SetPaused (bool paused);
    void SetFocus ();
    void SetReference ();

    public slots:
    void InitializeOpenGL ();
    int GetAndClearFailedFrameCount ();
    void VideoPlayerStatusChanged (QMediaPlayer::MediaStatus new_status);
    void FrameCallback (const QVideoFrame& const_buffer);

    private:
    QOpenGLContext opengl_context_;
    Acquisition acquisition_;
    VisionAlgorithm* vision_algorithm_;
    QCamera* camera_;
    QMediaPlayer* video_player_;
    QStatusBar& statusbar_;
    AugmentationWidget& augmentation_;
    QMutex vision_mutex_;
    QAtomicInteger<int> failed_frames_counter_;
};

#endif // VISION_HPP
