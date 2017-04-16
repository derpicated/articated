// augmentation_widget.h

#ifndef AUGMENTATION_WIDGET_HPP
#define AUGMENTATION_WIDGET_HPP

#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QString>
#include <QVector2D>
#include <QVector3D>

#include "model_loader.hpp"
#include "operators.hpp"

class augmentation_widget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT
    public:
    augmentation_widget (QWidget* parent = 0);
    ~augmentation_widget ();

    void initializeGL ();
    void resizeGL (int w, int h);
    void paintGL ();

    QSize minimumSizeHint () const;
    QSize sizeHint () const;

    public slots:
    bool loadObject (const QString& path);
    void setBackground (image_t image);
    void setScale (const float factor);
    void setXPosition (const float location);
    void setYPosition (const float location);
    void setZRotation (const GLfloat persp_mat[16]);
    void setXRotation (const GLfloat persp_mat[16]);
    void setYRotation (const GLfloat persp_mat[16]);
    void setZRotation (const GLfloat);
    void setXRotation (const GLfloat);
    void setYRotation (const GLfloat);

    signals:

    private:
    void mat_from_angle (GLfloat mat[16], GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
    void mat_multiply (GLfloat a[16], const GLfloat b[16]);
    void mat_scale (GLfloat mat_a[16], GLfloat x, GLfloat y, GLfloat z);
    void mat_translate (GLfloat mat_a[16], GLfloat x, GLfloat y, GLfloat z);
    void mat_ortho (GLfloat mat_a[16],
    GLfloat left,
    GLfloat right,
    GLfloat bottom,
    GLfloat top,
    GLfloat near,
    GLfloat far);
    void mat_identity (GLfloat mat[16]);

    bool upload_to_gpu (const std::vector<float>& model_interleafed);
    void draw_background ();

    model_obj _object;
    float _scale_factor;
    float _x_pos;
    float _y_pos;
    GLfloat _mat_x_rot[16];
    GLfloat _mat_y_rot[16];
    GLfloat _mat_z_rot[16];
    GLfloat _mat_projection[16];
    GLuint _texture_background;
    QOpenGLShaderProgram _program;
};

#endif // AUGMENTATION_WIDGET_HPP
