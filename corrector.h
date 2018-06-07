#ifndef CORRECTOR_H
#define CORRECTOR_H

#include <QThread>
#include <QDebug>
#include <qmath.h>
#include <QDateTime>


class Corrector : public QObject {
    Q_OBJECT
public:
    explicit Corrector(QObject *parent = nullptr);
    void compute(double);
    int get_output();
public slots:
    void setParameters(double, double, double);
private:
    int output = 0;
    double err_sum = 0, last_err = 0;
    double kp = 0, ki = 0, kd = 0;
    qint64 _last_command = 0;
};

#endif // CORRECTOR_H
