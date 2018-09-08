#ifndef FLIGTHCONTROLLER_H
#define FLIGTHCONTROLLER_H

#include <QDebug>
#include <QMetaType>
#include <QSocketNotifier>
#include <QThread>
#include <QDateTime>
#include <unistd.h>
#include "packet.h"
#include "corrector.h"
#include "sensor.h"

#define DEBUG
//#define FILE_LOG
#define REMOTE_GRAPH

#ifdef REMOTE_GRAPH
    #define REMOTE_GRAPH_UPDATE 50
#endif

#ifndef DEBUG
    #include <pigpio.h>
#endif

#ifdef FILE_LOG
    #include <QFile>
#endif

#define MAX_WIDTH 2000
#define MIN_WIDTH 1000
#define MAX_INT 65535
#define MIN_INT 0


typedef struct {
    unsigned int pin;
    unsigned short speed;
} Motor;


typedef struct {
    Motor HG;
    Motor HD;
    Motor BG;
    Motor BD;
} Quadrocopter;

class FligthController : public QThread {
    Q_OBJECT
public:
    explicit FligthController(QObject *parent = 0);
    void run() override;
    ~FligthController();
signals:
    void sensor_data_changed(SensorData);
public slots:
    void on_setpoint_received(SetPoint);
    void on_pid_params_received(PIDParams);
    void on_connection_lost();
    void on_connection_recovered();
private slots:
    unsigned short map(unsigned short x);
    unsigned short invmap(unsigned short x);
    void compute_command();
    void text();
    void emergency_stop();
private:
    Quadrocopter _drone;
    Corrector *_pitch_pid, *_roll_pid, *_yaw_pid;
    SetPoint _desired;
    QSocketNotifier _notifier;
    Sensor *_gyro;
    qint64 _last=0;

    #ifdef FILE_LOG
        QFile *_file;
        QTextStream *_stream;
        qint64 _start_file=0;
    #endif

    #ifdef REMOTE_GRAPH
        qint64 _last_sensor_data_time = 0;
    #endif

    bool loop = true;
};

#endif // FLIGTHCONTROLLER_H
