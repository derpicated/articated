// vision_algorithm.hpp

#ifndef VISION_ALGORITHM_HPP
#define VISION_ALGORITHM_HPP

#include <QObject>
#include <QOffscreenSurface>
#include <QOpenGLTexture>
#include <QVideoFrame>

#include "augmentation_widget/augmentation_widget.hpp"
#include "movement3d/movement3d.hpp"
#include "operators/operators.hpp"

#ifdef ANDROID
#include <GLES3/gl3.h>
#endif

class VisionAlgorithm : protected QObject, protected QOpenGLExtraFunctions {
    Q_OBJECT

    public:
    VisionAlgorithm (const int& max_debug_level,
    QOpenGLContext& opengl_context,
    augmentation_widget& augmentation);
    virtual ~VisionAlgorithm ();

    virtual int MaxDebugLevel ();
    void SetDebugLevel (const int& new_level);
    int DebugLevel ();

    virtual void SetReference ()                                 = 0;
    virtual movement3d Execute (const QVideoFrame& const_buffer) = 0;

    protected:
    bool FrameToRam (const QVideoFrame& const_buffer, image_t& image);
    bool FrameToTexture (const QVideoFrame& const_buffer, GLuint& texture_handle, GLuint& format);

    void SetBackground (image_t image);
    void DownloadImage (image_t& image, GLuint handle);
    void UploadImage (image_t image, GLint texture_handle, bool& is_grayscale);

    augmentation_widget& augmentation_;
    QOpenGLContext& opengl_context_;
    QOffscreenSurface dummy_surface_;
    GLuint framebuffer_download_;
    GLuint texture_;
    const int max_debug_level_;
    int debug_level_;
};

#endif // VISION_ALGORITHM_HPP
