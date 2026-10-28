#include "mytcpclient.h"

MyTcpClient* MyTcpClient::instance = nullptr;
MyTcpClientDestroyer MyTcpClient::destroyer;

MyTcpClientDestroyer::~MyTcpClientDestroyer() { delete instance; }

void MyTcpClient::sendMessage(QString msg)
{
    if (!msocket || msocket->state() != QAbstractSocket::ConnectedState) {
        return;
    }
    msocket->write(msg.toUtf8());
}

QString MyTcpClient::getMsg()
{
    msocket->waitForReadyRead();
    QString res = "";
    while(msocket->bytesAvailable() > 0)
    {
        QByteArray array = msocket->readAll();
        if(array == "\x01") {
            res = "";
        } else {
            res.append(array);
        }
    }
    return res;
}

void MyTcpClient::slotReadyRead()
{
    // If asynchronous reading is preferred, we can emit a signal here
    // However, following the example, getMsg() is used synchronously.
    // If data comes unexpectedly:
    if (msocket->bytesAvailable() > 0) {
        QByteArray array = msocket->readAll();
        emit messageReceived(QString(array));
    }
}
