/**
 * @file mytcpclient.h
 * @brief Сетевое подключение клиента к серверу.
 */
#ifndef MYTCPCLIENT_H
#define MYTCPCLIENT_H

#include <QTcpSocket>
#include <QObject>

class MyTcpClient;

/**
 * @brief Удаляет синглтон MyTcpClient при завершении программы.
 */
class MyTcpClientDestroyer
{
private:
    MyTcpClient* instance; ///< Указатель на удаляемый экземпляр.
public:
    /// Удаляет экземпляр MyTcpClient.
    ~MyTcpClientDestroyer();
    /// Запоминает экземпляр, который нужно будет удалить.
    void initialize(MyTcpClient * p) { instance = p; }
};

/**
 * @brief TCP-соединение клиента с сервером 127.0.0.1:33333 (паттерн «Одиночка»).
 *
 * Формы отправляют команду через sendMessage() и сразу ждут ответ через getMsg().
 */
class MyTcpClient : public QObject
{
    Q_OBJECT
private:
    QTcpSocket* msocket;                    ///< Сокет соединения с сервером.
    static MyTcpClient* instance;           ///< Единственный экземпляр.
    static MyTcpClientDestroyer destroyer;  ///< Удаляет экземпляр при выходе.

protected:
    /**
     * @brief Создаёт сокет и подключается к серверу.
     * @bug Дефект №10: подключение выполняется только здесь, один раз.
     *      Если сервер был недоступен при запуске клиента, клиент не переподключается,
     *      а форма показывает «Login failed» вместо сообщения об отсутствии связи.
     */
    MyTcpClient() : QObject(nullptr) {
        msocket = new QTcpSocket(this);
        msocket->connectToHost("127.0.0.1", 33333);
        connect(msocket, &QTcpSocket::connected, this, &MyTcpClient::connected);
        connect(msocket, &QTcpSocket::disconnected, this, &MyTcpClient::disconnected);
    }
    MyTcpClient(const MyTcpClient&);
    MyTcpClient& operator=(MyTcpClient&);
    ~MyTcpClient() {}
    friend class MyTcpClientDestroyer;

public:
    /**
     * @brief Возвращает единственный экземпляр, создавая его (и подключение) при первом вызове.
     */
    static MyTcpClient* getInstance() {
        if (!instance) {
            instance = new MyTcpClient();
            destroyer.initialize(instance);
        }
        return instance;
    }

    /**
     * @brief Отправляет команду серверу в кодировке UTF-8.
     * @param msg Текст команды. Если соединения нет, сообщение молча не отправляется.
     */
    void sendMessage(QString msg);

    /**
     * @brief Синхронно ждёт и читает ответ сервера.
     * @return Ответ сервера или пустая строка, если ответа нет.
     */
    QString getMsg(); // Synchronous read (from example)

signals:
    /// Соединение с сервером установлено.
    void connected();
    /// Соединение с сервером разорвано.
    void disconnected();
    /// Получено сообщение от сервера (для асинхронного режима).
    void messageReceived(QString msg);

private slots:
    /// Читает данные из сокета и испускает messageReceived().
    void slotReadyRead();
};

#endif // MYTCPCLIENT_H
