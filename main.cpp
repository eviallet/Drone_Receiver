#include <QCoreApplication>
#include "receiver.h"
#include "fligthcontroller.h"
#include "packet.h"


int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    Receiver *receiver = new Receiver;
    FligthController *controller = new FligthController;

    receiver->connect(receiver, &Receiver::setpoint_received, controller, &FligthController::on_setpoint_received);
    receiver->connect(receiver, &Receiver::pid_params_received, controller, &FligthController::on_pid_params_received);
    receiver->connect(controller, &FligthController::sensor_data_changed, receiver, &Receiver::update_remote_graph);

    controller->start(QThread::HighestPriority);

    return a.exec();
}

