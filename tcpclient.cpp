#include "tcpclient.h"

TcpClient::TcpClient()
{
}

TcpClient::~TcpClient()
{

}

void TcpClient::connect_host(QString ip, quint16 port)
{
    if (ip.isEmpty() || (port < 1024))
        return;

    connectToHost(ip, port);
}

void TcpClient::disconnect_host()
{
    waitForBytesWritten(10);
    disconnectFromHost();
}

bool TcpClient::is_valid()
{
    return isValid();
}

int TcpClient:: send_data(QByteArray data)
{
    return write(data);
}

QByteArray TcpClient::recv_data()
{
    return readAll();
}

