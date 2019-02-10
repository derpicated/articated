#include "vision_algorithm.hpp"

vision_algorithm::vision_algorithm (const int& max_debug_level, augmentation_widget& augmentation)
: _augmentation (augmentation)
, _max_debug_level (max_debug_level)
, _debug_level (0) {
}

int vision_algorithm::max_debug_level () {
    return _max_debug_level;
}

void vision_algorithm::set_debug_level (const int& new_level) {
    int level    = new_level;
    level        = level < 0 ? 0 : level;
    level        = level > _max_debug_level ? _max_debug_level : level;
    _debug_level = level;
}

int vision_algorithm::debug_level () {
    return _debug_level;
}

bool vision_algorithm::frame_to_ram (const QVideoFrame& const_buffer, image_t& image) {
    bool status = true;

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
                        //_statusbar.showMessage (QString ("unsuported format
                        //%1").arg (frame.pixelFormat ()), 2000);
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

                if (format == QVideoFrame::Format_BGR32 || format == QVideoFrame::Format_RGB24) {
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
                    image.data = (uint8_t*)malloc (image.width * image.height * pixelsize);

                    QVariant tex_name = const_buffer.handle ();
                    if (_debug_level == 0) {
                        _augmentation.setBackground (tex_name.toUInt ());
                    }
                    _augmentation.downloadImage (image, tex_name.toUInt ());
                } else {
                    //_statusbar.showMessage (QString ("unsuported format
                    //%1").arg (const_buffer.pixelFormat ()), 2000);
                }
                break;
            }
            default: {
                // if the frame is unsupported by articated
                //_statusbar.showMessage (QString ("unsuported framehandle
                //%1").arg (const_buffer.handleType ()), 2000);
                status = false;
                break;
            }
        }
    } else {
        status = false;
    }
    return status;
}
