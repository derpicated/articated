#include "frame_data_lister.hpp"

#include <QDebug>
#include <QOpenGLContext>

const char* type_to_console (const std::type_info& type) {
    std::cout.flush ();
    system (("echo " + std::string (type.name ()) + " | c++filt -t").c_str ());
    return "";
}

void FrameDataLister::addFrameData (FrameData new_data) {
    FrameData framecopy = FrameData (new_data);
    auto& data_map      = new_data.data;
    Movement3D transform;
    GLuint background_texture;

    try {
        transform          = std::any_cast<Movement3D> (data_map["transform"]);
        background_texture = std::any_cast<GLuint> (data_map["background"]);
    } catch (const std::bad_any_cast& e) {
        std::cout << e.what () << std::endl;
    }

    QString data_string =
    QString ("tex: %0\t yaw: %1\t pitch: %2\t roll: %3")
    .arg (background_texture)
    .arg (static_cast<int> (transform.yaw ()), 3, 10, QChar ('0'))
    .arg (static_cast<int> (transform.pitch ()), 3, 10, QChar ('0'))
    .arg (static_cast<int> (transform.roll ()), 3, 10, QChar ('0'));

    data_.append (data_string);

    if (data_.size () > length_) {
        data_.pop_front ();
    }

    emit dataChanged ();
}
