#include "vision.hpp"
#include "operators.hpp"

#include <QTemporaryFile>
#include <iostream>

vision::vision (QStatusBar& statusbar, augmentation_widget& augmentation, QObject* parent)
: QObject (parent)
, _failed_frames_counter (0)
, _debug_mode (0)
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

void vision::set_debug_mode (const int mode) {
    _debug_mode = mode;
}
void vision::set_input (const QCameraInfo& cameraInfo) {
    if (_video_player != NULL) {
        delete _video_player;
    }
    _video_player = NULL;
    if (_cam != NULL) {
        delete _cam;
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
            }
            _cam = NULL;
            if (_video_player != NULL) {
                delete _video_player;
            }

            _video_player = new QMediaPlayer ();
            _video_player->setVideoOutput (&_acquisition);
            _video_player->setMedia (QUrl::fromLocalFile (fs_path));
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

void vision::frame_callback (const QVideoFrame& const_buffer) {
    bool status = true;
    image_t image;
    if (const_buffer.isValid ()) {
        // copy image into cpu memory
        QVideoFrame frame (const_buffer);
        if (frame.map (QAbstractVideoBuffer::ReadOnly)) {
            image.data = (uint8_t*)malloc (frame.mappedBytes ());
            memcpy (image.data, frame.bits (), frame.mappedBytes ());

            if (frame.pixelFormat () == QVideoFrame::Format_RGB24) {
                image.format = RGB24;
                image.width  = frame.width ();
                image.height = frame.height ();
            } else if (frame.pixelFormat () == QVideoFrame::Format_YUV420P) {
                image.format = YUV;
                image.width  = frame.width ();
                image.height = frame.height ();
            } else {
                _statusbar.showMessage (
                QString ("unsuported format %1").arg (frame.pixelFormat ()), 2000);
            }
        } else {
            status = false;
        }
        frame.unmap ();
    }

    if (status) {
        if (_debug_mode == 0) {
            _augmentation.setBackground (image);
            _augmentation.update ();
        }
        // start image processing
        _operators.preprocessing (image);
        if (_debug_mode == 1) {
            _augmentation.setBackground (image);
            _augmentation.update ();
        }

        _operators.segmentation (image);
        if (_debug_mode == 2) {
            _augmentation.setBackground (image);
            _augmentation.update ();
        }

        _operators.extraction (image);
        if (_debug_mode == 3) {
            _augmentation.setBackground (image);
            _augmentation.update ();
        }

        QImage debug_image ((const unsigned char*)image.data, image.width,
        image.height, QImage::Format_Grayscale8);
        debug_image.save ("debug_image.png");

        delete image.data;
    }
}
