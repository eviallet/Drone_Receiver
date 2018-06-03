#ifndef FLIGTHCONTROLLER_H
#define FLIGTHCONTROLLER_H

#include <QObject>
#include <QDebug>
#include <QSocketNotifier>
#include <unistd.h>
#include "packet.h"
#include "corrector.h"
#include "gyro.h"

#define DEBUG
#define FILE_LOG

#ifndef DEBUG
    #include <pigpio.h>
#endif

#ifdef FILE_LOG
    #include <QFile>
    #include <QDateTime>
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

class FligthController : public QObject {
    Q_OBJECT
public:
    explicit FligthController(QObject *parent = 0);
    ~FligthController();
signals:

public slots:
    void on_command_received(Command);
    void on_connection_lost();
    void on_connection_recovered();
private slots:
    unsigned short map(unsigned short x);
    void compute_command();
    void on_sensors_updated(double roll, double pitch);
    void text();
private:
    Quadrocopter drone;
    Gyro *_gyro;
    Corrector *_pitch_pid, *_roll_pid; //, *_yaw_pid;
    Command _desired;
    QSocketNotifier notifier;

    #ifdef FILE_LOG
        QFile *file;
        QTextStream *stream;
        qint64 start;
    #endif
};

#endif // FLIGTHCONTROLLER_H
