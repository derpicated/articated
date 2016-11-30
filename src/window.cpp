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
#include <QtMultimedia/QCameraInfo>

#include "window.h"

Window::Window (QWidget* parent)
: QWidget (parent)
, _holder_layout_img (this)
, _holder_layout_btn (this)
, _layout_app (QBoxLayout::TopToBottom, this)
, _layout_img (&_holder_layout_img)
, _layout_btn (QBoxLayout::LeftToRight, &_holder_layout_btn)
, _btn_pause ("Pause", this)
, _btn_reference ("Set Reference", this)
, _btn_input ("Select Input", this)
, _statusbar (this) {
    _layout_app.addWidget (&_holder_layout_img);
    _layout_app.addWidget (&_holder_layout_btn);
    _layout_app.addWidget (&_statusbar);

    //_layout_img.addWidget (&_augmentation, 0, 0);
    _layout_btn.addWidget (&_btn_pause);
    _layout_btn.addWidget (&_btn_reference);
    _layout_btn.addWidget (&_btn_input);
    _statusbar.setSizeGripEnabled (false);

    connect (&_frame_timer, SIGNAL (timeout ()), this, SLOT (timeout ()));
    connect (&_btn_pause, SIGNAL (clicked ()), this, SLOT (btn_pause_clicked ()));
    connect (&_btn_reference, SIGNAL (clicked ()), this, SLOT (btn_reference_clicked ()));
    connect (&_btn_input, SIGNAL (clicked ()), this, SLOT (btn_input_clicked ()));

    _frame_timer.setInterval (1000 / _framerate);
    _frame_timer.start ();
}

Window::~Window () {
}

QSize Window::minimumSizeHint () const {
    return _layout_app.minimumSize ();
}

QSize Window::sizeHint () const {
    return _layout_app.sizeHint ();
}

void Window::keyPressEvent (QKeyEvent* e) {
}

void Window::timeout () {
}

void Window::btn_pause_clicked () {
    if (_frame_timer.isActive ()) {
        _frame_timer.stop ();
        _btn_pause.setText ("Resume");
    } else {
        _frame_timer.start ();
        _btn_pause.setText ("Pause");
    }
}

void Window::btn_reference_clicked () {
}

void Window::btn_input_clicked () {
    // create dialog ui elements
    QDialog dialog (this);
    QBoxLayout layout_dialog (QBoxLayout::TopToBottom, &dialog);
    QPushButton btn_open_filebrowser ("Open File Browser");
    QComboBox box_camid;
    QLabel label1 ("Select camera:");
    QLabel label2 ("Or choose a file:");

    // order the ui elements
    dialog.setWindowTitle ("Select Input");
    layout_dialog.addWidget (&label1);
    layout_dialog.addWidget (&box_camid);
    layout_dialog.addWidget (&label2);
    layout_dialog.addWidget (&btn_open_filebrowser);

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

    connect (&box_camid, SIGNAL (currentIndexChanged (int)), &dialog, SLOT (close ()));
    connect (&box_camid, SIGNAL (currentIndexChanged (int)), this,
    SLOT (dialog_box_camid_indexchanged (int)));
    connect (&btn_open_filebrowser, SIGNAL (clicked ()), &dialog, SLOT (close ()));
    connect (&btn_open_filebrowser, SIGNAL (clicked ()), this,
    SLOT (dialog_btn_filebrowser_clicked ()));

    dialog.exec ();
}

void Window::dialog_btn_filebrowser_clicked () {
    // test file
    QString file_name = QFileDialog::getOpenFileName (
    this, tr ("Open Video"), SAMPLES_DIR, tr ("Videos (*.webm)"));

    if (!file_name.isEmpty ()) {
        //_acquisition.source (file_name.toStdString ());
        _statusbar.showMessage (QString ("Set source: ") + file_name, 2000);
    }
}

void Window::dialog_box_camid_indexchanged (int idx) {
    //_acquisition.source (idx - 1);
    _statusbar.showMessage (QString ("Selected camera #") + QString::number (idx), 2000);
}
