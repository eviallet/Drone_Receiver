#include "receiver.h"

Receiver::Receiver() {
    _last_cmd.speed = 0;
    _last_cmd.roll = 0;
    _last_cmd.pitch = 0;
    _last_cmd.yaw = 0;

    _socket_data = Q_NULLPTR;
    _server_data = new QTcpServer;

    connect(_server_data, SIGNAL(newConnection()), this, SLOT(on_server_data_newConnection()));
    init_servers();
}


void Receiver::init_servers() {
    for(int i=0; i<QNetworkInterface::allAddresses().size(); i++)
        if(QNetworkInterface::allAddresses().at(i).toString().contains("192.168.")) {
            if(!_server_data->isListening()&&((_socket_data!=Q_NULLPTR&&_socket_data->state()!=QAbstractSocket::ConnectedState)||_socket_data==Q_NULLPTR)) {
                if(_server_data->listen(QNetworkInterface::allAddresses().at(i), 50000))
                    qDebug() << "Data - Listening on " << _server_data->serverAddress().toString();
            }
            break;
        }
}

void Receiver::on_server_data_newConnection() {
    _socket_data = _server_data->nextPendingConnection();
    connect(_socket_data, SIGNAL(readyRead()), this, SLOT(on_socket_data_readyRead()));
    connect(_socket_data, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_socket_data_error(QAbstractSocket::SocketError)));
    connect(_socket_data, SIGNAL(disconnected()), this, SLOT(on_socket_data_disconnected()));
    qDebug() << "Data - Device connected";
    _server_data->close();
}

void Receiver::on_socket_data_readyRead() {
    while((_socket_data->bytesAvailable()%(4*sizeof(short))==0)&&_socket_data->bytesAvailable()!=0) {
        QByteArray received = _socket_data->read(4*sizeof(short));
        Command cmd;
        memcpy(cmd.Bytes, received.data(), 4*sizeof(short));

        emit(command_received(cmd));
    }

    if(_socket_data->bytesAvailable()>0)
        _socket_data->readAll();
}


void Receiver::update_remote_graph(SensorData s) {
    if(_socket_data!=Q_NULLPTR&&_socket_data->state()==QTcpSocket::ConnectedState)
        _socket_data->write(sensor_to_bytes(s));
}

void Receiver::on_socket_data_disconnected() {
    qWarning() << "Data - Disconnected";
    init_servers();
}

void Receiver::on_socket_data_error(QAbstractSocket::SocketError error) {
    qDebug() << "Data - Error : " << error;
}

