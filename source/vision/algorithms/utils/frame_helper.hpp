#ifndef FRAMEHELPER_HPP
#define FRAMEHELPER_HPP

#ifdef ANDROID
#include <GLES3/gl3.h>
#endif

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>
#include <QVideoFrame>
#include <optional>
#include <utility>

#include "image.hpp"
#include "shared/frame_data.hpp"
#include "shared/movement3d/movement3d.hpp"


class FrameHelper : protected QOpenGLExtraFunctions {
    public:
    FrameHelper ();
    ~FrameHelper ();

    std::optional<GLuint> FrameToTexture (const QVideoFrame& const_buffer);
    bool FrameToRam (const QVideoFrame& const_buffer, image_t& image);
    bool FrameToRam (const QVideoFrame& const_buffer,
    image_t& image,
    bool output_to_texture,
    GLuint& output_texture);

    void DownloadImage (image_t& image, GLuint handle);
    GLuint UploadImage (image_t image, bool& is_grayscale);

    void SetBackground (image_t image);

    private:
    GLuint framebuffer_download_;
    GLuint frame_texture_;
};


#endif // FRAMEHELPER_HPP
