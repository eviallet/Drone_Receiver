#ifndef FLIGTHCONTROLLER_H
#define FLIGTHCONTROLLER_H

#include <QObject>
#include <QDebug>
#include <wiringPi.h>
#include <softPwm.h>
#include "packet.h"

typedef struct {
    int pin;
    short speed;
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
private:
    Quadrocopter drone;
};

#endif // FLIGTHCONTROLLER_H
