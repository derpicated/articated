// augmentation_widget.h

#ifndef AUGMENTATION_WIDGET_HPP
#define AUGMENTATION_WIDGET_HPP

#include <QMatrix4x4>
#include <QMutex>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QString>
#include <QVector2D>
#include <QVector3D>

#include "model_loader.hpp"
#include "operators/operators.hpp"

class augmentation_widget final : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT
    public:
    // singleton functions
    static augmentation_widget& instance ();

    // QOpenGLWidget reimplemented functions
    void initializeGL ();
    void resizeGL (int w, int h);
    void paintGL ();
    QSize minimumSizeHint () const;
    QSize sizeHint () const;

    public slots:
    bool loadObject (const QString& path);
    void downloadImage (image_t& image, GLuint handle);
    void setBackground (GLuint tex);
    void setBackground (image_t image);
    void setScale (const float factor);
    void setXPosition (const float location);
    void setYPosition (const float location);
    void setZRotation (const GLfloat);
    void setXRotation (const GLfloat);
    void setYRotation (const GLfloat);

    signals:
    void initialized ();

    private:
    // singleton functions
    augmentation_widget (QWidget* parent = 0);
    ~augmentation_widget ();
    augmentation_widget (const augmentation_widget&) = delete;
    augmentation_widget (augmentation_widget&&)      = delete;
    augmentation_widget& operator= (const augmentation_widget&) = delete;
    augmentation_widget& operator= (augmentation_widget&&) = delete;

    void generate_buffers ();
    void compile_shaders ();
    void draw_object ();
    void draw_background ();

    model_obj _object;
    int _view_width;
    int _view_height;
    float _scale_factor;
    float _x_pos;
    float _y_pos;
    float _x_rot;
    float _y_rot;
    float _z_rot;
    QMutex _opengl_mutex;
    QMatrix4x4 _mat_projection;
    GLuint _is_GLRED;
    GLuint _texture_background;
    GLuint _current_handle;
    GLuint _last_handle;
    GLuint _readback_buffer;
    GLuint _background_vao;
    GLuint _object_vao;
    GLuint _background_vbo;
    GLuint _object_vbo;
    GLuint _vertex_count;
    QOpenGLShaderProgram _program_background;
    QOpenGLShaderProgram _program_object;
};

#endif // AUGMENTATION_WIDGET_HPP
