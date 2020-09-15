// vision_algorithm.hpp

#ifndef VISION_ALGORITHM_HPP
#define VISION_ALGORITHM_HPP

#include <QObject>
#include <QOffscreenSurface>
#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>
#include <QVideoFrame>
#include <utility>

#include "shared/frame_data.hpp"
#include "shared/image.hpp"
#include "shared/movement3d/movement3d.hpp"

#ifdef ANDROID
#include <GLES3/gl3.h>
#endif

class VisionAlgorithm : protected QObject, protected QOpenGLExtraFunctions {
    Q_OBJECT

    public:
    VisionAlgorithm (const int& max_debug_level, QOpenGLContext& opengl_context);
    virtual ~VisionAlgorithm ();

    virtual int MaxDebugLevel ();
    void SetDebugLevel (const int& new_level);
    int DebugLevel ();

    virtual void SetReference ()                                = 0;
    virtual FrameData Execute (const QVideoFrame& const_buffer) = 0;

    protected:
    bool FrameToRam (const QVideoFrame& const_buffer, image_t& image);
    bool FrameToTexture (const QVideoFrame& const_buffer, GLuint& texture_handle, GLuint& format);

    void SetBackground (image_t image);
    void DownloadImage (image_t& image, GLuint handle);
    void UploadImage (image_t image, bool& is_grayscale);

    QOpenGLContext& opengl_context_;
    QOffscreenSurface dummy_surface_;
    GLuint framebuffer_download_;
    GLuint texture_;
    const int max_debug_level_;
    int debug_level_;
    GLuint background_tex_;
    bool background_is_grayscale_;
};

#endif // VISION_ALGORITHM_HPP
