// window.cpp

#define DEFAULT_MODEL ":/3D_models/articated.obj"

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
, is_paused_ (false)
, vision_ (statusbar_, this)
, layout_ (this)
, button_reference_ ("")
, button_pause_ ("")
, button_settings_ ("") {
    this->layout ()->setContentsMargins (0, 0, 0, 0);
    // add background and foreground
    layout_.addLayout (&layout_back_, 0, 0);
    layout_.addLayout (&layout_ui_, 0, 0);

    augmentation_.setMinimumSize (600, 350); // somewhat 16:9 ratio

    layout_back_.addWidget (&augmentation_, 1);

    layout_ui_.addLayout (&layout_statusbar_, 64);
    layout_ui_.addLayout (&layout_buttons_, 8);
    layout_ui_.insertStretch (2, 1); // small border after buttons

    layout_statusbar_.insertStretch (0, 12);
    layout_statusbar_.addWidget (&statusbar_, 1);
    statusbar_.setSizeGripEnabled (false);

    layout_buttons_.insertStretch (0, 1);
    layout_buttons_.addWidget (&button_pause_, 2);
    button_pause_.setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout_buttons_.setAlignment (&button_pause_, Qt::AlignHCenter);
    layout_buttons_.insertStretch (2, 1);

    layout_buttons_.addWidget (&button_reference_, 4);
    button_reference_.setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout_buttons_.insertStretch (4, 1);

    layout_buttons_.addWidget (&button_settings_, 2);
    button_settings_.setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout_buttons_.setAlignment (&button_settings_, Qt::AlignHCenter);
    layout_buttons_.insertStretch (6, 1);

    statusbar_.raise (); // don't be shy, come closer to people

    connect (&button_settings_, SIGNAL (clicked ()), this, SLOT (ButtonSettingsClicked ()));
    connect (&button_pause_, SIGNAL (clicked ()), this, SLOT (ButtonPauseClicked ()));
    connect (&button_reference_, SIGNAL (clicked ()), this,
    SLOT (ButtonReferenceClicked ()));
    UpdateUIStyle ();

    connect (&fps_timer_, SIGNAL (timeout ()), this, SLOT (FPSTimeout ()));
    connect (&vision_, SIGNAL (FrameProcessed (FrameData)), &augmentation_,
    SLOT (DrawFrame (FrameData)));
    connect (&augmentation_, SIGNAL (InitializedOpenGL ()), this,
    SLOT (AugmentationWidgetInitialized ()));

    fps_timer_.setInterval (1000);
    fps_timer_.start ();
}

Window::~Window () {
}

void Window::resizeEvent (QResizeEvent* event) {
    (void)event; // this is not used atm
    UpdateUIStyle ();
}

QSize Window::minimumSizeHint () const {
    return layout_back_.minimumSize ();
}

QSize Window::sizeHint () const {
    return layout_back_.sizeHint ();
}

void Window::keyPressEvent (QKeyEvent* e) {
    switch (e->key ()) {
        case Qt::Key_Plus: DebugLevel (vision_.DebugLevel () + 1); break;
        case Qt::Key_Minus: DebugLevel (vision_.DebugLevel () - 1); break;
        case Qt::Key_M:
        case Qt::Key_Menu:
        case Qt::Key_Control: ButtonSettingsClicked (); break;
        case Qt::Key_Back:
        case Qt::Key_Escape: this->close (); break;
        default: break;
    }
}

void Window::AugmentationWidgetInitialized () {
    vision_.InitializeOpenGL (augmentation_.context ());
    bool object_load_succes = augmentation_.LoadObject (DEFAULT_MODEL);
    if (!object_load_succes) {
        statusbar_.showMessage ("failed to load inital model", 5000);
    }
}

void Window::FPSTimeout () {
    int failed_frames = vision_.GetAndClearFailedFrameCount ();
    if (failed_frames > 0) {
        statusbar_.showMessage (QString ("%1 failed frames").arg (failed_frames), 1000);
    }
}

void Window::ButtonPauseClicked () {
    if (is_paused_) {
        vision_.SetPaused (false);
        is_paused_ = false;
    } else {
        vision_.SetPaused (true);
        is_paused_ = true;
    }
    UpdateUIStyle ();
}

void Window::ButtonSettingsClicked () {
    // vision_.set_input (QString (":/debug_samples/3_markers_good.webm"));
    // create dialog ui elements
    QDialog dialog (this);
    QBoxLayout layout_dialog (QBoxLayout::TopToBottom, &dialog);
    QPushButton btn_debug_file ("Load test video");
    QComboBox box_camid;
    QComboBox box_debug;
    QComboBox box_model;
    QComboBox box_algorithm;
    QLabel label1 ("Select camera:");
    QLabel label2 ("Or load test video");
    QLabel label3 ("Select 3D model:");
    QLabel label4 ("Select vision algorithm:");
    QLabel label5 ("Debug level:");

    // order the ui elements
    dialog.setWindowTitle ("Settings");
    layout_dialog.addWidget (&label1);
    layout_dialog.addWidget (&box_camid);
    layout_dialog.addWidget (&label2);
    layout_dialog.addWidget (&btn_debug_file);
    layout_dialog.addWidget (&label3);
    layout_dialog.addWidget (&box_model);
    layout_dialog.addWidget (&label4);
    layout_dialog.addWidget (&box_algorithm);
    layout_dialog.addWidget (&label5);
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

    // fill list of algorithms
    box_algorithm.addItem ("Select Algorithm");
    box_algorithm.addItems (vision_.AlgorithmList ());

    // fill list of debug levels
    int max_debug_level = vision_.MaxDebugLevel ();
    for (int i = 0; i <= max_debug_level; i++) {
        box_debug.addItem (QString::number (i));
    }
    box_debug.setCurrentIndex (vision_.DebugLevel ());

    connect (&box_camid, SIGNAL (currentIndexChanged (int)), &dialog, SLOT (close ()));
    connect (&box_camid, SIGNAL (currentIndexChanged (int)), this,
    SLOT (DialogBoxCamIDIndexChanged (int)));

    connect (&btn_debug_file, SIGNAL (clicked ()), &dialog, SLOT (close ()));
    connect (&btn_debug_file, SIGNAL (clicked ()), this,
    SLOT (ButtonLoadTestVideoClicked ()));

    connect (&box_model, SIGNAL (currentIndexChanged (int)), &dialog, SLOT (close ()));
    connect (&box_model, SIGNAL (currentIndexChanged (QString)), this,
    SLOT (DialogBoxModelIndexChanged (QString)));

    connect (&box_algorithm, SIGNAL (currentIndexChanged (int)), &dialog, SLOT (close ()));
    connect (&box_algorithm, SIGNAL (currentIndexChanged (int)), this,
    SLOT (DialogBoxAlgorithmIndexChanged (int)));

    connect (&box_debug, SIGNAL (currentIndexChanged (int)), &dialog, SLOT (close ()));
    connect (&box_debug, SIGNAL (currentIndexChanged (int)), this,
    SLOT (DebugLevel (int)));

    dialog.exec ();

    // no need to disconnect signals, qt cleans them up
}

void Window::ButtonLoadTestVideoClicked () {
    vision_.SetInput (QString (":/debug_samples/3_markers_good.webm"));
}

void Window::DialogBoxCamIDIndexChanged (int idx) {
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras ();
    if (cameras.size () > 0) {
        idx -= 1;
        if (idx >= 0 && idx < cameras.size ()) {
            vision_.SetInput (cameras.at (idx));
        }
    }
    statusbar_.showMessage (QString ("Selected camera #") + QString::number (idx), 2000);
}

void Window::DialogBoxModelIndexChanged (QString name) {
    augmentation_.LoadObject (name.prepend (":/3D_models/"));
}

void Window::DialogBoxAlgorithmIndexChanged (int idx) {
    vision_.SetAlgorithm (idx);
}

void Window::ButtonReferenceClicked () {
    statusbar_.showMessage (QString ("set reference button"), 2000);

    vision_.SetReference ();
}

void Window::DebugLevel (int lvl) {
    vision_.SetDebugLevel (lvl);
}

void Window::UpdateUIStyle () {
    /* ref button */
    QSize _btn_ref_size = button_reference_.size ();
    button_reference_.setMinimumWidth (_btn_ref_size.height ());

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
    button_reference_.setStyleSheet (_btn_ref_style);

    /* settings button */
    QSize _btn_settings_size = button_settings_.size ();
    button_settings_.setMinimumWidth (_btn_settings_size.height ());

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
    button_settings_.setStyleSheet (_btn_settings_style);

    /* pause button */
    QSize _btn_pause_size = button_pause_.size ();
    button_pause_.setMinimumWidth (_btn_pause_size.height ());

    QString _btn_pause_style = "";
    if (is_paused_) {
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
    button_pause_.setStyleSheet (_btn_pause_style);

    /* status bar */
    QString _statusbar_style = "color:rgba(255, 255, 255, 255);"
                               "background-color: rgba(100, 100, 100, 255);"
                               "border-top-right-radius: 50px;";
    _statusbar_style.replace ("border-top-right-radius: 50px;",
    QString ("border-top-right-radius:" +
    QString::number (statusbar_.size ().height () * 0.75) + "px"));
    statusbar_.setStyleSheet (_statusbar_style);
}
