// augmentation_widget.cpp

#include "augmentation_widget.hpp"

#ifdef OPENGL_ES
#include <GLES/gl.h>
#else
#include <GL/gl.h>
#endif // OPENGL_ES

#include <QTemporaryFile>
#include <QVector2D>
#include <QVector3D>
#include <math.h>

// clang-format off
#define MATRIX_INITVAL  1, 0, 0, 0,\
                        0, 1, 0, 0,\
                        0, 0, 1, 0,\
                        0, 0, 0, 1
// clang-format on

augmentation_widget::augmentation_widget (QWidget* parent)
: QOpenGLWidget (parent)
, _scale_factor (1.0f)
, _x_pos (0.0f)
, _y_pos (0.0f)
, _x_persp_mat{ MATRIX_INITVAL }
, _y_persp_mat{ MATRIX_INITVAL }
, _z_persp_mat{ MATRIX_INITVAL } {
}

augmentation_widget::~augmentation_widget () {
}

QSize augmentation_widget::minimumSizeHint () const {
    return QSize (600, 350);
}

QSize augmentation_widget::sizeHint () const {
    return QSize (600, 350);
}

bool augmentation_widget::loadObject (QString resource_path) {
    bool status = false;

    // extract model from resources into filesystem and parse it
    QFile resource_file (resource_path);
    if (resource_file.exists ()) {
        auto temp_file  = QTemporaryFile::createNativeFile (resource_file);
        QString fs_path = temp_file->fileName ();

        if (!fs_path.isEmpty ()) {
            status = _object.load (fs_path.toStdString ());
        }
    }
    return status;
}

void augmentation_widget::setBackground (image_t image) {
    // create background texture
    glBindTexture (GL_TEXTURE_2D, _texture_background);

    switch (image.format) {
        case RGB24: {
            glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height,
            0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
            break;
        }
        case GREY8: {
            glTexImage2D (GL_TEXTURE_2D, 0, GL_R8, image.width, image.height, 0,
            GL_RED, GL_UNSIGNED_BYTE, image.data);
        }
        case BINARY8: {
            glTexImage2D (GL_TEXTURE_2D, 0, GL_R8, image.width, image.height, 0,
            GL_RED, GL_UNSIGNED_BYTE, image.data);
        }
    }

    // normalize coordinates
    glMatrixMode (GL_TEXTURE);
    glLoadIdentity ();
    glScalef (1.0 / image.width, 1.0 / image.height, 1);
    glMatrixMode (GL_MODELVIEW);
}

void augmentation_widget::setScale (const float scale) {
    _scale_factor = scale;
}

void augmentation_widget::setXPosition (const float location) {
    _x_pos = location;
}

void augmentation_widget::setYPosition (const float location) {
    _y_pos = location;
}

void augmentation_widget::setXRotation (const GLfloat persp_mat[16]) {
    for (int i = 0; i < 16; i++) {
        _x_persp_mat[i] = persp_mat[i];
    }
}

void augmentation_widget::setYRotation (const GLfloat persp_mat[16]) {
    for (int i = 0; i < 16; i++) {
        _y_persp_mat[i] = persp_mat[i];
    }
}

void augmentation_widget::setZRotation (const GLfloat persp_mat[16]) {
    for (int i = 0; i < 16; i++) {
        _z_persp_mat[i] = persp_mat[i];
    }
}

void augmentation_widget::setXRotation (const GLfloat angle) {
    angle_to_matrix (_x_persp_mat, angle, 1, 0, 0);
}

void augmentation_widget::setYRotation (const GLfloat angle) {
    angle_to_matrix (_y_persp_mat, angle, 0, 1, 0);
}

void augmentation_widget::setZRotation (const GLfloat angle) {
    angle_to_matrix (_z_persp_mat, angle, 0, 0, 1);
}

/*derived from
 * https://www.opengl.org/sdk/docs/man2/xhtml/glRotate.xml*/
void augmentation_widget::angle_to_matrix (GLfloat mat[16], GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
    float deg2rad = 3.14159265f / 180.0f;
    float c       = cosf (angle * deg2rad);
    float s       = sinf (angle * deg2rad);
    float c1      = 1.0f - c;

    // build rotation matrix
    mat[0]  = (x * x * c1) + c;
    mat[1]  = (x * y * c1) - (z * s);
    mat[2]  = (x * z * c1) + (y * s);
    mat[3]  = 0;
    mat[4]  = (y * x * c1) + (z * s);
    mat[5]  = (y * y * c1) + c;
    mat[6]  = (y * z * c1) - (x * s);
    mat[7]  = 0;
    mat[8]  = (z * x * c1) - (y * s);
    mat[9]  = (z * y * c1) + (x * s);
    mat[10] = (z * z * c1) + c;
    mat[11] = 0;
    mat[12] = 0;
    mat[13] = 0;
    mat[14] = 0;
    mat[15] = 1;
}

void augmentation_widget::initializeGL () {
    initializeOpenGLFunctions ();

    glClearColor (1, 0.5, 1, 1.0f);
    glEnable (GL_DEPTH_TEST);
    // glEnable (GL_CULL_FACE);
    glShadeModel (GL_SMOOTH);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
    glEnable (GL_COLOR_MATERIAL);
    glMatrixMode (GL_PROJECTION);
    glEnable (GL_TEXTURE_2D);

    glGenTextures (1, &_texture_background);
    glBindTexture (GL_TEXTURE_2D, _texture_background);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glMatrixMode (GL_MODELVIEW);
    static GLfloat lightPosition[4] = { 0, 0, 10, 1.0 };
    glLightfv (GL_LIGHT0, GL_POSITION, lightPosition);
    glLoadIdentity ();
    // gluPerspective (33.7, 1.3, 0.1, 100.0);
    glMatrixMode (GL_MODELVIEW);
}

void augmentation_widget::resizeGL (int width, int height) {
    int side = qMin (width, height);
    glViewport ((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
#ifdef OPENGL_ES
    glOrthof (-2.0f, +2.0f, -2.0f, +2.0f, 1.0f, 25.0f);
#else
    glOrtho (-2.0f, +2.0f, -2.0f, +2.0f, 1.0f, 25.0f);
#endif // OPENGL_ES
    glMatrixMode (GL_MODELVIEW);
}

void augmentation_widget::paintGL () {
    glMatrixMode (GL_MODELVIEW);
    // QOpenGLFunctions* f = QOpenGLContext::currentContext
    // ()->functions
    // ();
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity ();

    // draw background
    glTranslatef (0.0, 0.0, -10.0);
    draw_background ();
    glPushMatrix ();

    // draw object
    glTranslatef (_x_pos, _y_pos, 0);
    glScalef (_scale_factor, _scale_factor, _scale_factor);
    glMultMatrixf (_x_persp_mat);
    glMultMatrixf (_y_persp_mat);
    glMultMatrixf (_z_persp_mat);

    _object.draw ();

    glPopMatrix ();
}

void augmentation_widget::draw_background () {
#ifndef OPENGL_ES
    glBindTexture (GL_TEXTURE_2D, _texture_background);
    glBegin (GL_QUADS);
    glColor3f (1, 1, 1);
    glTexCoord2f (0.0, 1.0);
    glVertex3f (-4.0, -3.0, -2.0);
    glTexCoord2f (1.0, 1.0);
    glVertex3f (4.0, -3.0, -2.0);
    glTexCoord2f (1.0, 0.0);
    glVertex3f (4.0, 3.0, -2.0);
    glTexCoord2f (0.0, 0.0);
    glVertex3f (-4.0, 3.0, -2.0);
    glEnd ();
#else  // OPENGL_ES
    // clang-format off
    GLfloat vertices_buff[6 * 3] = {    -4.0, -3.0, -2.0,   // poly 1 a
                                        4.0, -3.0, -2.0,    // poly 1 b
                                        4.0, 3.0, -2.0,     // poly 1 c
                                        -4.0, -3.0, -.0,   // poly 2 a
                                        4.0, 3.0, -0.0,     // poly 2 b
                                        -4.0, 3.0, -0.0 };  // poly 2 c

    GLfloat normals_buff[6 * 3] = {     0, 0, 1,           // poly 1 a
                                        0, 0, 1,           // poly 1 b
                                        0, 0, 1,           // poly 1 c
                                        0, 0, 1,           // poly 2 a
                                        0, 0, 1,           // poly 2 b
                                        0, 0, 1 };         // poly 2 c

    GLfloat texture_buff[6 * 2] = {     0.0, 1.0,           // poly 1 a
                                        1.0, 1.0,           // poly 1 b
                                        1.0, 0.0,           // poly 1 c
                                        0.0, 1.0,           // poly 2 a
                                        1.0, 0.0,           // poly 2 b
                                        0.0, 0.0 };         // poly 2 c

    GLfloat colors_buff[6 * 4];
    // clang-format on
    // all collors white
    for (int i = 0; i < (6 * 4); i++) {
        colors_buff[i] = 1.0f;
    }

    glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_NORMAL_ARRAY);
    glEnableClientState (GL_COLOR_ARRAY);
    // glEnableClientState (GL_TEXTURE_COORD_ARRAY);

    glBindTexture (GL_TEXTURE_2D, _texture_background);
    glVertexPointer (3, GL_FLOAT, 0, vertices_buff);
    glNormalPointer (GL_FLOAT, 0, normals_buff);
    glColorPointer (4, GL_FLOAT, 0, colors_buff);
    // glTexCoordPointer (2, GL_FLOAT, 0, texture_buff);

    glDrawArrays (GL_TRIANGLES, 0, 2); // draw the 2 triangles

    glDisableClientState (GL_VERTEX_ARRAY);
    glDisableClientState (GL_NORMAL_ARRAY);
    glDisableClientState (GL_COLOR_ARRAY);
// glDisableClientState (GL_TEXTURE_COORD_ARRAY);
#endif // OPENGL_ES
}
