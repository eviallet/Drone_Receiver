#include "corrector.h"

Corrector::Corrector(QObject *parent) : QObject(parent) {
}


void Corrector::compute(float sensor) {
    if(_last_command == 0) {
        _last_command = QDateTime::currentMSecsSinceEpoch();
        return;
    }

   double dt = QDateTime::currentMSecsSinceEpoch() - _last_command;

   double err = sensor - setpoint;
   err_sum += (err * dt);
   double d_err = (err - last_err) / dt;

   output = qRound(kp * err + ki * err_sum + kd * d_err);

    #ifdef MAX_OUTPUT_ENABLED
       if(output>MAX_OUTPUT)
           output = MAX_OUTPUT;
       else if(output<MIN_OUTPUT)
           output = MIN_OUTPUT;
    #endif

   //qDebug() << QString::number(sensor) << QString::number(output);

   last_err = err;
   _last_command = QDateTime::currentMSecsSinceEpoch();
}

int Corrector::get_output() {
    return output;
}

void Corrector::setSetpoint(int sp) {
    setpoint = sp;
    if(sp!=0)
        qDebug() << "SetPoint updated to " << QString::number(sp);
}

void Corrector::setParameters(double Kp, double Ki, double Kd) {
   kp = Kp;
   ki = Ki;
   kd = Kd;
   qDebug() << "Parameters updated to " << QString::number(Kp) << QString::number(Ki) << QString::number(Kd);
}
