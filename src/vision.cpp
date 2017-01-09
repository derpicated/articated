#include "vision.hpp"

vision::vision (QObject* parent)
: QObject (parent)
, _failed_frames_counter (0)
, _cam ()
, _cam_cap (&_cam) {
    connect (&_cam_cap, SIGNAL (imageAvailable (int, const QVideoFrame&)), this,
    SLOT (frame_callback (int, const QVideoFrame&)));
}

void vision::set_debug_mode (int mode) {
    _debug_mode = mode;
}
void vision::set_input () {
    ; // TODO: add implementation for selecting inputs, camera or file
}

void set_focus () {
    ; // TODO: add focus implementation
}

void vision::execute_frame () {
    if (_cam_cap.isReadyForCapture ()) {
        _cam_cap.capture ();
    } else {
        ++_failed_frames_counter;
    }
}

void vision::frame_callback (int id, const QVideoFrame& buffer) {
    ;
}
