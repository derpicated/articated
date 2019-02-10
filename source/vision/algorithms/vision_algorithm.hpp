// vision_algorithm.hpp

#ifndef VISION_ALGORITHM_HPP
#define VISION_ALGORITHM_HPP

#include <QVideoFrame>

#include "augmentation_widget/augmentation_widget.hpp"
#include "movement3d/movement3d.hpp"
#include "operators/operators.hpp"

class vision_algorithm : protected QOpenGLExtraFunctions {
    public:
    vision_algorithm (const int& max_debug_level,
    QOpenGLContext& _opengl_context,
    augmentation_widget& augmentation);
    virtual ~vision_algorithm (){};

    int max_debug_level ();
    void set_debug_level (const int& new_level);
    int debug_level ();

    virtual void set_reference ()                                = 0;
    virtual movement3d execute (const QVideoFrame& const_buffer) = 0;

    protected:
    bool frame_to_ram (const QVideoFrame& const_buffer, image_t& image);
    void set_background (image_t image);
    void download_image (image_t& image, GLuint handle);
    void upload_image (image_t image, GLint texture_handle, bool& is_grayscale);

    augmentation_widget& _augmentation;
    QOpenGLContext& _opengl_context;
    const int _max_debug_level;
    int _debug_level;
};

#endif // VISION_ALGORITHM_HPP
