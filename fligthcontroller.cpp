#include "fligthcontroller.h"

FligthController::FligthController(QObject *parent) : QThread(parent), _notifier(STDIN_FILENO, QSocketNotifier::Read) {
    connect(&_notifier, SIGNAL(activated(int)), this, SLOT(text()));

    qRegisterMetaType<SensorData>("SensorData");

    #ifdef FILE_LOG
        QFile::remove("/home/pi/Qt/data.txt");
        _file = new QFile("/home/pi/Qt/data.txt");
        if (_file->open(QIODevice::ReadWrite))
            _stream = new QTextStream(_file);
    #endif

    _desired.speed = 0;
    _desired.yaw = 0;
    _desired.pitch = 0;
    _desired.roll = 0;

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
    while(loop) {
        if(_last==0)
            _last = QDateTime::currentMSecsSinceEpoch();
        else if(QDateTime::currentMSecsSinceEpoch()-_last >= 1) {
            compute_command();
            _last = QDateTime::currentMSecsSinceEpoch();
        }
    }
}


void FligthController::on_command_received(Command cmd) {
    _desired = cmd;
}

void FligthController::compute_command() {
    std::tuple<float,float,float> ypr = _gyro->get_angles();
    //qDebug() << QString::number(std::get<0>(ypr)) << QString::number(std::get<1>(ypr)) << QString::number(std::get<2>(ypr));
    _yaw_pid->compute(std::get<0>(ypr));
    _pitch_pid->compute(std::get<1>(ypr));
    _roll_pid->compute(std::get<2>(ypr));
    //qDebug() << QString::number(_roll_pid->get_output());

    int droneHG = 0, droneHD = 0, droneBG = 0, droneBD = 0;
    if((droneHG = (int)_desired.speed + _roll_pid->get_output() - _yaw_pid->get_output())>=0 &&
            (droneHD = (int)_desired.speed + _pitch_pid->get_output() - _yaw_pid->get_output())>=0 &&
            (droneBG = (int)_desired.speed - _pitch_pid->get_output() + _yaw_pid->get_output())>=0 &&
            (droneBD = (int)_desired.speed - _roll_pid->get_output() + _yaw_pid->get_output())>=0) {
        _drone.HG.speed = map(droneHG);
        _drone.HD.speed = map(droneHD);
        _drone.BG.speed = map(droneBG);
        _drone.BD.speed = map(droneBD);
    } else {
        #ifndef DEBUG
            gpioServo(_drone.HG.pin, MIN_WIDTH);
            gpioServo(_drone.HD.pin, MIN_WIDTH);
            gpioServo(_drone.BG.pin, MIN_WIDTH);
            gpioServo(_drone.BD.pin, MIN_WIDTH);
        #endif
    }

    //qDebug() << " HG " << QString::number(_drone.HG.speed) << " HD " << QString::number(_drone.HD.speed)
    //         << " BG " << QString::number(_drone.BG.speed) << " BD " << QString::number(_drone.BD.speed);


    #ifdef FILE_LOG
        if(_start_file==0)
            _start_file = QDateTime::currentMSecsSinceEpoch();
        else {
            //*_stream << QString::number(QDateTime::currentMSecsSinceEpoch() - _start_file) << "," << QString::number(_drone.HG.speed) << "," <<
            //      QString::number(_drone.HD.speed) << "," << QString::number(_drone.BG.speed) << "," << QString::number(_drone.BD.speed) << "\n";
            *_stream << QString::number(QDateTime::currentMSecsSinceEpoch() - _start_file) << "," << QString::number(std::get<2>(ypr)) << "\n";
        }
    #endif

    #ifdef REMOTE_GRAPH
        if(QDateTime::currentMSecsSinceEpoch()-_last_sensor_data_time>REMOTE_GRAPH_UPDATE) {
            SensorData s;
            s.alt = 0;
            s.yaw = std::get<0>(ypr);
            s.pitch = std::get<1>(ypr);
            s.roll = std::get<2>(ypr);
            emit(sensor_data_changed(s));
            _last_sensor_data_time = QDateTime::currentMSecsSinceEpoch();
        }
    #endif

    #ifndef DEBUG
        gpioServo(_drone.HG.pin, _drone.HG.speed);
        //gpioServo(_drone.HD.pin, _drone.HD.speed);
        //gpioServo(_drone.BG.pin, _drone.BG.speed);
        gpioServo(_drone.BD.pin, _drone.BD.speed);
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

unsigned short FligthController::invmap(unsigned short x) {
    return (x - MIN_WIDTH) * (MAX_INT - MIN_INT) / (MAX_WIDTH - MIN_WIDTH) + MIN_INT;
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
    case 'd':
    case 'D':
        _desired.speed = invmap(line.section(" ",1,1).toShort());
        qDebug() << "Desired changed to " << QString::number(_desired.speed);
        break;
    case 's':
    case 'S':
        emergency_stop();
        break;
    }

}

void FligthController::emergency_stop() {
    loop = false;
    #ifndef DEBUG
        gpioServo(_drone.HG.pin, MIN_WIDTH);
        gpioServo(_drone.HD.pin, MIN_WIDTH);
        gpioServo(_drone.BG.pin, MIN_WIDTH);
        gpioServo(_drone.BD.pin, MIN_WIDTH);
    #endif
}
