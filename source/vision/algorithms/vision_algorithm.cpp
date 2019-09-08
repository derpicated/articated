#include "vision_algorithm.hpp"

VisionAlgorithm::VisionAlgorithm (const int& max_debug_level,
QOpenGLContext& opengl_context,
augmentation_widget& augmentation)
: QOpenGLExtraFunctions (&opengl_context)
, augmentation_ (augmentation)
, opengl_context_ (opengl_context)
, texture_ ()
, max_debug_level_ (max_debug_level)
, debug_level_ (0) {
    dummy_surface_.create ();
    opengl_context_.makeCurrent (&dummy_surface_);
    initializeOpenGLFunctions ();
    // Generate the video frame texture
    glGenTextures (1, &texture_);
    glBindTexture (GL_TEXTURE_2D, texture_);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glBindTexture (GL_TEXTURE_2D, 0);

    // Generate the download buffer
    glGenFramebuffers (1, &framebuffer_download_);
    opengl_context_.doneCurrent ();
}

VisionAlgorithm::~VisionAlgorithm () {
    glDeleteTextures (1, &texture_);
    glDeleteFramebuffers (1, &framebuffer_download_);
}

int VisionAlgorithm::MaxDebugLevel () {
    return max_debug_level_;
}

void VisionAlgorithm::SetDebugLevel (const int& new_level) {
    int level    = new_level;
    level        = level < 0 ? 0 : level;
    level        = level > max_debug_level_ ? max_debug_level_ : level;
    debug_level_ = level;
}

int VisionAlgorithm::DebugLevel () {
    return debug_level_;
}

bool VisionAlgorithm::FrameToRam (const QVideoFrame& const_buffer, image_t& image) {
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
                    if (debug_level_ == 0) {
                        SetBackground (image);
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
                    if (debug_level_ == 0) {
                        augmentation_.setBackground (tex_name.toUInt (), false);
                    }
                    DownloadImage (image, tex_name.toUInt ());
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

bool VisionAlgorithm::FrameToTexture (const QVideoFrame& const_buffer,
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
                    glBindTexture (GL_TEXTURE_2D, texture_);

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
                        texture_handle = texture_;
                    }
                    glBindTexture (GL_TEXTURE_2D, 0);
                } else {
                    // Failed to map frame to memory
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
                    // Unsupported frame format
                    status = false;
                }
                break;
            }
            default: {
                // Unsupported handle type
                status = false;
                break;
            }
        }
    } else {
        status = false;
    }
    return status;
}


void VisionAlgorithm::SetBackground (image_t image) {
    bool is_grayscale;
    GLuint tex = augmentation_.getBackgroundTexture ();
    UploadImage (image, tex, is_grayscale);
    augmentation_.setBackground (tex, is_grayscale);
}

void VisionAlgorithm::DownloadImage (image_t& image, GLuint handle) {
    GLuint _previous_framebuffer;
    glGetIntegerv (GL_FRAMEBUFFER_BINDING, (GLint*)&_previous_framebuffer);
    glBindFramebuffer (GL_FRAMEBUFFER, framebuffer_download_);
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handle, 0);
    glReadPixels (
    0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
    glBindFramebuffer (GL_FRAMEBUFFER, _previous_framebuffer);
}

void VisionAlgorithm::UploadImage (image_t image, GLint texture_handle, bool& is_grayscale) {
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
