#pragma once

#ifdef ANDROID
#include <GLES3/gl3.h>
#endif

#include <QOpenGLTexture>
#include <QVideoFrame>
#include <optional>
#include <utility>

#include "../image.hpp"

class FrameHelperInterface {
    public:
    virtual ~FrameHelperInterface () = default;

    virtual std::optional<GLuint> FrameToTexture (const QVideoFrame& const_buffer) = 0;
    virtual bool FrameToRam (const QVideoFrame& const_buffer, image_t& image) = 0;
    virtual bool FrameToRam (const QVideoFrame& const_buffer,
    image_t& image,
    bool output_to_texture,
    GLuint& output_texture) = 0;

    virtual void DownloadImage (image_t& image, GLuint handle)     = 0;
    virtual GLuint UploadImage (image_t image, bool& is_grayscale) = 0;
};

std::unique_ptr<FrameHelperInterface> CreateFrameHelper ();
