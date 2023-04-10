#include "frame_helper.hpp"

#include <QtMultimedia/QVideoFrame>
// TODO (articated #44) Remove private include when suitable replacement API has been found
#include <QtMultimedia/private/qabstractvideobuffer_p.h>

FrameHelper::FrameHelper ()
: QOpenGLExtraFunctions () {
    initializeOpenGLFunctions ();
    // Generate the video frame texture
    glGenTextures (1, &frame_texture_);
    glBindTexture (GL_TEXTURE_2D, frame_texture_);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glBindTexture (GL_TEXTURE_2D, 0);

    // Generate the download buffer
    glGenFramebuffers (1, &framebuffer_download_);
}

FrameHelper::~FrameHelper () {
    glDeleteTextures (1, &frame_texture_);
    glDeleteFramebuffers (1, &framebuffer_download_);
}

bool FrameHelper::FrameToRam (const QVideoFrame& const_buffer, image_t& image) {
    GLuint unused_text;
    return FrameToRam (const_buffer, image, false, unused_text);
}

bool FrameHelper::FrameToRam (const QVideoFrame& const_buffer,
image_t& image,
bool output_to_texture,
GLuint& output_texture) {
    bool status = true;
    QVideoFrame frame (const_buffer);
    if (!frame.map (QVideoFrame::MapMode::ReadOnly)) {
        // Could not map frame into CPU RAM!
        return false;
    }
    auto mapped_bytes = frame.mappedBytes (0);
    image.data        = (uint8_t*)malloc (mapped_bytes);
    memcpy (image.data, frame.bits (0), mapped_bytes);

    if (frame.pixelFormat () == QVideoFrameFormat::PixelFormat::Format_XRGB8888) {
        image.format = RGB24;
    } else if (frame.pixelFormat () == QVideoFrameFormat::PixelFormat::Format_XBGR8888) {
        image.format = BGR32;
    } else if (frame.pixelFormat () == QVideoFrameFormat::PixelFormat::Format_YUV420P) {
        image.format = YUV;
    } else {
        // Unsupported image format
        status = false;
        delete image.data;
    }

    frame.unmap ();
    if (status) {
        image.width  = frame.width ();
        image.height = frame.height ();
        if (output_to_texture) {
            bool unused_is_grayscale;
            output_texture = UploadImage (image, unused_is_grayscale);
        }
    }
    return status;
}

std::optional<GLuint>
FrameHelper::FrameToTexture (const QVideoFrame& const_buffer, bool& is_grayscale) {
    bool status = true;
    GLuint texture_handle;
    GLuint format;

    if (const_buffer.isValid ()) {
        // copy image into cpu memory
        switch (const_buffer.handleType ()) {
            case QVideoFrame::HandleType::NoHandle: {
                // if the frame can be mapped
                QVideoFrame frame (const_buffer);
                if (frame.map (QVideoFrame::MapMode::ReadOnly)) {
                    GLuint internalformat;
                    glBindTexture (GL_TEXTURE_2D, frame_texture_);

                    if (frame.pixelFormat () == QVideoFrameFormat::PixelFormat::Format_XRGB8888) {
                        is_grayscale   = false;
                        internalformat = GL_RGB;
                        format         = GL_RGB;
                    } else if (frame.pixelFormat () ==
                    QVideoFrameFormat::PixelFormat::Format_XBGR8888) {
                        is_grayscale   = false;
                        internalformat = GL_RGB;
                        format         = GL_ABGR_EXT;
                    } else if (frame.pixelFormat () ==
                    QVideoFrameFormat::PixelFormat::Format_YUV420P) {
                        is_grayscale   = true;
                        internalformat = GL_R8;
                        format         = GL_RED;
                    } else {
                        is_grayscale = false;
                        status       = false;
                    }

                    if (status) {
                        glTexImage2D (GL_TEXTURE_2D, 0, internalformat, frame.width (),
                        frame.height (), 0, format, GL_UNSIGNED_BYTE, frame.bits (0));
                        texture_handle = frame_texture_;
                    }
                    glBindTexture (GL_TEXTURE_2D, 0);
                } else {
                    // Failed to map frame to memory
                    status = false;
                }
                frame.unmap ();
                break;
            }
            case QVideoFrame::HandleType::RhiTextureHandle: {
                // if the frame is an OpenGL texture
                QVideoFrameFormat::PixelFormat frame_format = const_buffer.pixelFormat ();

                // TODO(articated #44) Remove usuage of private QAbstractVideoBuffer API
                //  once a suitable way to access the native OpenGL texture handle of a QVideoFrame has been found.
                auto tex_name = const_buffer.videoBuffer ()->textureHandle (0);
                if (frame_format == QVideoFrameFormat::PixelFormat::Format_XBGR8888 ||
                frame_format == QVideoFrameFormat::PixelFormat::Format_XRGB8888) {
                    texture_handle = tex_name;
                    format         = GL_RGB;
                } else if (frame_format == QVideoFrameFormat::PixelFormat::Format_YUV420P) {
                    texture_handle = tex_name;
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

    if (status) {
        return { texture_handle };
    } else {
        return std::nullopt;
    }
}

void FrameHelper::DownloadImage (image_t& image, GLuint handle) {
    GLuint _previous_framebuffer;
    glGetIntegerv (GL_FRAMEBUFFER_BINDING, (GLint*)&_previous_framebuffer);
    glBindFramebuffer (GL_FRAMEBUFFER, framebuffer_download_);
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handle, 0);
    glReadPixels (
    0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
    glBindFramebuffer (GL_FRAMEBUFFER, _previous_framebuffer);
}

GLuint FrameHelper::UploadImage (image_t image, bool& is_grayscale) {
    bool status = true;
    GLint format_gl;
    GLint internalformat_gl;
    glBindTexture (GL_TEXTURE_2D, frame_texture_);

    is_grayscale = false;
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
            is_grayscale      = true;
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

    return frame_texture_;
}
