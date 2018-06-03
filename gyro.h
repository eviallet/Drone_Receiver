#ifndef GYRO_H
#define GYRO_H

#include <QThread>
#include <QProcess>
#include <qmath.h>

class Gyro : public QThread {
    Q_OBJECT
public:
    explicit Gyro(QObject *parent = 0);
    void run() override;
signals:
    void start_script();
    void read();
    void data(double,double);
private slots:
    void on_start_script();
    void on_read();
private:
    QProcess *_script;
    double _last_pitch = 0, _last_roll = 0;
    bool _reading = false;
};

#endif // GYRO_H
