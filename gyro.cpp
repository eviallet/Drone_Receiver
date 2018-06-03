#include "gyro.h"

Gyro::Gyro(QObject *parent) : QThread(parent) {
    _script = new QProcess;
    _script->setProgram("/usr/bin/python");
    _script->setArguments(QStringList() << "/home/pi/gyro.py");
    _script->setReadChannel(QProcess::StandardOutput);
    connect(this, &Gyro::read, this, &Gyro::on_read);
    connect(this, &Gyro::start_script, this, &Gyro::on_start_script);
}

void Gyro::run() {
    emit(start_script());
    while(true)
        if(!_reading) {
            _reading = true;
            emit(read());
        }
}

void Gyro::on_start_script() {
    if(_script->state()!=QProcess::Running&&_script->state()!=QProcess::Starting)
        _script->start();
}

void Gyro::on_read() {
    if(_script->canReadLine()) {
        QString str = QString(_script->read(14).replace("\n",""));

        if((qAbs(str.section(" ",0,0).toDouble()-_last_roll)>=1.5)||qAbs(str.section(" ",1,1).toDouble()-_last_pitch)>=1.5) {
            emit(data(str.section(" ",0,0).toDouble(),str.section(" ",1,1).toDouble()));
            _last_roll = str.section(" ",0,0).toDouble();
            _last_pitch = str.section(" ",1,1).toDouble();
        }
    }
    _reading = false;
}
