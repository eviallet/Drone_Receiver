#include <QCoreApplication>
#include "receiver.h"
#include "fligthcontroller.h"
#include "packet.h"


int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    Receiver *drone = new Receiver;
    FligthController *controller = new FligthController;

    drone->connect(drone, &Receiver::command_received, controller, &FligthController::on_command_received);
    drone->connect(drone, &Receiver::connection_lost, controller, &FligthController::on_connection_lost);
    drone->connect(drone, &Receiver::connection_recovered, controller, &FligthController::on_connection_recovered);

    controller->start(QThread::HighestPriority);

    return a.exec();
}

