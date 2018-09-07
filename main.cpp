#include <QCoreApplication>
#include "receiver.h"
#include "fligthcontroller.h"
#include "packet.h"


int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    Receiver *receiver = new Receiver;
    FligthController *controller = new FligthController;

    receiver->connect(receiver, &Receiver::command_received, controller, &FligthController::on_command_received);
    receiver->connect(controller, &FligthController::sensor_data_changed, receiver, &Receiver::update_remote_graph);

    controller->start(QThread::HighestPriority);

    return a.exec();
}

