// window.h

#ifndef WINDOW_H
#define WINDOW_H

#include <QBoxLayout>
#include <QGridLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QStatusBar>
#include <QTimer>

namespace Ui {
class Window;
}

class Window : public QWidget {
    Q_OBJECT

    public:
    explicit Window (QWidget* parent = 0);
    ~Window ();

    QSize minimumSizeHint () const;
    QSize sizeHint () const;

    public slots:
    void timeout ();
    void btn_pause_clicked ();
    void btn_reference_clicked ();
    void btn_input_clicked ();
    void dialog_btn_filebrowser_clicked ();
    void dialog_box_camid_indexchanged (int idx);

    protected:
    void keyPressEvent (QKeyEvent* event);

    private:
    unsigned int _framerate = 30;
    // vision_methods _vision_methods;
    // movement3d_average _movements;
    // acquisition _acquisition;

    QTimer _frame_timer;

    // ui elements
    QWidget _holder_layout_img;
    QWidget _holder_layout_btn;
    QBoxLayout _layout_app;
    QGridLayout _layout_img;
    QBoxLayout _layout_btn;
    QPushButton _btn_pause;
    QPushButton _btn_reference;
    QPushButton _btn_input;
    QStatusBar _statusbar;
    // augmentation_widget _augmentation;
};

#endif // WINDOW_H
