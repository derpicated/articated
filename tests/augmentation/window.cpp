// window.cpp
#include <QDebug>
#include <QDir>
#include <math.h>

#include "ui_window.h"
#include "window.hpp"

#include "augmentation_widget.hpp"

#ifndef RES_DIR
#define RES_DIR ""
#endif

Window::Window (QWidget* parent)
: QWidget (parent)
, ui (new Ui::Window) {
    _augmentation = augmentation_widget::instance ();
    _augmentation->show ();
    ui->setupUi (this);

    // fill model selection box
    ui->loadBox->addItem ("Select Model");
    QDir path (":/3D_models/");
    QStringList files = path.entryList (QDir::Files);
    ui->loadBox->addItems (files);

    connect (ui->loadBox, SIGNAL (currentIndexChanged (QString)), this,
    SLOT (loadBox_indexchanged (QString)));
    connect (ui->texButton, SIGNAL (clicked ()), this, SLOT (texButton_clicked ()));

    connect (ui->scaleSlider, SIGNAL (valueChanged (int)), this,
    SLOT (scaleSlider_valueChanged (int)));
    connect (ui->posXSlider, SIGNAL (valueChanged (int)), this,
    SLOT (posXSlider_valueChanged (int)));
    connect (ui->posYSlider, SIGNAL (valueChanged (int)), this,
    SLOT (posYSlider_valueChanged (int)));

    connect (ui->rotXSlider, SIGNAL (valueChanged (int)), this,
    SLOT (rotXSlider_valueChanged (int)));
    connect (ui->rotYSlider, SIGNAL (valueChanged (int)), this,
    SLOT (rotYSlider_valueChanged (int)));
    connect (ui->rotZSlider, SIGNAL (valueChanged (int)), this,
    SLOT (rotZSlider_valueChanged (int)));
}

Window::~Window () {
    delete _augmentation;
    delete ui;
}

void Window::keyPressEvent (QKeyEvent* e) {
    if (e->key () == Qt::Key_Escape)
        close ();
    else
        QWidget::keyPressEvent (e);
}

void Window::loadBox_indexchanged (QString selection) {
    _augmentation->loadObject (selection.prepend (":/3D_models/"));
    _augmentation->update ();
}

void Window::texButton_clicked () {
    QImage image_qt;
    image_t image;

    if (image_qt.load (":/debug_samples/textest.png")) {
        size_t size = image_qt.width () * image_qt.height ();
        image_qt    = image_qt.convertToFormat (QImage::Format_RGB888);
        image.data  = (uint8_t*)malloc (size * 3);
        memcpy (image.data, image_qt.bits (), size * 3);
        image.format = RGB24;
        image.height = image_qt.height ();
        image.width  = image_qt.width ();

        _augmentation->setBackground (image);
        _augmentation->update ();
    } else {
        qDebug () << "no image";
    }
}

void Window::scaleSlider_valueChanged (int new_value) {
    _augmentation->setScale (((float)new_value) / 100);
    _augmentation->update ();
}
void Window::posXSlider_valueChanged (int new_value) {
    _augmentation->setXPosition (((float)new_value) / 100);
    _augmentation->update ();
}
void Window::posYSlider_valueChanged (int new_value) {
    _augmentation->setYPosition (((float)new_value) / 100);
    _augmentation->update ();
}

void Window::rotXSlider_valueChanged (int new_value) {
    _augmentation->setXRotation (new_value);
    _augmentation->update ();
}
void Window::rotYSlider_valueChanged (int new_value) {
    _augmentation->setYRotation (new_value);
    _augmentation->update ();
}
void Window::rotZSlider_valueChanged (int new_value) {
    _augmentation->setZRotation (new_value);
    _augmentation->update ();
}

/*derived from  https://www.opengl.org/sdk/docs/man2/xhtml/glRotate.xml*/
void Window::angle_to_matrix (float mat[16], float angle, float x, float y, float z) {
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
