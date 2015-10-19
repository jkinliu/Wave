#ifndef TCPCLIENT_H
#define TCPCLIENT_H


#include <QString>
#include <QByteArray>
#include <QTcpSocket>

class TcpClient : public QTcpSocket
{
public:
    TcpClient();
    ~TcpClient();
    void connect_host(QString ip, quint16 port);
    void disconnect_host();
    bool is_valid();
    int send_data(QByteArray data);
    QByteArray recv_data();
};

#endif // TCPCLIENT_H
