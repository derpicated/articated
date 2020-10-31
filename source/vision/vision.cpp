#include "vision.hpp"

#include <QResource>
#include <QTemporaryFile>
#include <iomanip>
#include <iostream>
#include <sstream>

Vision::Vision ()
: acquisition_ (nullptr)
, camera_ (new QCamera (QCamera::BackFace)) {
    camera_->setViewfinder (&acquisition_);
    connect (&acquisition_, &Acquisition::FrameAvailable, this, &Vision::FrameCallback);
    camera_->start ();

    InitializeOpenGL ();
}

Vision::~Vision () {
    delete vision_algorithm_;
}

void Vision::InitializeOpenGL () {
    dummy_surface_.create ();
    opengl_context_.setShareContext (QOpenGLContext::globalShareContext ());
    opengl_context_.create ();

    SetAlgorithm (-1);
}

int Vision::GetAndClearFailedFrameCount () {
    int ret                = failed_frames_counter_;
    failed_frames_counter_ = 0;
    return ret;
}

void Vision::SetAlgorithm (int idx) {
    opengl_context_.makeCurrent (&dummy_surface_);
    selected_algorithm_ = idx;
    if (vision_algorithm_ != NULL) {
        delete vision_algorithm_;
    }

    switch (idx) {
        case 0: {
            vision_algorithm_ = new AlgorithmOriginal ();
            break;
        }
        default:
        case 1: {
            vision_algorithm_ = new AlgorithmGpu ();
            break;
        }
        case 2: {
            vision_algorithm_ = new AlgorithmRandom ();
            break;
        }
    }
    opengl_context_.doneCurrent ();
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

void Vision::SetSource (const QString& source) {
    if (QResource (source).isValid ()) {
        SetSourceVideo (source);
    } else {
        SetSourceCamera (source);
    }
}

void Vision::SetSourceCamera (const QString& camera_device) {
    if (video_player_ != NULL) {
        delete video_player_;
        video_player_ = NULL;
    }
    if (camera_ != NULL) {
        delete camera_;
        camera_ = NULL;
    }

    camera_ = new QCamera (camera_device.toLocal8Bit ());
    camera_->setViewfinder (&acquisition_);
    camera_->start ();
    if (camera_->status () != QCamera::ActiveStatus) {
        qDebug () << "camera status" << camera_->status ();
    }
}

void Vision::SetSourceVideo (const QString& resource_path) {
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
                delete video_player_;
                video_player_ = NULL;
            }

            video_player_ = new QMediaPlayer ();
            connect (video_player_, &QMediaPlayer::mediaStatusChanged, this,
            &Vision::VideoPlayerStatusChanged);
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
        disconnect (&acquisition_, &Acquisition::FrameAvailable, this, &Vision::FrameCallback);
    } else {
        connect (&acquisition_, &Acquisition::FrameAvailable, this, &Vision::FrameCallback);
    }

    is_paused_ = paused;
    emit isPausedChanged ();
}

void SetFocus () {
    ; // TODO: add focus implementation
}

void Vision::SetReference () {
    vision_mutex_.lock ();
    try {
        vision_algorithm_->SetReference ();
    } catch (const std::exception& e) {
        qDebug () << "Error getting reference";
    }
    vision_mutex_.unlock ();
}

void Vision::FrameCallback (const QVideoFrame& const_buffer) {
    if (vision_mutex_.tryLock ()) {
        try {
            opengl_context_.makeCurrent (&dummy_surface_);
            FrameData frame_data = vision_algorithm_->Execute (const_buffer);
            opengl_context_.doneCurrent ();
            emit frameProcessed (frame_data);
        } catch (const std::exception& e) {
            qDebug () << "Error in execution";
        }
        vision_mutex_.unlock ();
    } else {
        failed_frames_counter_++;
    }
}
