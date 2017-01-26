#include "vision.hpp"
#include "operators.hpp"
#include <iostream>

vision::vision (augmentation_widget& augmentation, QObject* parent)
: QObject (parent)
, _failed_frames_counter (0)
, _debug_mode (0)
, _augmentation (augmentation)
, _cam (new QCamera (QCamera::BackFace))
, _cam_cap (new QCameraImageCapture (_cam)) {
    _cam->setCaptureMode (QCamera::CaptureVideo);
    _cam->load ();
    _cam->start ();
    _cam->searchAndLock ();
    connect (_cam_cap, SIGNAL (imageAvailable (int, const QVideoFrame&)), this,
    SLOT (frame_callback (int, const QVideoFrame&)));
}

void vision::set_debug_mode (const int mode) {
    _debug_mode = mode;
}
void vision::set_input (const QCameraInfo& cameraInfo) {
    delete _cam;
    delete _cam_cap;

    _cam     = new QCamera (cameraInfo);
    _cam_cap = new QCameraImageCapture (_cam);

    _cam->setCaptureMode (QCamera::CaptureVideo);
    _cam->load ();
    _cam->start ();
    _cam->searchAndLock ();


    connect (_cam_cap, SIGNAL (imageAvailable (int, const QVideoFrame&)), this,
    SLOT (frame_callback (int, const QVideoFrame&)));
}

void vision::set_input (const QString& path) {
    // TODO: add implementation for selecting input file
}

void set_focus () {
    ; // TODO: add focus implementation
}

void vision::execute_frame () {
    if (_cam_cap != NULL) {
        if (_cam_cap->isReadyForCapture ()) {
            _cam_cap->capture ();
        } else {
            ++_failed_frames_counter;
            std::cout << _cam->status () << ", total failed frames: " << _failed_frames_counter
                      << std::endl;
        }
    }
}

void vision::frame_callback (int id, const QVideoFrame& const_buffer) {
    (void)id; // we dont need the ID
    image_t image;

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
        }
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
