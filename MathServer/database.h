/**
 * @file database.h
 * @brief Работа с базой данных SQLite (пользователи и история вычислений).
 */
#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class Database;

/**
 * @brief Вспомогательный класс, который удаляет синглтон Database при завершении программы.
 */
class DatabaseDestroyer
{
private:
    Database* instance; ///< Указатель на удаляемый экземпляр.
public:
    /// Удаляет экземпляр Database.
    ~DatabaseDestroyer();
    /// Запоминает экземпляр, который нужно будет удалить.
    void initialize(Database* p) { instance = p; }
};

/**
 * @brief Доступ к базе данных mathserver.db (паттерн «Одиночка»).
 *
 * Таблицы:
 * - **Users** (login TEXT PRIMARY KEY, password TEXT) — учётные записи;
 * - **History** (id, login, functionId, a, b, eps, bisectionResult, newtonResult) — история расчётов.
 *
 * Все запросы используют подготовленные выражения (prepare/bindValue),
 * поэтому SQL-инъекции через логин и пароль невозможны.
 */
class Database : public QObject
{
    Q_OBJECT
private:
    static Database* instance;          ///< Единственный экземпляр.
    static DatabaseDestroyer destroyer; ///< Удаляет экземпляр при выходе.
    QSqlDatabase db;                    ///< Соединение с SQLite.

protected:
    Database() : QObject(nullptr) {}
    Database(const Database&);
    Database& operator=(Database&);
    ~Database();
    friend class DatabaseDestroyer;

public:
    /**
     * @brief Возвращает единственный экземпляр, создавая его при первом вызове.
     * @return Указатель на Database.
     */
    static Database* getInstance() {
        if (!instance) {
            instance = new Database();
            destroyer.initialize(instance);
        }
        return instance;
    }

    /**
     * @brief Открывает (или создаёт) файл базы данных через драйвер QSQLITE.
     * @param path Путь к файлу, например "mathserver.db".
     */
    void openDatabase(const QString& path);

    /// Создаёт таблицы Users и History, если их ещё нет.
    void createTables();

    /**
     * @brief Регистрирует нового пользователя.
     * @param login    Логин (первичный ключ таблицы Users).
     * @param password Пароль (хранится в открытом виде).
     * @return true — пользователь добавлен; false — логин уже занят.
     * @bug Дефект №6: пустой логин не отклоняется и успешно регистрируется.
     */
    bool registerUser(const QString& login, const QString& password);

    /**
     * @brief Проверяет пару логин/пароль.
     * @return true, если такая пара есть в таблице Users.
     */
    bool authoUser(const QString& login, const QString& password);

    /**
     * @brief Сохраняет результат расчёта в таблицу History.
     * @param login           Логин пользователя.
     * @param functionId      Идентификатор уравнения.
     * @param a               Левая граница.
     * @param b               Правая граница.
     * @param eps             Точность.
     * @param bisectionResult Корень по методу половинного деления.
     * @param newtonResult    Корень по методу Ньютона.
     * @bug Дефект №11: если метод не сошёлся, в History пишется 0 вместо статуса FAILED,
     *      и неудачный расчёт неотличим от корня x = 0.
     */
    void saveTaskResult(const QString& login, int functionId, double a, double b, double eps, double bisectionResult, double newtonResult);
};

#endif // DATABASE_H
