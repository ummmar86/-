#include "mytcpserver.h"
#include <QDebug>
#include "parsing.h"

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent)
{
    myparsing = new CommandParsing(this);
    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection, this, &MyTcpServer::slotNewConnection);

    if (!mTcpServer->listen(QHostAddress::Any, 33333)) {
        qDebug() << "Server is not started:" << mTcpServer->errorString();
    } else {
        qDebug() << "Server is started on port" << mTcpServer->serverPort();
    }
}

MyTcpServer::~MyTcpServer()
{
    for (auto sock : mSockets) {
        if (sock) {
            sock->disconnectFromHost();
            sock->close();
            sock->deleteLater();
        }
    }
    mSockets.clear();

    if (mTcpServer->isListening()) {
        mTcpServer->close();
    }
}

void MyTcpServer::slotNewConnection()
{
    while (mTcpServer->hasPendingConnections()) {
        QTcpSocket *clientSocket = mTcpServer->nextPendingConnection();
        if (!clientSocket) continue;

        qintptr id = clientSocket->socketDescriptor();
        mSockets[id] = clientSocket;

        qDebug() << "New client connected, id =" << id;

        connect(clientSocket, &QTcpSocket::readyRead, this, &MyTcpServer::slotServerRead);
        connect(clientSocket, &QTcpSocket::disconnected, this, &MyTcpServer::slotClientDisconnected);
    }
}

void MyTcpServer::slotServerRead()
{
    QTcpSocket *mTcpSocket = qobject_cast<QTcpSocket*>(sender());
    if (!mTcpSocket) return;

    QString resp = "";
    while(mTcpSocket->bytesAvailable() > 0)
    {
        QByteArray array = mTcpSocket->readAll();
        QString dataStr = QString::fromUtf8(array).trimmed();
        qDebug() << "Received:" << dataStr;
        resp = myparsing->Command(dataStr);
    }
    
    qDebug() << "Sending:" << resp;
    // Client expects '\x01' or similar in example? The MathClient `getMsg` stops if `\x01` is received, but we didn't add it in our sending.
    // Wait, the client uses `getMsg()` which reads everything in the socket, and blocks.
    mTcpSocket->write(resp.toUtf8());
}

void MyTcpServer::slotClientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) return;

    qintptr id = socket->socketDescriptor();
    qDebug() << "Client disconnected, id =" << id;

    mSockets.remove(id);
    socket->close();
    socket->deleteLater();
}
