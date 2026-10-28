/**
 * @file clientcontroller.h
 * @brief Формирование команд для сервера и разбор его ответов (клиент).
 */
#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#include <QObject>
#include <QString>

class ClientController;

/**
 * @brief Удаляет синглтон ClientController при завершении программы.
 */
class ClientControllerDestroyer
{
private:
    ClientController* instance; ///< Указатель на удаляемый экземпляр.
public:
    /// Удаляет экземпляр ClientController.
    ~ClientControllerDestroyer();
    /// Запоминает экземпляр, который нужно будет удалить.
    void initialize(ClientController * p) { instance = p; }
};

/**
 * @brief Логика клиента без интерфейса и сети (паттерн «Одиночка»).
 *
 * Собирает текстовые команды протокола и переводит ответы сервера
 * в понятные формам статусы. Благодаря отделению от GUI класс покрыт unit-тестами.
 */
class ClientController : public QObject
{
    Q_OBJECT
private:
    static ClientController* instance;          ///< Единственный экземпляр.
    static ClientControllerDestroyer destroyer; ///< Удаляет экземпляр при выходе.

protected:
    ClientController() : QObject(nullptr) {}
    ClientController(const ClientController&);
    ClientController& operator=(ClientController&);
    ~ClientController() {}
    friend class ClientControllerDestroyer;

public:
    /**
     * @brief Возвращает единственный экземпляр, создавая его при первом вызове.
     */
    static ClientController* getInstance() {
        if (!instance) {
            instance = new ClientController();
            destroyer.initialize(instance);
        }
        return instance;
    }

    /**
     * @brief Команда входа.
     * @return Строка "auth <login> <password>".
     */
    QString makeLoginCommand(QString login, QString password);

    /**
     * @brief Команда регистрации.
     * @return Строка "reg <login> <password>".
     */
    QString makeRegisterCommand(QString login, QString password);

    /**
     * @brief Команда решения уравнения.
     * @param functionId Идентификатор уравнения (0, 1, 2).
     * @param a          Левая граница.
     * @param b          Правая граница.
     * @param eps        Точность.
     * @return Строка "solve <functionId> <a> <b> <eps>".
     */
    QString makeSolveTaskCommand(int functionId, double a, double b, double eps);

    /**
     * @brief Команда одного шага метода Ньютона с заданной производной.
     * @param functionId Идентификатор уравнения (0, 1, 2).
     * @param x0         Начальное приближение.
     * @param dfx0       Значение производной f'(x0).
     * @return Строка "step <functionId> <x0> <dfx0>".
     */
    QString makeNewtonStepCommand(int functionId, double x0, double dfx0);

    /**
     * @brief Команда одного шага метода Ньютона, производную считает сервер.
     * @return Строка "step <functionId> <x0>".
     */
    QString makeNewtonStepCommand(int functionId, double x0);

    /**
     * @brief Переводит ответ сервера в статус для формы.
     * @param response Ответ сервера.
     * @return "SUCCESS", "FAILED", "ALREADY_EXISTS" или исходная строка без изменений.
     */
    QString parseServerResponse(QString response);
};

#endif // CLIENTCONTROLLER_H
