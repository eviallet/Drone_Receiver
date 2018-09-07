#ifndef RECEIVER_H
#define RECEIVER_H

#include <QNetworkInterface>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QTimer>

#include "packet.h"


class Receiver : public QObject {
    Q_OBJECT
public:
    Receiver();
signals:
    void command_received(Command);
public slots:
    void update_remote_graph(SensorData s);
private slots:
    void on_server_data_newConnection();
    void on_socket_data_readyRead();
    void on_socket_data_disconnected();
    void on_socket_data_error(QAbstractSocket::SocketError error);
    void init_servers();
private:
    QTcpServer *_server_data;
    QTcpSocket *_socket_data;

    Command _last_cmd;
};

#endif // RECEIVER_H
