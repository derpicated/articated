#include "mock_algorithm.hpp"

#include <QDebug>

MockAlgorithm::MockAlgorithm () {
    opengl_context_.setShareContext (QOpenGLContext::globalShareContext ());
    opengl_context_.create ();
    dummy_surface_.create ();
}

void MockAlgorithm::loadTexture () {
    if (!test_texture_) {
        opengl_context_.makeCurrent (&dummy_surface_);
        QImage image (":/debug_samples/textest.png");
        if (image.isNull ()) {
            qDebug () << "image unable to load";
            return;
        }
        test_texture_ = new QOpenGLTexture (image);
        composeFrame ();
        opengl_context_.doneCurrent ();
    }
}

void MockAlgorithm::setRotationX (const float value) {
    transform_.pitch (value);
    composeFrame ();
}

float MockAlgorithm::getRotationX () const {
    return transform_.pitch ();
}

void MockAlgorithm::setRotationY (const float value) {
    transform_.yaw (value);
    composeFrame ();
}

float MockAlgorithm::getRotationY () const {
    return transform_.yaw ();
}

void MockAlgorithm::setRotationZ (const float value) {
    transform_.roll (value);
    composeFrame ();
}

float MockAlgorithm::getRotationZ () const {
    return transform_.roll ();
}

void MockAlgorithm::setTranslationX (const float value) {
    transform_.translation ({ value, transform_.translation ().y });
    composeFrame ();
}

float MockAlgorithm::getTranslationX () const {
    return transform_.translation ().x;
}

void MockAlgorithm::setTranslationY (const float value) {
    transform_.translation ({ transform_.translation ().x, value });
    composeFrame ();
}

float MockAlgorithm::getTranslationY () const {
    return transform_.translation ().y;
}

void MockAlgorithm::setScale (const float value) {
    transform_.scale (value);
    composeFrame ();
}

float MockAlgorithm::getScale () const {
    return transform_.scale ();
}

void MockAlgorithm::composeFrame () {
    FrameData frame;
    if (test_texture_) {
        frame.data["background"] = static_cast<GLuint> (test_texture_->textureId ());
    } else {
        frame.data["background"] = static_cast<GLuint> (0);
    }
    frame.data["transform"] = transform_;
    emit frameReady (frame);
}
