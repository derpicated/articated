// window.h

#ifndef WINDOW_H
#define WINDOW_H

#include <QBoxLayout>
#include <QGridLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QStatusBar>
#include <QTimer>

#include "augmentation_widget/augmentation_widget.hpp"
#include "vision/vision.hpp"
#include "window.hpp"

namespace Ui {
class Window;
}

class Window : public QWidget {
    Q_OBJECT

    public:
    explicit Window (QWidget* parent = 0);
    ~Window ();
    void resizeEvent (QResizeEvent* event);
    QSize minimumSizeHint () const;
    QSize sizeHint () const;

    public slots:
    /* On timer timeout, do ARticated things */
    void augmentation_widget_initialized ();
    void fps_timeout ();
    void btn_settings_clicked ();
    void btn_pause_clicked ();
    void btn_reference_clicked ();
    void dialog_box_camid_indexchanged (int idx);
    void dialog_box_model_indexchanged (QString);
    void dialog_box_algorithm_indexchanged (int idx);
    void btn_load_test_video_clicked ();
    void debug_level (int lvl);

    protected:
    void keyPressEvent (QKeyEvent* event);

    private:
    void update_ui_style ();

    bool _is_paused;
    Vision _vision;
    QTimer _fps_timer;

    // ui elements
    QGridLayout _layout;
    augmentation_widget _augmentation;
    QHBoxLayout _layout_back;    // background
    QHBoxLayout _layout_ui;      // foreground
    QVBoxLayout _layout_buttons; // buttons
    QVBoxLayout _layout_status;  // status bar
    QPushButton _btn_reference;
    QPushButton _btn_pause;
    QPushButton _btn_settings;

    QStatusBar _statusbar;
};

#endif // WINDOW_H
