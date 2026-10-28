#include "database.h"
#include <QDebug>
#include <QDir>

Database* Database::instance = nullptr;
DatabaseDestroyer Database::destroyer;

DatabaseDestroyer::~DatabaseDestroyer() { delete instance; }

Database::~Database() {
    if (db.isOpen()) {
        db.close();
    }
}

void Database::openDatabase(const QString& path) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    if (!db.open()) {
        qDebug() << "Failed to open DB:" << db.lastError().text();
    } else {
        qDebug() << "DB opened successfully!";
    }
}

void Database::createTables() {
    QSqlQuery query(db);
    // Table for Users
    if (!query.exec("CREATE TABLE IF NOT EXISTS Users ("
                    "login TEXT PRIMARY KEY, "
                    "password TEXT)")) {
        qDebug() << "Failed to create Users table:" << query.lastError().text();
    }
    
    // Table for History
    if (!query.exec("CREATE TABLE IF NOT EXISTS History ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "login TEXT, "
                    "functionId INTEGER, "
                    "a REAL, "
                    "b REAL, "
                    "eps REAL, "
                    "bisectionResult REAL, "
                    "newtonResult REAL)")) {
        qDebug() << "Failed to create History table:" << query.lastError().text();
    }
}

bool Database::registerUser(const QString& login, const QString& password) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO Users (login, password) VALUES (:login, :password)");
    query.bindValue(":login", login);
    query.bindValue(":password", password);
    return query.exec();
}

bool Database::authoUser(const QString& login, const QString& password) {
    QSqlQuery query(db);
    query.prepare("SELECT * FROM Users WHERE login = :login AND password = :password");
    query.bindValue(":login", login);
    query.bindValue(":password", password);
    if (query.exec() && query.next()) {
        return true;
    }
    return false;
}

void Database::saveTaskResult(const QString& login, int functionId, double a, double b, double eps, double bisectionResult, double newtonResult) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO History (login, functionId, a, b, eps, bisectionResult, newtonResult) "
                  "VALUES (:login, :functionId, :a, :b, :eps, :bisectionResult, :newtonResult)");
    query.bindValue(":login", login);
    query.bindValue(":functionId", functionId);
    query.bindValue(":a", a);
    query.bindValue(":b", b);
    query.bindValue(":eps", eps);
    query.bindValue(":bisectionResult", bisectionResult);
    query.bindValue(":newtonResult", newtonResult);
    if (!query.exec()) {
        qDebug() << "Failed to save history:" << query.lastError().text();
    }
}
