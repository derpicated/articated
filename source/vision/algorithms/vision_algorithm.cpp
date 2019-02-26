#include "vision_algorithm.hpp"

vision_algorithm::vision_algorithm (const int& max_debug_level,
QOpenGLContext& opengl_context,
augmentation_widget& augmentation)
: QOpenGLExtraFunctions (&opengl_context)
, _augmentation (augmentation)
, _opengl_context (opengl_context)
, _texture ()
, _max_debug_level (max_debug_level)
, _debug_level (0) {
    glGenTextures (1, &_texture);
    glBindTexture (GL_TEXTURE_2D, _texture);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glBindTexture (GL_TEXTURE_2D, 0);
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
                        set_background (image);
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
                        _augmentation.setBackground (tex_name.toUInt (), false);
                    }
                    download_image (image, tex_name.toUInt ());
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

bool vision_algorithm::frame_to_texture (const QVideoFrame& const_buffer,
GLuint& texture_handle,
GLuint& format) {
    bool status = true;

    if (const_buffer.isValid ()) {
        // copy image into cpu memory
        switch (const_buffer.handleType ()) {
            case QAbstractVideoBuffer::NoHandle: {
                // if the frame can be mapped
                QVideoFrame frame (const_buffer);
                if (frame.map (QAbstractVideoBuffer::ReadOnly)) {
                    GLuint internalformat;
                    glBindTexture (GL_TEXTURE_2D, _texture);

                    if (frame.pixelFormat () == QVideoFrame::Format_RGB24) {
                        internalformat = GL_RGB;
                        format         = GL_RGB;
                    } else if (frame.pixelFormat () == QVideoFrame::Format_YUV420P) {
                        internalformat = GL_R8;
                        format         = GL_RED;
                    } else {
                        status = false;
                    }

                    if (status) {
                        glTexImage2D (GL_TEXTURE_2D, 0, internalformat, frame.width (),
                        frame.height (), 0, format, GL_UNSIGNED_BYTE, frame.bits ());
                        texture_handle = _texture;
                    }
                    glBindTexture (GL_TEXTURE_2D, 0);
                } else {
                    status = false;
                }
                frame.unmap ();
                break;
            }
            case QAbstractVideoBuffer::GLTextureHandle: {
                // if the frame is an OpenGL texture
                QVideoFrame::PixelFormat frame_format = const_buffer.pixelFormat ();

                QVariant tex_name = const_buffer.handle ();
                printf ("Tex id: %d\n", tex_name.toInt ());
                if (frame_format == QVideoFrame::Format_BGR32 ||
                format == QVideoFrame::Format_RGB24) {
                    texture_handle = tex_name.toUInt ();
                    format         = GL_RGB;
                } else if (frame_format == QVideoFrame::Format_YUV420P) {
                    texture_handle = tex_name.toUInt ();
                    format         = GL_RED;
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


void vision_algorithm::set_background (image_t image) {
    bool is_grayscale;
    GLuint tex = _augmentation.getBackgroundTexture ();
    upload_image (image, tex, is_grayscale);
    _augmentation.setBackground (tex, is_grayscale);
}

void vision_algorithm::download_image (image_t& image, GLuint handle) {
    GLuint fbo;
    glGenFramebuffers (1, &fbo);
    GLuint prevFbo;
    glGetIntegerv (GL_FRAMEBUFFER_BINDING, (GLint*)&prevFbo);
    glBindFramebuffer (GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handle, 0);
    glReadPixels (
    0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
    glBindFramebuffer (GL_FRAMEBUFFER, prevFbo);
}

void vision_algorithm::upload_image (image_t image, GLint texture_handle, bool& is_grayscale) {
    bool status = true;
    GLint format_gl;
    GLint internalformat_gl;
    glBindTexture (GL_TEXTURE_2D, texture_handle);

    is_grayscale = 0;
    switch (image.format) {
        case RGB24: {
            format_gl         = GL_RGB;
            internalformat_gl = GL_RGB;
            break;
        }
        case YUV:
        case GREY8:
        case BINARY8: {
            internalformat_gl = GL_R8;
            format_gl         = GL_RED;
            is_grayscale      = 1;
            break;
        }
        case BGR32: {
            status = false;
            break;
        }
    }

    if (status) {
        glTexImage2D (GL_TEXTURE_2D, 0, internalformat_gl, image.width,
        image.height, 0, format_gl, GL_UNSIGNED_BYTE, image.data);
    }

    glBindTexture (GL_TEXTURE_2D, 0);
}
