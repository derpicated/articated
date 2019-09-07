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

class augmentation_widget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT
    public:
    augmentation_widget (QWidget* parent = 0);
    ~augmentation_widget ();

    // QOpenGLWidget reimplemented functions
    void initializeGL ();
    void resizeGL (int w, int h);
    void paintGL ();
    QSize minimumSizeHint () const;
    QSize sizeHint () const;

    public slots:
    bool loadObject (const QString& path);
    void setBackground (GLuint tex, bool is_grayscale);
    GLuint getBackgroundTexture ();
    void setScale (const float factor);
    void setXPosition (const float location);
    void setYPosition (const float location);
    void setZRotation (const GLfloat);
    void setXRotation (const GLfloat);
    void setYRotation (const GLfloat);

    signals:
    void initialized ();

    private:
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
    GLuint _is_grayscale;
    GLuint _texture_background;
    GLuint _current_handle;
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
