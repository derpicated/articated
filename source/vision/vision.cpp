#include "vision.hpp"

#include <QTemporaryFile>
#include <iomanip>
#include <iostream>
#include <sstream>

Vision::Vision (QStatusBar& statusbar, AugmentationWidget& augmentation, QObject* parent)
: QObject (parent)
, opengl_context_ ()
, acquisition_ (this)
, vision_algorithm_ (NULL)
, camera_ (new QCamera (QCamera::BackFace))
, video_player_ (NULL)
, statusbar_ (statusbar)
, augmentation_ (augmentation)
, failed_frames_counter_ (0) {
    camera_->setViewfinder (&acquisition_);
    connect (&acquisition_, SIGNAL (FrameAvailable (const QVideoFrame&)), this,
    SLOT (FrameCallback (const QVideoFrame&)));
    camera_->start ();
}

Vision::~Vision () {
    delete vision_algorithm_;
}

void Vision::InitializeOpenGL () {
    opengl_context_.setShareContext (augmentation_.context ());
    opengl_context_.create ();
    SetAlgorithm (0);
}

int Vision::GetAndClearFailedFrameCount () {
    int ret                = failed_frames_counter_;
    failed_frames_counter_ = 0;
    return ret;
}

QStringList Vision::AlgorithmList () {
    QStringList algorithms{ "Original (CPU)", "Original (GPU)", "Random Movement" };

    return algorithms;
}

void Vision::SetAlgorithm (int idx) {
    if (vision_algorithm_ != NULL) {
        delete vision_algorithm_;
    }

    switch (idx) {
        case 1: {
            vision_algorithm_ = new AlgorithmOriginal (opengl_context_);
            break;
        }
        default:
        case 2: {
            vision_algorithm_ = new AlgorithmGpu (opengl_context_);
            break;
        }
        case 3: {
            vision_algorithm_ = new AlgorithmRandom (opengl_context_);
            break;
        }
    }
}

int Vision::MaxDebugLevel () {
    return vision_algorithm_->MaxDebugLevel ();
}

void Vision::SetDebugLevel (const int& new_level) {
    vision_algorithm_->SetDebugLevel (new_level);
}

int Vision::DebugLevel () {
    return vision_algorithm_->DebugLevel ();
}

void Vision::SetInput (const QCameraInfo& cameraInfo) {
    if (video_player_ != NULL) {
        disconnect (video_player_, SIGNAL (mediaStatusChanged (QMediaPlayer::MediaStatus)),
        this, SLOT (VideoPlayerStatusChanged (QMediaPlayer::MediaStatus)));
        delete video_player_;
        video_player_ = NULL;
    }
    if (camera_ != NULL) {
        delete camera_;
        camera_ = NULL;
    }

    camera_ = new QCamera (cameraInfo);
    camera_->setViewfinder (&acquisition_);
    camera_->start ();
    if (camera_->status () != QCamera::ActiveStatus) {
        statusbar_.showMessage (QString ("camera status %1").arg (camera_->status ()), 2000);
    }
}

void Vision::SetInput (const QString& resource_path) {
    QFile resource_file (resource_path);
    if (resource_file.exists ()) {
        auto temp_file  = QTemporaryFile::createNativeFile (resource_file);
        QString fs_path = temp_file->fileName ();

        if (!fs_path.isEmpty ()) {
            if (camera_ != NULL) {
                delete camera_;
                camera_ = NULL;
            }
            if (video_player_ != NULL) {
                disconnect (video_player_,
                SIGNAL (mediaStatusChanged (QMediaPlayer::MediaStatus)), this,
                SLOT (VideoPlayerStatusChanged (QMediaPlayer::MediaStatus)));
                delete video_player_;
                video_player_ = NULL;
            }

            video_player_ = new QMediaPlayer ();
            connect (video_player_,
            SIGNAL (mediaStatusChanged (QMediaPlayer::MediaStatus)), this,
            SLOT (VideoPlayerStatusChanged (QMediaPlayer::MediaStatus)));
            video_player_->setVideoOutput (&acquisition_);
            video_player_->setMedia (QUrl::fromLocalFile (fs_path));
            video_player_->play ();
        }
    }
}

void Vision::VideoPlayerStatusChanged (QMediaPlayer::MediaStatus new_status) {
    // this function simply starts playing the video again, if it ended
    if (new_status == QMediaPlayer::EndOfMedia) {
        if (video_player_ != NULL) {
            video_player_->play ();
        }
    }
}

void Vision::SetPaused (bool paused) {
    if (paused) {
        disconnect (&acquisition_, SIGNAL (frameAvailable (const QVideoFrame&)),
        this, SLOT (FrameCallback (const QVideoFrame&)));
    } else {
        connect (&acquisition_, SIGNAL (frameAvailable (const QVideoFrame&)),
        this, SLOT (FrameCallback (const QVideoFrame&)));
    }
}

void SetFocus () {
    ; // TODO: add focus implementation
}

void Vision::SetReference () {
    vision_mutex_.lock ();
    try {
        vision_algorithm_->SetReference ();
    } catch (const std::exception& e) {
        statusbar_.showMessage ("Error getting reference");
    }
    vision_mutex_.unlock ();
}

void Vision::FrameCallback (const QVideoFrame& const_buffer) {
    if (vision_mutex_.tryLock ()) {
        try {
            FrameData frame_data = vision_algorithm_->Execute (const_buffer);
            emit FrameProcessed (frame_data);
        } catch (const std::exception& e) {
            statusbar_.showMessage ("Error in execution");
        }
        vision_mutex_.unlock ();
    } else {
        failed_frames_counter_++;
    }
}
