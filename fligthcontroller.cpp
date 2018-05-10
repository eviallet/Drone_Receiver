#include "fligthcontroller.h"

FligthController::FligthController(QObject *parent) : QObject(parent) {

    drone.HG.pin = 17;
    drone.HD.pin = 27;
    drone.BG.pin = 22;
    drone.BD.pin = 10;

    if(gpioInitialise()<0)
        qWarning() << "Couldn't initialise GPIOs";
    else
        qDebug() << "GPIOs initialized";

    drone.HG.speed = MIN_WIDTH;
    drone.HD.speed = MIN_WIDTH;
    drone.BG.speed = MIN_WIDTH;
    drone.BD.speed = MIN_WIDTH;

    gpioServo(drone.HG.pin, drone.HG.speed);
    gpioServo(drone.HD.pin, drone.HD.speed);
    gpioServo(drone.BG.pin, drone.BG.speed);
    gpioServo(drone.BD.pin, drone.BD.speed);
}

void FligthController::on_command_received(Command cmd) {
    drone.HG.speed = map(cmd.motor_H_G, 0, 65535, MIN_WIDTH, MAX_WIDTH);
    drone.HD.speed = map(cmd.motor_H_D, 0, 65535, MIN_WIDTH, MAX_WIDTH);
    drone.BG.speed = map(cmd.motor_B_G, 0, 65535, MIN_WIDTH, MAX_WIDTH);
    drone.BD.speed = map(cmd.motor_B_D, 0, 65535, MIN_WIDTH, MAX_WIDTH);

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
