// window.cpp

#define DEFAULT_MODEL ":/3D_models/articated.obj"

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

#include "window.hpp"


Window::Window (QWidget* parent)
: QWidget (parent)
, _vision (_augmentation, this)
, _layout (this)
, _btn_reference ("")
, _btn_pause ("") {
    this->layout ()->setContentsMargins (0, 0, 0, 0);
    // add background and foreground
    _layout.addLayout (&_layout_back, 0, 0);
    _layout.addLayout (&_layout_ui, 0, 0);

    _augmentation.setMinimumSize (600, 350); // somewhat 16:9 ratio

    _layout_back.addWidget (&_augmentation, 1);

    _layout_ui.addLayout (&_layout_status, 64);
    _layout_ui.addLayout (&_layout_buttons, 8);
    _layout_ui.insertStretch (2, 1); // small border after buttons

    _layout_status.insertStretch (0, 12);
    _layout_status.addWidget (&_statusbar, 1);
    _statusbar.setSizeGripEnabled (false);

    _layout_buttons.insertStretch (0, 1);
    _layout_buttons.addWidget (&_btn_pause, 2);
    _btn_pause.setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Expanding);
    _layout_buttons.setAlignment (&_btn_pause, Qt::AlignHCenter);
    _layout_buttons.insertStretch (2, 1);

    _layout_buttons.addWidget (&_btn_reference, 4);
    _btn_reference.setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Expanding);
    _layout_buttons.insertStretch (4, 4);

    _statusbar.raise (); // don't be shy, come closer to people

    connect (&_btn_pause, SIGNAL (clicked ()), this, SLOT (btn_pause_clicked ()));
    connect (&_btn_reference, SIGNAL (clicked ()), this, SLOT (btn_reference_clicked ()));
    connect (&_frame_timer, SIGNAL (timeout ()), this, SLOT (timeout ()));
    update_ui_style ();
    set_framerate (30); // fps

    bool object_load_succes = _augmentation.loadObject (DEFAULT_MODEL);
    if (!object_load_succes) {
        _statusbar.showMessage ("failed to load inital model", 5000);
    }
}

Window::~Window () {
}

void Window::resizeEvent (QResizeEvent* event) {
    (void)event; // this is not used atm
    update_ui_style ();
}

QSize Window::minimumSizeHint () const {
    return _layout_back.minimumSize ();
}

QSize Window::sizeHint () const {
    return _layout_back.sizeHint ();
}

void Window::keyPressEvent (QKeyEvent* e) {
    switch (e->key ()) {
        case Qt::Key_Menu:
        case Qt::Key_Control:
            _statusbar.showMessage (QString ("menu button"), 2000);
            break;
        case Qt::Key_Back:
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
    if (_frame_timer.isActive ()) {
        _frame_timer.stop ();
    } else {
        _frame_timer.start ();
    }
    update_ui_style ();
}

void Window::btn_reference_clicked () {
    _statusbar.showMessage (QString ("set reference button"), 2000);

    // test settup
    _vision.execute_frame ();
}

void Window::update_ui_style () {
    /* ref button */
    QSize _btn_ref_size = _btn_reference.size ();
    _btn_reference.setMinimumWidth (_btn_ref_size.height ());

    QString _btn_ref_style = "QPushButton { "
                             "  background-color: rgba(255, 255, 255, 50);"
                             "  border:5px solid rgb(255, 255, 255);"
                             "  border-radius:50px;"
                             "}"
                             "QPushButton:pressed {"
                             "  background-color: rgba(255, 255, 255, 255);"
                             "  border: 5px solid rgba(150, 150, 150);"
                             "}"
                             "QPushButton:focus {"
                             "  outline: none;"
                             "}";
    _btn_ref_style.replace ("border-radius:50px",
    QString ("border-radius:" + QString::number (_btn_ref_size.height () / 2) + "px"));
    _btn_reference.setStyleSheet (_btn_ref_style);

    /* pause button */
    QSize _btn_pause_size = _btn_pause.size ();
    _btn_pause.setMinimumWidth (_btn_pause_size.height ());

    QString _btn_pause_style = "";
    if (_frame_timer.isActive ()) {
        _btn_pause_style = "QPushButton { "
                           "  background-color: rgba(255, 0, 0, 100);"
                           "  border: 5px solid rgb(255, 0, 0);"
                           "  border-radius:50px;"
                           "}"
                           "QPushButton:focus {"
                           "  outline: none;"
                           "}";
    } else {
        _btn_pause_style = "QPushButton { "
                           "  background-color: rgba(255, 0, 0, 255);"
                           "  border: 5px solid rgb(255, 0, 0);"
                           "  border-radius:50px;"
                           "}"
                           "QPushButton:focus {"
                           "  outline: none;"
                           "}";
    }
    _btn_pause_style.replace ("border-radius:50px",
    QString ("border-radius:" + QString::number (_btn_pause_size.height () / 2) + "px"));
    _btn_pause.setStyleSheet (_btn_pause_style);

    /* status bar */
    QString _statusbar_style = "color:rgba(255, 255, 255, 255);"
                               "background-color: rgba(255, 255, 255, 42);"
                               "border-top-right-radius: 50px;";
    _statusbar_style.replace ("border-top-right-radius: 50px;",
    QString ("border-top-right-radius:" +
    QString::number (_statusbar.size ().height () * 0.75) + "px"));
    _statusbar.setStyleSheet (_statusbar_style);
}
