// window.hpp

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QKeyEvent>
#include <QSlider>
#include <QString>

#include "augmentation_widget/augmentation_widget.hpp"
#include "movement3d/movement3d.hpp"

namespace Ui {
class Window;
}

class Window : public QWidget {
    Q_OBJECT

    public:
    explicit Window (QWidget* parent = 0);
    ~Window ();

    public slots:
    void loadBox_indexchanged (QString selection);
    void texButton_clicked ();
    void scaleSlider_valueChanged (int new_value);
    void posXSlider_valueChanged (int new_value);
    void posYSlider_valueChanged (int new_value);
    void rotXSlider_valueChanged (int new_value);
    void rotYSlider_valueChanged (int new_value);
    void rotZSlider_valueChanged (int new_value);

    protected:
    void keyPressEvent (QKeyEvent* event);

    private:
    void angle_to_matrix (float mat[16], float angle, float x, float y, float z);
    Ui::Window* ui;
    AugmentationWidget augmentation_;
    Movement3D transform_;
};

#endif // WINDOW_HPP
