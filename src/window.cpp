// window.cpp

#define DEFAULT_MODEL ":/3D_models/articated.obj"

#ifndef SAMPLES_DIR
#define SAMPLES_DIR ""
#endif

#include <QComboBox>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMessageBox>
#include <QResizeEvent>
#include <QSpinBox>
#include <QStringList>
#include <QtMultimedia/QCameraInfo>
#include <iostream>

#include "window.hpp"


Window::Window (QWidget* parent)
: QWidget (parent)
, _is_paused (false)
, _vision (_statusbar, _augmentation, this)
, _layout (this)
, _btn_reference ("")
, _btn_pause ("")
, _btn_settings ("") {
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
    _layout_buttons.insertStretch (4, 1);

    _layout_buttons.addWidget (&_btn_settings, 2);
    _btn_settings.setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Expanding);
    _layout_buttons.setAlignment (&_btn_settings, Qt::AlignHCenter);
    _layout_buttons.insertStretch (6, 1);

    _statusbar.raise (); // don't be shy, come closer to people

    connect (&_btn_settings, SIGNAL (clicked ()), this, SLOT (btn_settings_clicked ()));
    connect (&_btn_pause, SIGNAL (clicked ()), this, SLOT (btn_pause_clicked ()));
    connect (&_btn_reference, SIGNAL (clicked ()), this, SLOT (btn_reference_clicked ()));
    connect (&_frame_timer, SIGNAL (timeout ()), this, SLOT (timeout ()));
    update_ui_style ();
    set_framerate (30); // fps

    connect (&_augmentation, SIGNAL (initialized ()), this,
    SLOT (augmentation_widget_initialized ()));
    debug_level (0);
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
        case Qt::Key_Plus: debug_level (_vision.debug_mode () + 1); break;
        case Qt::Key_Minus: debug_level (_vision.debug_mode () - 1); break;
        case Qt::Key_M:
        case Qt::Key_Menu:
        case Qt::Key_Control: btn_settings_clicked (); break;
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
    ;
}

void Window::augmentation_widget_initialized () {
    bool object_load_succes = _augmentation.loadObject (DEFAULT_MODEL);
    if (!object_load_succes) {
        _statusbar.showMessage ("failed to load inital model", 5000);
    }
}

void Window::btn_pause_clicked () {
    if (_is_paused) {
        _vision.set_paused (false);
        _is_paused = false;
    } else {
        _vision.set_paused (true);
        _is_paused = true;
    }
    update_ui_style ();
}

void Window::btn_settings_clicked () {
    //_vision.set_input (QString (":/debug_samples/3_markers_good.webm"));
    // create dialog ui elements
    QDialog dialog (this);
    QBoxLayout layout_dialog (QBoxLayout::TopToBottom, &dialog);
    QPushButton btn_debug_file ("Load test video");
    QComboBox box_camid;
    QSpinBox box_debug;
    QComboBox box_model;
    QLabel label1 ("Select camera:");
    QLabel label2 ("Or load test video");
    QLabel label3 ("Select 3D model:");
    QLabel label4 ("Debug level:");

    // order the ui elements
    dialog.setWindowTitle ("Settings");
    layout_dialog.addWidget (&label1);
    layout_dialog.addWidget (&box_camid);
    layout_dialog.addWidget (&label2);
    layout_dialog.addWidget (&btn_debug_file);
    layout_dialog.addWidget (&label3);
    layout_dialog.addWidget (&box_model);
    layout_dialog.addWidget (&label4);
    layout_dialog.addWidget (&box_debug);

    // fill list of cameras
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras ();
    if (cameras.size () > 0) {
        box_camid.addItem ("Select Camera");
        foreach (const QCameraInfo& cameraInfo, cameras) {
            box_camid.addItem (cameraInfo.description ());
        }
    } else {
        box_camid.setEnabled (false);
        box_camid.addItem ("No Cameras Found");
    }

    // fill list of models
    box_model.addItem ("Select Model");
    QDir path (":/3D_models/");
    QStringList files = path.entryList (QDir::Files);
    box_model.addItems (files);

    connect (&box_camid, SIGNAL (currentIndexChanged (int)), &dialog, SLOT (close ()));
    connect (&box_camid, SIGNAL (currentIndexChanged (int)), this,
    SLOT (dialog_box_camid_indexchanged (int)));

    connect (&box_model, SIGNAL (currentIndexChanged (int)), &dialog, SLOT (close ()));
    connect (&box_model, SIGNAL (currentIndexChanged (QString)), this,
    SLOT (dialog_box_model_indexchanged (QString)));

    connect (&btn_debug_file, SIGNAL (clicked ()), &dialog, SLOT (close ()));
    connect (&btn_debug_file, SIGNAL (clicked ()), this,
    SLOT (btn_load_test_video_clicked ()));

    connect (&box_debug, SIGNAL (valueChanged (int)), this, SLOT (debug_level (int)));

    dialog.exec ();
}

void Window::btn_load_test_video_clicked () {
    _vision.set_input (QString (":/debug_samples/3_markers_good.webm"));
}

void Window::dialog_box_camid_indexchanged (int idx) {
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras ();
    if (cameras.size () > 0) {
        idx -= 1;
        if (idx >= 0 && idx < cameras.size ()) {
            _vision.set_input (cameras.at (idx));
        }
    }
    _statusbar.showMessage (QString ("Selected camera #") + QString::number (idx), 2000);
}

void Window::dialog_box_model_indexchanged (QString name) {
    _augmentation.loadObject (name.prepend (":/3D_models/"));
}


void Window::btn_reference_clicked () {
    _statusbar.showMessage (QString ("set reference button"), 2000);

    _vision.set_reference ();
}

void Window::debug_level (int lvl) {
    lvl = lvl < 0 ? 0 : lvl;
    lvl = lvl > 3 ? 3 : lvl;
    _vision.set_debug_mode (lvl);
}

void Window::update_ui_style () {
    /* ref button */
    QSize _btn_ref_size = _btn_reference.size ();
    _btn_reference.setMinimumWidth (_btn_ref_size.height ());

    QString _btn_ref_style = "QPushButton { "
                             "  background-color: rgba(255, 255, 255, 50);"
                             "  border:5px solid rgb(100, 100, 100);"
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

    /* settings button */
    QSize _btn_settings_size = _btn_settings.size ();
    _btn_settings.setMinimumWidth (_btn_settings_size.height ());

    QString _btn_settings_style = "QPushButton { "
                                  "  background-color: rgba(50, 50, 50, 50);"
                                  "  border:5px solid rgb(0, 0, 0);"
                                  "  border-radius:50px;"
                                  "}"
                                  "QPushButton:pressed {"
                                  "  background-color: rgba(50, 50, 50, 255);"
                                  "  border: 5px solid rgba(50, 50, 50);"
                                  "}"
                                  "QPushButton:focus {"
                                  "  outline: none;"
                                  "}";
    _btn_settings_style.replace ("border-radius:50px",
    QString ("border-radius:" + QString::number (_btn_settings_size.height () / 2) + "px"));
    _btn_settings.setStyleSheet (_btn_settings_style);

    /* pause button */
    QSize _btn_pause_size = _btn_pause.size ();
    _btn_pause.setMinimumWidth (_btn_pause_size.height ());

    QString _btn_pause_style = "";
    if (_is_paused) {
        _btn_pause_style = "QPushButton { "
                           "  background-color: rgba(255, 0, 0, 255);"
                           "  border: 5px solid rgb(255, 0, 0);"
                           "  border-radius:50px;"
                           "}"
                           "QPushButton:focus {"
                           "  outline: none;"
                           "}";
    } else {
        _btn_pause_style = "QPushButton { "
                           "  background-color: rgba(255, 0, 0, 100);"
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
                               "background-color: rgba(100, 100, 100, 255);"
                               "border-top-right-radius: 50px;";
    _statusbar_style.replace ("border-top-right-radius: 50px;",
    QString ("border-top-right-radius:" +
    QString::number (_statusbar.size ().height () * 0.75) + "px"));
    _statusbar.setStyleSheet (_statusbar_style);
}
