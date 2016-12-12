// window.cpp

#ifndef SAMPLES_DIR
#define SAMPLES_DIR ""
#endif

#include <QComboBox>
#include <QDialog>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMessageBox>
#include <QResizeEvent>
#include <QtMultimedia/QCameraInfo>
#include <iostream>

#include "window.h"


Window::Window (QWidget* parent)
: QWidget (parent)
, _layout (this)
, _btn_reference ("")
, _btn_pause ("") {
    this->layout ()->setContentsMargins (0, 0, 0, 0);
    // add background and foreground
    _layout.addLayout (&_layout_back, 0, 0);
    _layout.addLayout (&_layout_ui, 0, 0);

    _back.setMinimumSize (400, 600);
    _back.setStyleSheet ("background-color: rgba(0, 0, 0);");
    _layout_back.addWidget (&_back, 1);

    _layout_ui.addStretch (8);
    _layout_ui.addLayout (&_layout_buttons, 1);

    _layout_buttons.insertStretch (0, 4);
    _layout_buttons.addWidget (&_btn_reference, 4);

    _layout_buttons.insertStretch (2, 1);
    _layout_buttons.addWidget (&_btn_pause, 2);
    _layout_buttons.insertStretch (4, 1);

    _layout_ui.addWidget (&_statusbar, 0.2);
    _statusbar.setStyleSheet (
    "color:rgb(255, 255, 255);background-color: rgba(0, 0, 0);");
    _statusbar.setSizeGripEnabled (false);
    _statusbar.raise ();


    connect (&_btn_pause, SIGNAL (clicked ()), this, SLOT (btn_pause_clicked ()));
    connect (&_btn_reference, SIGNAL (clicked ()), this, SLOT (btn_reference_clicked ()));
    connect (&_frame_timer, SIGNAL (timeout ()), this, SLOT (timeout ()));
    update_button_style ();
    set_framerate (30); // fps
}

Window::~Window () {
}

void Window::resizeEvent (QResizeEvent* event) {
    (void)event; // this is not used atm
    update_button_style ();
}

QSize Window::minimumSizeHint () const {
    return _layout_back.minimumSize ();
}

QSize Window::sizeHint () const {
    return _layout_back.sizeHint ();
}

void Window::keyPressEvent (QKeyEvent* e) {
    switch (e->key ()) {
        case Qt::Key_Back: this->close (); break;
        case Qt::Key_Escape: this->close (); break;
        default: break;
    }
}

void Window::set_framerate (int framerate) {
    if (framerate < 0) {
        _frame_timer.stop ();
    } else if (framerate == 0) {
        _frame_timer.setInterval (0);
        _frame_timer.start ();
    } else {
        _frame_timer.setInterval (1000 / framerate);
        _frame_timer.start ();
    }
}

void Window::timeout () {
}

void Window::btn_pause_clicked () {
    _statusbar.showMessage (QString ("pause button"), 2000);
}

void Window::btn_reference_clicked () {
    _statusbar.showMessage (QString ("set reference button"), 2000);
}

void Window::update_button_style () {
    /* style of ref button */
    QSize _btn_ref_size = _btn_reference.size ();
    _btn_reference.setMinimumHeight (_btn_ref_size.width ());

    QString _btn_ref_style = "QPushButton { "
                             "  background-color: rgba(255, 255, 255, 50);"
                             "  border:5px solid rgb(255, 255, 255);"
                             "  border-radius:50px;"
                             "}"
                             "QPushButton:pressed {"
                             "  background-color: rgba(255, 255, 255, 255);"
                             "  border: 5px solid rgba(150, 150, 150);"
                             "}";
    _btn_ref_style.replace ("border-radius:50px",
    QString ("border-radius:" + QString::number (_btn_ref_size.width () / 2) + "px"));
    _btn_reference.setStyleSheet (_btn_ref_style);

    /* style of pause button */
    QSize _btn_pause_size = _btn_pause.size ();
    _btn_pause.setMinimumHeight (_btn_pause_size.width ());

    QString _btn_pause_style = "QPushButton { "
                               "  background-color: rgba(255, 0, 0, 100);"
                               "  border: 5px solid rgb(255, 0, 0);"
                               "  border-radius:50px;"
                               "}"
                               "QPushButton:pressed {"
                               "  background-color: rgb(255, 0, 0);"
                               "  border: 5px solid rgb(200, 0, 0);"
                               "}";
    _btn_pause_style.replace ("border-radius:50px",
    QString ("border-radius:" + QString::number (_btn_pause_size.width () / 2) + "px"));
    _btn_pause.setStyleSheet (_btn_pause_style);
}
