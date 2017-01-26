#include "vision.hpp"
#include "operators.hpp"
#include <iostream>

vision::vision (augmentation_widget& augmentation, QObject* parent)
: QObject (parent)
, _failed_frames_counter (0)
, _debug_mode (0)
, _augmentation (augmentation)
, _cam (new QCamera (QCamera::BackFace))
, _acquisition (this) {
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
            memcpy (image.data, frame.bits (), frame.mappedBytes ());
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
        // call vision opperators here :D

        delete image.data;
    }
}

/*if (frame.isMapped ()) {
    if (initializedTexture) {
        context->context ()->makeCurrent ();
        glBindTexture (GL_TEXTURE_2D, texture);
        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, frame.width (),
        frame.height (), 0, GL_BGRA, GL_UNSIGNED_BYTE, frame.bits ());
    } else {
        QImage::Format format =
        frame.imageFormatFromPixelFormat (frame.pixelFormat ());
        QImage image = QImage (frame.bits (), frame.width (),
        frame.height (), frame.bytesPerLine (), format);
        context->context ()->makeCurrent ();
        texture            = context->bindTexture (image, GL_TEXTURE_2D);
        initializedTexture = true;
    }
}*/
