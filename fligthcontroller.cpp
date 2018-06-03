#include "fligthcontroller.h"

FligthController::FligthController(QObject *parent) : QObject(parent), notifier(STDIN_FILENO, QSocketNotifier::Read) {

    connect(&notifier, SIGNAL(activated(int)), this, SLOT(text()));

    #ifdef FILE_LOG
        file = new QFile("/home/pi/Qt/data.txt");
        if (file->open(QIODevice::ReadWrite))
            stream = new QTextStream(file);
    #endif

    _desired.motor_H_G = 0;
    _desired.motor_H_D = 0;
    _desired.motor_B_G = 0;
    _desired.motor_B_D = 0;

    drone.HG.pin = 17;
    drone.HD.pin = 27;
    drone.BG.pin = 22;
    drone.BD.pin = 10;

    #ifndef DEBUG
    if(gpioInitialise()<0)
        qWarning() << "Couldn't initialise GPIOs";
    else
        qDebug() << "GPIOs initialized";
    #endif

    drone.HG.speed = MIN_WIDTH;
    drone.HD.speed = MIN_WIDTH;
    drone.BG.speed = MIN_WIDTH;
    drone.BD.speed = MIN_WIDTH;

    #ifndef DEBUG
        gpioServo(drone.HG.pin, drone.HG.speed);
        gpioServo(drone.HD.pin, drone.HD.speed);
        gpioServo(drone.BG.pin, drone.BG.speed);
        gpioServo(drone.BD.pin, drone.BD.speed);
    #endif

    _gyro = new Gyro;
    _roll_pid = new Corrector;
    _pitch_pid = new Corrector;
    //_yaw_pid = new Corrector;

    connect(_gyro, &Gyro::data, this, &FligthController::on_sensors_updated);

    _gyro->start(QThread::HighPriority);

}

void FligthController::on_sensors_updated(double roll, double pitch) {
    _roll_pid->compute(roll);
    _pitch_pid->compute(pitch);

    compute_command();
}

void FligthController::on_command_received(Command cmd) {
    _desired = cmd;

    compute_command();
}

void FligthController::compute_command() {
    Command cmd;
    cmd.motor_H_G = _desired.motor_H_G + _pitch_pid->get_output(); //- _yaw_pid->get_output();
    cmd.motor_H_D = _desired.motor_H_D - _roll_pid->get_output(); //- _yaw_pid->get_output();
    cmd.motor_B_G = _desired.motor_B_G + _roll_pid->get_output(); //+ _yaw_pid->get_output();
    cmd.motor_B_D = _desired.motor_B_D - _pitch_pid->get_output();//+ _yaw_pid->get_output();

    drone.HG.speed = map(cmd.motor_H_G);
    drone.HD.speed = map(cmd.motor_H_D);
    drone.BG.speed = map(cmd.motor_B_G);
    drone.BD.speed = map(cmd.motor_B_D);

    //qDebug() << " HG " << QString::number(drone.HG.speed) << " HD " << QString::number(drone.HD.speed)
    //         << " BG " << QString::number(drone.BG.speed) << " BD " << QString::number(drone.BD.speed);

    #ifdef FILE_LOG
        if(start==0)
            start = QDateTime::currentMSecsSinceEpoch();
        *stream << QString::number(QDateTime::currentMSecsSinceEpoch()-start) << "," << QString::number(drone.HG.speed) << "," <<
                  QString::number(drone.HD.speed) << "," << QString::number(drone.BG.speed) << "," << QString::number(drone.BD.speed) << "\n";
    #endif

    #ifndef DEBUG
        //gpioServo(drone.HG.pin, drone.HG.speed);
        gpioServo(drone.HD.pin, drone.HD.speed);
        gpioServo(drone.BG.pin, drone.BG.speed);
        //gpioServo(drone.BD.pin, drone.BD.speed);
    #endif
}

void FligthController::on_connection_lost() {

}

void FligthController::on_connection_recovered() {

}

FligthController::~FligthController() {
    #ifndef DEBUG
        gpioServo(drone.HG.pin, MIN_WIDTH);
        gpioServo(drone.HD.pin, MIN_WIDTH);
        gpioServo(drone.BG.pin, MIN_WIDTH);
        gpioServo(drone.BD.pin, MIN_WIDTH);
    #endif
}

unsigned short FligthController::map(unsigned short x) {
    return (x - MIN_INT) * (MAX_WIDTH - MIN_WIDTH) / (MAX_INT - MIN_INT) + MIN_WIDTH;
}

void FligthController::text() {
    QTextStream qin(stdin);
    QString line = qin.readLine();
    _roll_pid->setParameters(line.section(" ",0,0).toDouble(), line.section(" ",1,1).toDouble(), line.section(" ",2,2).toDouble());
}
