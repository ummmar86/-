/**
 * @file parsing.h
 * @brief Разбор текстовых команд клиента на сервере.
 */
#ifndef PARSING_H
#define PARSING_H

#include <QObject>
#include "database.h"
#include "mathsolver.h"

/**
 * @brief Разбирает команду клиента, выполняет её и формирует ответ.
 *
 * Протокол (слова разделяются пробелом):
 * | Команда                          | Ответ сервера                         |
 * |----------------------------------|---------------------------------------|
 * | `reg <login> <password>`         | `SUCCESS` или `ALREADY_EXISTS`        |
 * | `auth <login> <password>`        | `SUCCESS` или `FAILED`                |
 * | `solve <funcId> <a> <b> <eps>`   | bisection\|newton, напр. 2.000122\|2.000000; вместо числа может быть `FAILED` |
 * | `step <funcId> <x0> [<f'(x0)>]`  | x1 = x0 - f(x0)/f'(x0), напр. 2.500000; `FAILED`, если f'(x0) = 0 или ввод неверный |
 * | любая другая                     | `UNKNOWN COMMAND`                     |
 */
class CommandParsing : public QObject
{
    Q_OBJECT
private:
    Database* my_database; ///< База данных (синглтон).

    /**
     * @brief Логин, под которым записывается история.
     * @bug Дефект №3: поле одно на весь сервер, а не на каждое подключение.
     *      Если вошли несколько клиентов, все расчёты записываются под логином
     *      последнего вошедшего.
     */
    QString currentLogin;

public:
    /**
     * @brief Открывает mathserver.db и создаёт таблицы.
     * @param parent Родительский объект Qt.
     */
    explicit CommandParsing(QObject *parent = nullptr);

    /**
     * @brief Выполняет одну команду клиента.
     * @param command Строка команды, например "solve 0 0 5 0.001".
     * @return Ответ, который сервер отправит клиенту.
     * @bug Дефект №2: команда solve выполняется без авторизации (нарушение SEC-1, BR-2).
     * @bug Дефект №7: пароль с пробелом обрезается до первого слова.
     */
    QString Command(const QString &command);
};

#endif // PARSING_H
