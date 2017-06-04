#include "vision.hpp"
#include "operators.hpp"

#include <QTemporaryFile>
#include <iomanip>
#include <iostream>
#include <sstream>

vision::vision (QStatusBar& statusbar, augmentation_widget& augmentation, QObject* parent)
: QObject (parent)
, _movement3d_average (1)
, _failed_frames_counter (0)
, _debug_level (0)
, _augmentation (augmentation)
, _cam (new QCamera (QCamera::BackFace))
, _video_player (NULL)
, _acquisition (this)
, _operators ()
, _statusbar (statusbar) {
    _cam->setViewfinder (&_acquisition);
    connect (&_acquisition, SIGNAL (frameAvailable (const QVideoFrame&)), this,
    SLOT (frame_callback (const QVideoFrame&)));
    _cam->start ();
}

int vision::get_and_clear_failed_frame_count () {
    int ret                = _failed_frames_counter;
    _failed_frames_counter = 0;
    return ret;
}

int vision::max_debug_level () {
    return _max_debug_level;
}

void vision::set_debug_level (const int level) {
    _debug_level = level;
}

int vision::debug_level () {
    return _debug_level;
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
    _markers_mutex.lock ();
    _reference = _markers;
    _markers_mutex.unlock ();
}

void vision::frame_callback (const QVideoFrame& const_buffer) {
    bool status = true;
    image_t image;

    if (_vision_mutex.tryLock ()) {
        if (const_buffer.isValid ()) {
            // copy image into cpu memory
            switch (const_buffer.handleType ()) {
                case QAbstractVideoBuffer::NoHandle: {
                    // if the frame can be mapped
                    QVideoFrame frame (const_buffer);
                    if (frame.map (QAbstractVideoBuffer::ReadOnly)) {
                        image.data = (uint8_t*)malloc (frame.mappedBytes ());
                        memcpy (image.data, frame.bits (), frame.mappedBytes ());

                        if (frame.pixelFormat () == QVideoFrame::Format_RGB24) {
                            image.format = RGB24;
                        } else if (frame.pixelFormat () == QVideoFrame::Format_YUV420P) {
                            image.format = YUV;
                        } else {
                            status = false;
                            delete image.data;
                            _statusbar.showMessage (
                            QString ("unsuported format %1").arg (frame.pixelFormat ()), 2000);
                        }
                    } else {
                        status = false;
                    }
                    frame.unmap ();
                    if (status) {
                        image.width  = frame.width ();
                        image.height = frame.height ();
                        if (_debug_level == 0) {
                            _augmentation.setBackground (image);
                        }
                    }
                    break;
                }
                case QAbstractVideoBuffer::GLTextureHandle: {
                    // if the frame is an OpenGL texture
                    QVideoFrame::PixelFormat format = const_buffer.pixelFormat ();

                    if (format == QVideoFrame::Format_BGR32 ||
                    format == QVideoFrame::Format_RGB24) {
                        size_t pixelsize;
                        if (format == QVideoFrame::Format_BGR32) {
                            pixelsize    = 4;
                            image.format = BGR32;
                        } else {
                            pixelsize    = 3;
                            image.format = RGB24;
                        }
                        image.width  = const_buffer.width ();
                        image.height = const_buffer.height ();
                        image.data =
                        (uint8_t*)malloc (image.width * image.height * pixelsize);

                        QVariant tex_name = const_buffer.handle ();
                        if (_debug_level == 0) {
                            _augmentation.setBackground (tex_name.toUInt ());
                        }
                        _augmentation.downloadImage (image, tex_name.toUInt ());
                    } else {
                        _statusbar.showMessage (
                        QString ("unsuported format %1").arg (const_buffer.pixelFormat ()), 2000);
                    }
                    break;
                }
                default: {
                    // if the frame is unsupported by articated
                    _statusbar.showMessage (
                    QString ("unsuported framehandle %1").arg (const_buffer.handleType ()), 2000);
                    status = false;
                    break;
                }
            }
        } else {
            status = false;
        }

        if (status) {
            execute_processing (image);
            _augmentation.update ();
            free (image.data);
        }
        _vision_mutex.unlock ();
    } else {
        _failed_frames_counter++;
    }
}

void vision::execute_processing (image_t image) {
    // start image processing
    _operators.preprocessing (image);
    if (_debug_level == 1) {
        _augmentation.setBackground (image);
    }

    QImage debug_image ((const unsigned char*)image.data, image.width,
    image.height, QImage::Format_Grayscale8);
    debug_image.save ("debug_image.png");

    _operators.segmentation (image);
    if (_debug_level == 2) {
        _augmentation.setBackground (image);
    }

    _markers_mutex.lock ();
    _markers.clear ();
    _operators.extraction (image, _markers);
    if (_debug_level == 3) {
        _augmentation.setBackground (image);
    }

    movement3d movement;
    bool clasified = _operators.classification (_reference, _markers, movement); // classify
    if (clasified) {
        movement = _movement3d_average.average (movement);
        _augmentation.setScale (movement.scale ());
        translation_t translation = movement.translation ();
        movement.translation (
        { movement.translation_delta_to_absolute (translation.x, image.width, -1, 1),
        movement.translation_delta_to_absolute (translation.y, image.height, -1, 1) });
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
    } else {
        _statusbar.showMessage ("No markers! You idiot...");
    }
    _markers_mutex.unlock ();
}
