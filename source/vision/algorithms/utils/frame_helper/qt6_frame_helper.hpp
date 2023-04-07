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

#include "frame_helper_interface.hpp"

class FrameHelper : public FrameHelperInterface, protected QOpenGLExtraFunctions {
    public:
    FrameHelper ();
    virtual ~FrameHelper ();

    std::optional<GLuint> FrameToTexture (const QVideoFrame& const_buffer) final;
    bool FrameToRam (const QVideoFrame& const_buffer, image_t& image) final;
    bool FrameToRam (const QVideoFrame& const_buffer,
    image_t& image,
    bool output_to_texture,
    GLuint& output_texture) final;

    void DownloadImage (image_t& image, GLuint handle) final;
    GLuint UploadImage (image_t image, bool& is_grayscale) final;

    private:
    GLuint framebuffer_download_;
    GLuint frame_texture_;
};


#endif // FRAMEHELPER_HPP
