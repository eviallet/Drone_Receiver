#ifndef RECEIVER_H
#define RECEIVER_H

#include <QNetworkInterface>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QTimer>

#include "packet.h"

#define ALLOWED_FLOATING_TIME 300   // max time in millis without being pinged before triggering connection_lost

class Receiver : public QObject {
    Q_OBJECT
public:
    Receiver();
signals:
    void connection_lost();
    void connection_recovered();
    void command_received(Command);
private slots:
    void on_server_data_newConnection();
    void on_socket_data_readyRead();
    void on_socket_data_disconnected();
    void on_socket_data_error(QAbstractSocket::SocketError error);
/*
    void on_server_ping_newConnection();
    void on_socket_ping_readyRead();
    void on_socket_ping_disconnected();
    void on_socket_ping_error(QAbstractSocket::SocketError error);
    void on_connection_lost();
    void on_connection_recovered();
*/
    void init_servers();
private:
    QTcpServer *_server_data;
    //QTcpServer *_server_ping;
    QTcpSocket *_socket_data;
    //QTcpSocket *_socket_ping;

    //QTimer *_timer;
    Command _last_cmd;
};

#endif // RECEIVER_H
