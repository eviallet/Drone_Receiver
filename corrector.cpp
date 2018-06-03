#include "corrector.h"

Corrector::Corrector(QObject *parent) : QObject(parent) {
}


int Corrector::compute(double sensor) {
    if(_last_command == 0) {
        _last_command = QDateTime::currentMSecsSinceEpoch();
        return 0;
    }
   double dt = QDateTime::currentMSecsSinceEpoch() - _last_command;

   double err = sensor;
   err_sum += (err * dt);
   double d_err = (err - last_err) / dt;

   output = qRound(kp * err + ki * err_sum + kd * d_err);

   last_err = err;
   _last_command = QDateTime::currentMSecsSinceEpoch();

   return output;
}

int Corrector::get_output() {
    return output;
}

void Corrector::setParameters(double Kp, double Ki, double Kd) {
   kp = Kp;
   ki = Ki;
   kd = Kd;
   qDebug() << "Parameters updated to" << QString::number(Kp) << QString::number(Ki) << QString::number(Kd);
}
