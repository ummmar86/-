/**
 * @file mytcpserver.h
 * @brief TCP-сервер MathServer.
 */
#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QByteArray>
#include "parsing.h"

/**
 * @brief Принимает подключения клиентов на порту 33333 и отвечает на их команды.
 *
 * Каждая полученная строка передаётся в CommandParsing::Command(),
 * а результат отправляется обратно тому же клиенту.
 */
class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Создаёт сервер и начинает слушать порт 33333 на всех интерфейсах.
     * @param parent Родительский объект Qt.
     */
    explicit MyTcpServer(QObject *parent = nullptr);

    /// Закрывает все клиентские сокеты и останавливает сервер.
    ~MyTcpServer();

public slots:
    /// Принимает новые подключения и подписывается на их сигналы.
    void slotNewConnection();
    /// Освобождает сокет отключившегося клиента (ROB-2).
    void slotClientDisconnected();
    /// Читает команду клиента, выполняет её и отправляет ответ.
    void slotServerRead();

private:
    CommandParsing* myparsing;            ///< Разбор и выполнение команд.
    QTcpServer *mTcpServer;               ///< Слушающий сокет.
    QMap<qintptr, QTcpSocket*> mSockets;  ///< Подключённые клиенты по дескриптору.
};

#endif // MYTCPSERVER_H
