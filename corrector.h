#ifndef CORRECTOR_H
#define CORRECTOR_H

#include <QThread>
#include <QDebug>
#include <qmath.h>
#include <QDateTime>


#ifdef MAX_OUTPUT_ENABLED
    #define MAX_OUTPUT 6553
    #define MIN_OUTPUT -MAX_OUTPUT
#endif


class Corrector : public QObject {
    Q_OBJECT
public:
    explicit Corrector(QObject *parent = nullptr);
    void compute(float);
    int get_output();
public slots:
    void setSetpoint(int sp);
    void setParameters(double, double, double);
private:
    int output = 0;
    double err_sum = 0, last_err = 0;
    double kp = 0, ki = 0, kd = 0;
    qint64 _last_command = 0;
    int setpoint = 0;
};

#endif // CORRECTOR_H
