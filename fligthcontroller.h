#ifndef FLIGTHCONTROLLER_H
#define FLIGTHCONTROLLER_H

#include <QDebug>
#include <QSocketNotifier>
#include <QThread>
#include <QDateTime>
#include <unistd.h>
#include "packet.h"
#include "corrector.h"
#include "sensor.h"
#define DEBUG
#define FILE_LOG

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

public slots:
    void on_command_received(Command);
    void on_connection_lost();
    void on_connection_recovered();
private slots:
    unsigned short map(unsigned short x);
    void compute_command();
    void update_angles(std::tuple<float,float,float>);
    void text();
private:
    Quadrocopter _drone;
    Corrector *_pitch_pid, *_roll_pid, *_yaw_pid;
    Command _desired;
    QSocketNotifier _notifier;
    Sensor *_gyro;
    qint64 _last;

    #ifdef FILE_LOG
        QFile *_file;
        QTextStream *_stream;
        qint64 _start_file;
    #endif
};

#endif // FLIGTHCONTROLLER_H
