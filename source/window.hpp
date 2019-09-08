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
    void AugmentationWidgetInitialized ();
    void FPSTimeout ();
    void ButtonSettingsClicked ();
    void ButtonPauseClicked ();
    void ButtonReferenceClicked ();
    void DialogBoxCamIDIndexChanged (int idx);
    void DialogBoxModelIndexChanged (QString);
    void DialogBoxAlgorithmIndexChanged (int idx);
    void ButtonLoadTestVideoClicked ();
    void DebugLevel (int lvl);

    protected:
    void keyPressEvent (QKeyEvent* event);

    private:
    void UpdateUIStyle ();

    bool is_paused_;
    Vision vision_;
    QTimer fps_timer_;

    // ui elements
    QGridLayout layout_;
    AugmentationWidget augmentation_;
    QHBoxLayout layout_back_;      // background
    QHBoxLayout layout_ui_;        // foreground
    QVBoxLayout layout_buttons_;   // buttons
    QVBoxLayout layout_statusbar_; // status bar
    QPushButton button_reference_;
    QPushButton button_pause_;
    QPushButton button_settings_;

    QStatusBar statusbar_;
};

#endif // WINDOW_H
