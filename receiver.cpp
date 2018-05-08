#include "receiver.h"

Receiver::Receiver() {
    _timer = new QTimer;
    _timer->setInterval(ALLOWED_FLOATING_TIME);
    _timer->setSingleShot(true);
    connect(_timer, &QTimer::timeout, this, &Receiver::on_connection_lost);

    _server_data = new QTcpServer;
    _server_ping = new QTcpServer;
    connect(_server_data, SIGNAL(newConnection()), this, SLOT(on_server_data_newConnection()));
    connect(_server_ping, SIGNAL(newConnection()), this, SLOT(on_server_ping_newConnection()));
    init_servers();
}

void Receiver::init_servers() {
    for(int i=0; i<QNetworkInterface::allAddresses().size(); i++)
        if(QNetworkInterface::allAddresses().at(i).toString().contains("192.168.")) {
            if(!_server_data->isListening()&&((_socket_data!=nullptr&&_socket_data->state()!=QAbstractSocket::ConnectedState)||_socket_data==nullptr)) {
                if(_server_data->listen(QNetworkInterface::allAddresses().at(i), 50000))
                    qDebug() << "Data - Listening on " << _server_data->serverAddress().toString();
            }
            if(!_server_ping->isListening()&&((_socket_ping!=nullptr&&_socket_ping->state()!=QAbstractSocket::ConnectedState)||_socket_ping==nullptr)) {
                if(_server_ping->listen(QNetworkInterface::allAddresses().at(i), 50001))
                    qDebug() << "Ping - Listening on " << _server_data->serverAddress().toString();
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
    on_connection_recovered();
}

void Receiver::on_socket_data_readyRead() {
    _timer->stop();
    qDebug() << "Available bytes : " << _socket_data->bytesAvailable();
    while((_socket_data->bytesAvailable()%(4*sizeof(short))==0)&&_socket_data->bytesAvailable()!=0) {
        QByteArray received = _socket_data->read(4*sizeof(short));
        Command cmd;
        memcpy(cmd.Bytes, received.data(), 4*sizeof(short));
        //qDebug() << "Received : HG " << QString::number(cmd.motor_H_G) << " HD " << QString::number(cmd.motor_H_D)
        //         << " BG " << QString::number(cmd.motor_B_G) << " BD " << QString::number(cmd.motor_B_D);
        emit(command_received(cmd));
    }

    if(_socket_data->bytesAvailable()>0) {
        _buffer.append(_socket_data->readAll());
        qDebug() << "Buffer size : " << _buffer.size();
    }
    _timer->start();
}

void Receiver::on_socket_data_disconnected() {
    qWarning() << "Data - Disconnected";
    on_connection_lost();
    init_servers();
}

void Receiver::on_socket_data_error(QAbstractSocket::SocketError error) {
    qDebug() << "Data - Error : " << error;
}




void Receiver::on_server_ping_newConnection() {
    _socket_ping = _server_ping->nextPendingConnection();
    connect(_socket_ping, SIGNAL(readyRead()), this, SLOT(on_socket_ping_readyRead()));
    connect(_socket_ping, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_socket_ping_error(QAbstractSocket::SocketError)));
    connect(_socket_ping, SIGNAL(disconnected()), this, SLOT(on_socket_ping_disconnected()));
    qDebug() << "Ping - Device connected";
    _server_ping->close();
    on_connection_recovered();
}

void Receiver::on_socket_ping_readyRead() {
    _timer->stop();
    _socket_ping->write("0");
    _timer->start();
}

void Receiver::on_socket_ping_disconnected() {
    qDebug() << "Ping - Disconnected";
    on_connection_lost();
    init_servers();
}

void Receiver::on_socket_ping_error(QAbstractSocket::SocketError error) {
    qDebug() << "Ping - Error : " << error;
}

void Receiver::on_connection_lost() {
    emit(connection_lost());
}

void Receiver::on_connection_recovered() {
    emit(connection_recovered());
}
