// frame_data_lister.hpp

#ifndef FRAME_DATA_LISTER_HPP
#define FRAME_DATA_LISTER_HPP

#include <QDebug>
#include <QObject>

#include "shared/frame_data.hpp"
#include "shared/movement3d/movement3d.hpp"

class FrameDataLister : public QObject {
    Q_OBJECT
    Q_PROPERTY (QStringList data MEMBER data_ NOTIFY dataChanged)
    Q_PROPERTY (int length MEMBER length_ NOTIFY lengthChanged)

    public:
    FrameDataLister ()  = default;
    ~FrameDataLister () = default;

    signals:
    void dataChanged ();
    void lengthChanged ();

    public slots:
    void addFrameData (FrameData new_data);

    private:
    QStringList data_;
    int length_{ 0 };
};

#endif // FRAME_DATA_LISTER_HPP
