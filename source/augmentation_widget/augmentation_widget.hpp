// augmentation_widget.hpp

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
#include "movement3d/movement3d.hpp"

class AugmentationWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT
    public:
    AugmentationWidget (QWidget* parent = 0);
    ~AugmentationWidget ();

    // QOpenGLWidget reimplemented functions
    void initializeGL ();
    void resizeGL (int w, int h);
    void paintGL ();
    QSize minimumSizeHint () const;
    QSize sizeHint () const;

    public slots:
    bool LoadObject (const QString& path);
    void DrawFrame (GLuint tex, bool is_grayscale, Movement3D transform);
    void SetBackground (GLuint tex, bool is_grayscale);
    GLuint Background ();
    void SetTransform (Movement3D transform);
    Movement3D Transform ();

    signals:
    void InitializedOpenGL ();

    private:
    void GenerateBuffers ();
    void CompileShaders ();
    void DrawObject ();
    void DrawBackground ();

    ModelLoader object_;
    int view_width_;
    int view_height_;
    Movement3D transform_;
    QMutex opengl_mutex_;
    QMatrix4x4 mat_projection_;
    GLuint is_grayscale_;
    GLuint texture_background_;
    GLuint current_handle_;
    GLuint readback_buffer_;
    GLuint background_vao_;
    GLuint object_vao_;
    GLuint background_vbo_;
    GLuint object_vbo_;
    GLuint vertex_count_;
    QOpenGLShaderProgram program_background_;
    QOpenGLShaderProgram program_object_;
};

#endif // AUGMENTATION_WIDGET_HPP
