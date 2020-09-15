#include "mock_algorithm.hpp"

#include <QDebug>

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
    frame.data["background"] = 0;
    frame.data["transform"]  = transform_;
    emit frameReady (frame);
}
