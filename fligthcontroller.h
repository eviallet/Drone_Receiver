#ifndef FLIGTHCONTROLLER_H
#define FLIGTHCONTROLLER_H

#include <QObject>
#include <QDebug>
#include <pigpio.h>
#include "packet.h"

#define MAX_WIDTH 2000
#define MIN_WIDTH 1000


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
    unsigned short map(unsigned short x, int in_min, int in_max, int out_min, int out_max);
private:
    Quadrocopter drone;
};

#endif // FLIGTHCONTROLLER_H
