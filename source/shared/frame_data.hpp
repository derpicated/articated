// frame_data.hpp
#ifndef FRAME_DATA_HPP
#define FRAME_DATA_HPP

#include <QObject>
#include <any>
#include <map>
#include <string>

class FrameData : public QObject {
    Q_OBJECT
    public:
    FrameData (){};

    explicit FrameData (const FrameData& newdata)
    : QObject ()
    , data (newdata.data){};

    explicit FrameData (std::initializer_list<std::pair<std::string, std::any>> data_list)
    : QObject () {
        for (const auto& data_pair : data_list) {
            data.insert (data_pair);
        }
    };

    std::any& operator[] (const std::string& key) {
        return data[key];
    };

    std::map<std::string, std::any> data;
};

Q_DECLARE_METATYPE (FrameData)

#endif // FRAME_DATA_HPP
