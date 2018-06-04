#include "fligthcontroller.h"

FligthController::FligthController(QObject *parent) : QThread(parent), _notifier(STDIN_FILENO, QSocketNotifier::Read) {
    connect(&_notifier, SIGNAL(activated(int)), this, SLOT(text()));

    #ifdef FILE_LOG
        _file = new QFile("/home/pi/Qt/data.txt");
        if (_file->open(QIODevice::ReadWrite))
            _stream = new QTextStream(_file);
    #endif

    _desired.motor_H_G = 22000;
    _desired.motor_H_D = 22000;
    _desired.motor_B_G = 22000;
    _desired.motor_B_D = 22000;

    _drone.HG.pin = 17;
    _drone.HD.pin = 27;
    _drone.BG.pin = 22;
    _drone.BD.pin = 10;

    #ifndef DEBUG
    if(gpioInitialise()<0)
        qWarning() << "Couldn't initialise GPIOs";
    else
        qDebug() << "GPIOs initialized";
    #endif

    _drone.HG.speed = MIN_WIDTH;
    _drone.HD.speed = MIN_WIDTH;
    _drone.BG.speed = MIN_WIDTH;
    _drone.BD.speed = MIN_WIDTH;

    #ifndef DEBUG
        gpioServo(_drone.HG.pin, _drone.HG.speed);
        gpioServo(_drone.HD.pin, _drone.HD.speed);
        gpioServo(_drone.BG.pin, _drone.BG.speed);
        gpioServo(_drone.BD.pin, _drone.BD.speed);
    #endif

    _roll_pid = new Corrector;
    _pitch_pid = new Corrector;
    _yaw_pid = new Corrector;

    _gyro = new Sensor;

    _gyro->start();
}

void FligthController::run() {
    while(true) {
        if(_last==0)
            _last = QDateTime::currentMSecsSinceEpoch();
        else if(QDateTime::currentMSecsSinceEpoch()-_last >= 1) {
            update_angles(_gyro->get_angles());
            _last = QDateTime::currentMSecsSinceEpoch();
        }
    }
}

void FligthController::update_angles(std::tuple<float,float,float> ypr) {
    _roll_pid->compute(std::get<2>(ypr));
    _pitch_pid->compute(std::get<1>(ypr));
    _yaw_pid->compute(std::get<0>(ypr));

    compute_command();
}

void FligthController::on_command_received(Command cmd) {
    _desired = cmd;

    compute_command();
}

void FligthController::compute_command() {
    Command cmd;
    cmd.motor_H_G = _desired.motor_H_G + _pitch_pid->get_output() - _yaw_pid->get_output();
    cmd.motor_H_D = _desired.motor_H_D - _roll_pid->get_output() - _yaw_pid->get_output();
    cmd.motor_B_G = _desired.motor_B_G + _roll_pid->get_output() + _yaw_pid->get_output();
    cmd.motor_B_D = _desired.motor_B_D - _pitch_pid->get_output() + _yaw_pid->get_output();

    _drone.HG.speed = map(cmd.motor_H_G);
    _drone.HD.speed = map(cmd.motor_H_D);
    _drone.BG.speed = map(cmd.motor_B_G);
    _drone.BD.speed = map(cmd.motor_B_D);

    //qDebug() << " HG " << QString::number(_drone.HG.speed) << " HD " << QString::number(_drone.HD.speed)
    //         << " BG " << QString::number(_drone.BG.speed) << " BD " << QString::number(_drone.BD.speed);

    #ifdef FILE_LOG
        if(_start_file==0)
            _start_file = QDateTime::currentMSecsSinceEpoch();
        *_stream << QString::number(QDateTime::currentMSecsSinceEpoch()-_start_file) << "," << QString::number(_drone.HG.speed) << "," <<
                  QString::number(_drone.HD.speed) << "," << QString::number(_drone.BG.speed) << "," << QString::number(_drone.BD.speed) << "\n";
    #endif

    #ifndef DEBUG
        //gpioServo(_drone.HG.pin, _drone.HG.speed);
        gpioServo(_drone.HD.pin, _drone.HD.speed);
        gpioServo(_drone.BG.pin, _drone.BG.speed);
        //gpioServo(_drone.BD.pin, _drone.BD.speed);
    #endif
}

void FligthController::on_connection_lost() {

}

void FligthController::on_connection_recovered() {

}

FligthController::~FligthController() {
    #ifndef DEBUG
        gpioServo(_drone.HG.pin, MIN_WIDTH);
        gpioServo(_drone.HD.pin, MIN_WIDTH);
        gpioServo(_drone.BG.pin, MIN_WIDTH);
        gpioServo(_drone.BD.pin, MIN_WIDTH);
    #endif
}

unsigned short FligthController::map(unsigned short x) {
    return (x - MIN_INT) * (MAX_WIDTH - MIN_WIDTH) / (MAX_INT - MIN_INT) + MIN_WIDTH;
}

void FligthController::text() {
    QTextStream qin(stdin);
    QString line = qin.readLine();
    switch(line.section(" ",0,0).at(0).toLatin1()) {
    case 'y':
    case 'Y':
        _yaw_pid->setParameters(line.section(" ",1,1).toDouble(), line.section(" ",2,2).toDouble(), line.section(" ",3,3).toDouble());
        break;
    case 'p':
    case 'P':
        _pitch_pid->setParameters(line.section(" ",1,1).toDouble(), line.section(" ",2,2).toDouble(), line.section(" ",3,3).toDouble());
        break;
    case 'r':
    case 'R':
        _roll_pid->setParameters(line.section(" ",1,1).toDouble(), line.section(" ",2,2).toDouble(), line.section(" ",3,3).toDouble());
        break;
    }

}
