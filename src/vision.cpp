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

void vision::execute_frame () {
    std::cout << _cam->status () << std::endl;
    /*if () {
        _cam_cap->capture ();
        std::cout << "cam: " << _cam->status ()
                  << ", cap: " << _cam_cap->availability () << std::endl;
    } else {
        ++_failed_frames_counter;
        std::cout << _cam->status () << ", total failed frames: " <<
    _failed_frames_counter
                  << std::endl;
    }*/
}

void vision::frame_callback (const QVideoFrame& const_buffer) {
    image_t image;
    std::cout << "got em baws" << std::endl;
    if (const_buffer.isValid ()) {
        // copy image into cpu memory
        QVideoFrame frame (const_buffer);
        if (frame.map (QAbstractVideoBuffer::ReadOnly)) {
            image.data = malloc (frame.mappedBytes ());
            memcpy (image.data, frame.bits (), frame.mappedBytes ());
        }
        frame.unmap ();

        // start image processing
        if (_debug_mode == 0) {
            _augmentation.setBackground (image);
            _augmentation.update ();
        }
        // call vision opperators here :D
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
