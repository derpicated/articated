#include "vision.hpp"
#include "operators.hpp"
#include <iostream>

vision::vision (QStatusBar& statusbar, augmentation_widget& augmentation, QObject* parent)
: QObject (parent)
, _failed_frames_counter (0)
, _debug_mode (0)
, _augmentation (augmentation)
, _cam (new QCamera (QCamera::BackFace))
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
    delete _cam;

    _cam = new QCamera (cameraInfo);
    //_acquisition = new acquisition ();
    //_cam->setCaptureMode (QCamera::CaptureViewfinder);
    //_cam->load ();
    //_cam->searchAndLock ();
    _cam->setViewfinder (&_acquisition);
    connect (&_acquisition, SIGNAL (frameAvailable (const QVideoFrame&)), this,
    SLOT (frame_callback (const QVideoFrame&)));
    _cam->start ();
}

void vision::set_input (const QString& path) {
    // TODO: add implementation for selecting input file
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
            image.width  = frame.width ();
            image.height = frame.height ();
            image.data   = (uint8_t*)malloc (frame.mappedBytes ());
            image.format = RGBA32;
            memcpy (image.data, frame.bits (), frame.mappedBytes ());
            _statusbar.showMessage (QString ("%1").arg (frame.fieldType ()), 2000);
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
        debug_image.save ("debug_image.jpg");
        /*QString path ("debug_image.png");
        QImageWriter writer (path);
        writer.write (debug_image);
*/
        delete image.data;
    }
}
