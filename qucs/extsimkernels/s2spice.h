#ifndef S2SPICE_H
#define S2SPICE_H

#include <QtCore>

class S2Spice
{

private:
    double z0;
    QString file;
    QString device_name;

public:
    void setZ0(double z0_) { z0 = z0_; }
    void setFile(const QString &file_) { file = file_; }
    void setDeviceName(const QString &name_) {device_name = name_; }

    QString convertTouchstone();


    S2Spice();
    virtual ~S2Spice() {}
};

#endif // S2SPICE_H
