#include "fligthcontroller.h"

FligthController::FligthController(QObject *parent) : QObject(parent) {
    wiringPiSetup();

    drone.HG.pin = 0;
    drone.HD.pin = 2;
    drone.BG.pin = 3;
    drone.BD.pin = 12;

    pinMode(drone.HG.pin, OUTPUT);
    pinMode(drone.HD.pin, OUTPUT);
    pinMode(drone.BG.pin, OUTPUT);
    pinMode(drone.BD.pin, OUTPUT);

    if(softPwmCreate(drone.HG.pin, 0, 100))
        qWarning() << "Couldn't create HG softPwm pin : " << strerror(errno);
    if(softPwmCreate(drone.HD.pin, 0, 100))
        qWarning() << "Couldn't create HD softPwm pin : " << strerror(errno);
    if(softPwmCreate(drone.BG.pin, 0, 100))
        qWarning() << "Couldn't create BG softPwm pin : " << strerror(errno);
    if(softPwmCreate(drone.BD.pin, 0, 100))
        qWarning() << "Couldn't create BD softPwm pin : " << strerror(errno);
}

void FligthController::on_command_received(Command cmd) {
    drone.HG.speed = cmd.motor_H_G;
    drone.HD.speed = cmd.motor_H_D;
    drone.BG.speed = cmd.motor_B_G;
    drone.BD.speed = cmd.motor_B_D;

    qDebug() << "Writing : HG " << QString::number(drone.HG.speed) << " HD " << QString::number(drone.HD.speed)
             << " BG " << QString::number(drone.BG.speed) << " BD " << QString::number(drone.BD.speed);

    softPwmWrite(drone.HG.pin, drone.HG.speed);
    softPwmWrite(drone.HD.pin, drone.HD.speed);
    softPwmWrite(drone.BG.pin, drone.BG.speed);
    softPwmWrite(drone.BD.pin, drone.BD.speed);
}

void FligthController::on_connection_lost() {

}

void FligthController::on_connection_recovered() {

}


FligthController::~FligthController() {
    softPwmStop(drone.HG.pin);
    softPwmStop(drone.HD.pin);
    softPwmStop(drone.BG.pin);
    softPwmStop(drone.BD.pin);
}
