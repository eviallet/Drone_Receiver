#include "fligthcontroller.h"

FligthController::FligthController(QObject *parent) : QObject(parent) {

    drone.HG.pin = 11;
    drone.HD.pin = 13;
    drone.BG.pin = 15;
    drone.BD.pin = 19;

    if(gpioInitialise()<0)
        qWarning() << "Couldn't initialise GPIOs";
    else
        qDebug() << "GPIOs initialized";
}

void FligthController::on_command_received(Command cmd) {
    drone.HG.speed = map(cmd.motor_H_G, 0, 65535, MIN_WIDTH, MAX_WIDTH);
    drone.HD.speed = map(cmd.motor_H_D, 0, 65535, MIN_WIDTH, MAX_WIDTH);
    drone.BG.speed = map(cmd.motor_B_G, 0, 65535, MIN_WIDTH, MAX_WIDTH);
    drone.BD.speed = map(cmd.motor_B_D, 0, 65535, MIN_WIDTH, MAX_WIDTH);

    if(drone.HG.speed<=1200)
        drone.HG.speed = 1000;
    if(drone.HD.speed<=1200)
        drone.HD.speed = 1000;
    if(drone.BG.speed<=1200)
        drone.BG.speed = 1000;
    if(drone.BD.speed<=1200)
        drone.BD.speed = 1000;

    qDebug() << "Writing : HG " << QString::number(drone.HG.speed) << " HD " << QString::number(drone.HD.speed)
             << " BG " << QString::number(drone.BG.speed) << " BD " << QString::number(drone.BD.speed);


    gpioServo(drone.HG.pin, drone.HG.speed);
    gpioServo(drone.HD.pin, drone.HD.speed);
    gpioServo(drone.BG.pin, drone.BG.speed);
    gpioServo(drone.BD.pin, drone.BD.speed);
}

void FligthController::on_connection_lost() {

}

void FligthController::on_connection_recovered() {

}


FligthController::~FligthController() {
    gpioServo(drone.HG.pin, MIN_WIDTH);
    gpioServo(drone.HD.pin, MIN_WIDTH);
    gpioServo(drone.BG.pin, MIN_WIDTH);
    gpioServo(drone.BD.pin, MIN_WIDTH);
}

unsigned short FligthController::map(unsigned short x, int in_min, int in_max, int out_min, int out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
