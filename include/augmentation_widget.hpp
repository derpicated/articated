// gl_testwidget.h

#ifndef AUGMENTATION_WIDGET_HPP
#define AUGMENTATION_WIDGET_HPP

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QVector2D>
#include <QVector3D>

#include "model_loader.hpp"

class augmentation_widget : public QOpenGLWidget, protected QOpenGLFunctions {
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
    bool loadObject (std::string path);
    void setBackground (GLvoid* image, GLsizei width, GLsizei height);
    void setScale (const float factor);
    void setXPosition (const float location);
    void setYPosition (const float location);
    void setZRotation (const GLfloat persp_mat[16]);
    void setXRotation (const GLfloat persp_mat[16]);
    void setYRotation (const GLfloat persp_mat[16]);

    signals:

    private:
    void draw_background ();

    model_obj _object;
    float _scale_factor;
    float _x_pos;
    float _y_pos;
    GLfloat _x_persp_mat[16];
    GLfloat _y_persp_mat[16];
    GLfloat _z_persp_mat[16];
    GLuint _texture_background;
};

#endif // AUGMENTATION_WIDGET_HPP
