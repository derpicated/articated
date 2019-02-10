#include "vision.hpp"

#include <QTemporaryFile>
#include <iomanip>
#include <iostream>
#include <sstream>

vision::vision (QStatusBar& statusbar, augmentation_widget& augmentation, QObject* parent)
: QObject (parent)
, _opengl_context ()
, _acquisition (this)
, _vision_algorithm (NULL)
, _cam (new QCamera (QCamera::BackFace))
, _video_player (NULL)
, _statusbar (statusbar)
, _augmentation (augmentation)
, _failed_frames_counter (0) {
    _cam->setViewfinder (&_acquisition);
    connect (&_acquisition, SIGNAL (frameAvailable (const QVideoFrame&)), this,
    SLOT (frame_callback (const QVideoFrame&)));
    _cam->start ();
}

vision::~vision () {
    delete _vision_algorithm;
}

void vision::initialize_opengl () {
    _opengl_context.setShareContext (_augmentation.context ());
    _opengl_context.create ();

    _vision_algorithm = new algorithm_original (_opengl_context, _augmentation);
}

int vision::get_and_clear_failed_frame_count () {
    int ret                = _failed_frames_counter;
    _failed_frames_counter = 0;
    return ret;
}

QStringList vision::algorithm_list () {
    QStringList algorithms{ "Original (CPU)", "Random Movement" };

    return algorithms;
}

void vision::set_algorithm (int idx) {
    if (_vision_algorithm != NULL) {
        delete _vision_algorithm;
    }

    switch (idx) {
        case 1: {
            _vision_algorithm = new algorithm_original (_opengl_context, _augmentation);
            break;
        }
        case 2: {
            _vision_algorithm = new algorithm_random (_opengl_context, _augmentation);
            break;
        }
        default: {
            _vision_algorithm = new algorithm_original (_opengl_context, _augmentation);
            break;
        }
    }
}

int vision::max_debug_level () {
    return _vision_algorithm->max_debug_level ();
}

void vision::set_debug_level (const int& new_level) {
    _vision_algorithm->set_debug_level (new_level);
}

int vision::debug_level () {
    return _vision_algorithm->debug_level ();
}

void vision::set_input (const QCameraInfo& cameraInfo) {
    if (_video_player != NULL) {
        disconnect (_video_player, SIGNAL (mediaStatusChanged (QMediaPlayer::MediaStatus)),
        this, SLOT (video_player_status_changed (QMediaPlayer::MediaStatus)));
        delete _video_player;
        _video_player = NULL;
    }
    if (_cam != NULL) {
        delete _cam;
        _cam = NULL;
    }

    _cam = new QCamera (cameraInfo);
    _cam->setViewfinder (&_acquisition);
    _cam->start ();
    if (_cam->status () != QCamera::ActiveStatus) {
        _statusbar.showMessage (QString ("camera status %1").arg (_cam->status ()), 2000);
    }
}

void vision::set_input (const QString& resource_path) {
    QFile resource_file (resource_path);
    if (resource_file.exists ()) {
        auto temp_file  = QTemporaryFile::createNativeFile (resource_file);
        QString fs_path = temp_file->fileName ();

        if (!fs_path.isEmpty ()) {
            if (_cam != NULL) {
                delete _cam;
                _cam = NULL;
            }
            if (_video_player != NULL) {
                disconnect (_video_player,
                SIGNAL (mediaStatusChanged (QMediaPlayer::MediaStatus)), this,
                SLOT (video_player_status_changed (QMediaPlayer::MediaStatus)));
                delete _video_player;
                _video_player = NULL;
            }

            _video_player = new QMediaPlayer ();
            connect (_video_player,
            SIGNAL (mediaStatusChanged (QMediaPlayer::MediaStatus)), this,
            SLOT (video_player_status_changed (QMediaPlayer::MediaStatus)));
            _video_player->setVideoOutput (&_acquisition);
            _video_player->setMedia (QUrl::fromLocalFile (fs_path));
            _video_player->play ();
        }
    }
}

void vision::video_player_status_changed (QMediaPlayer::MediaStatus new_status) {
    // this function simply starts playing the video again, if it ended
    if (new_status == QMediaPlayer::EndOfMedia) {
        if (_video_player != NULL) {
            _video_player->play ();
        }
    }
}

void vision::set_paused (bool paused) {
    if (paused) {
        disconnect (&_acquisition, SIGNAL (frameAvailable (const QVideoFrame&)),
        this, SLOT (frame_callback (const QVideoFrame&)));
    } else {
        connect (&_acquisition, SIGNAL (frameAvailable (const QVideoFrame&)),
        this, SLOT (frame_callback (const QVideoFrame&)));
    }
}

void set_focus () {
    ; // TODO: add focus implementation
}

void vision::set_reference () {
    _vision_mutex.lock ();
    try {
        _vision_algorithm->set_reference ();
    } catch (const std::exception& e) {
        _statusbar.showMessage ("Error getting reference");
    }
    _vision_mutex.unlock ();
}

void vision::frame_callback (const QVideoFrame& const_buffer) {
    if (_vision_mutex.tryLock ()) {
        try {
            movement3d movement = _vision_algorithm->execute (const_buffer);

            _augmentation.setScale (movement.scale ());
            _augmentation.setXPosition (movement.translation ().x);
            _augmentation.setYPosition (movement.translation ().y);

            _augmentation.setYRotation (movement.yaw ());
            _augmentation.setZRotation (movement.roll ());
            _augmentation.setXRotation ((movement.pitch ()) - 90);

            std::stringstream stream;
            stream << std::setprecision (2);
            // stream << "T(" << movement.translation ().x << ","
            //        << movement.translation ().y << ") ";
            stream << "S: " << movement.scale () << " ";
            stream << "yaw: " << movement.yaw () << " ";
            stream << "pitch: " << movement.pitch () << " ";
            stream << "roll: " << movement.roll () << std::endl;
            _statusbar.showMessage (stream.str ().c_str ());

            _augmentation.update ();
        } catch (const std::exception& e) {
            _statusbar.showMessage ("Error in execution");
        }
        _vision_mutex.unlock ();
    } else {
        _failed_frames_counter++;
    }
}
