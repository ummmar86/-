#include <QtTest>
#include <QDebug>
#include <iostream>
#include "../MathClient/clientcontroller.h"

/*
 * Unit-тесты клиента: ClientController (формирование команд и разбор ответов).
 * Форматы команд — из спецификации (REQ-REG-2, REQ-AUTH-1, REQ-MATH-3).
 */
class ClientTests : public QObject
{
    Q_OBJECT

private slots:
    // Исходные тесты
    void testMakeLoginCommand();
    void testMakeRegisterCommand();
    void testMakeSolveTaskCommand();
    void testParseServerResponse();

    // Новые тесты
    void testSingleton();
    void testMakeSolveTaskCommandAllFunctions_data();
    void testMakeSolveTaskCommandAllFunctions();
    void testSolveCommandHasFiveParts();
    void testParseRealServerResponses_data();
    void testParseRealServerResponses();
    void testParseResultSplitsIntoTwoRoots();

    // Шаг метода Ньютона
    void testMakeNewtonStepCommand();
    void testMakeNewtonStepCommandAutoDerivative();
};

void ClientTests::testMakeLoginCommand()
{
    std::cout << "testMakeLoginCommand started" << std::endl;
    QString cmd = ClientController::getInstance()->makeLoginCommand("test_user", "password123");
    QCOMPARE(cmd, QString("auth test_user password123"));
    std::cout << "testMakeLoginCommand passed" << std::endl;
}

void ClientTests::testMakeRegisterCommand()
{
    QString cmd = ClientController::getInstance()->makeRegisterCommand("new_user", "secure_pass");
    QCOMPARE(cmd, QString("reg new_user secure_pass"));
}

void ClientTests::testMakeSolveTaskCommand()
{
    QString cmd = ClientController::getInstance()->makeSolveTaskCommand(1, -2.5, 2.5, 0.0001);
    QCOMPARE(cmd, QString("solve 1 -2.5 2.5 0.0001"));
}

void ClientTests::testParseServerResponse()
{
    QCOMPARE(ClientController::getInstance()->parseServerResponse("success"), QString("SUCCESS"));
    QCOMPARE(ClientController::getInstance()->parseServerResponse("failed"), QString("FAILED"));
    QCOMPARE(ClientController::getInstance()->parseServerResponse("ALREADY_EXISTS"), QString("ALREADY_EXISTS"));
}

void ClientTests::testSingleton()
{
    qDebug() << "Starting testSingleton...";
    // getInstance() всегда возвращает один и тот же объект
    QVERIFY(ClientController::getInstance() != nullptr);
    QCOMPARE(ClientController::getInstance(), ClientController::getInstance());
    qDebug() << "testSingleton passed";
}

void ClientTests::testMakeSolveTaskCommandAllFunctions_data()
{
    QTest::addColumn<int>("id");
    QTest::addColumn<double>("a");
    QTest::addColumn<double>("b");
    QTest::addColumn<double>("eps");
    QTest::addColumn<QString>("expected");

    // Значения как в полях MainForm по умолчанию и в тест-кейсах
    QTest::newRow("x^2-4")      << 0 << 0.0 << 5.0 << 0.001 << "solve 0 0 5 0.001";
    QTest::newRow("sin(x)-x/2") << 1 << 1.0 << 3.0 << 0.001 << "solve 1 1 3 0.001";
    QTest::newRow("e^x-2")      << 2 << 0.0 << 2.0 << 0.001 << "solve 2 0 2 0.001";
    QTest::newRow("дробные")    << 0 << -0.75 << 1.25 << 1e-05 << "solve 0 -0.75 1.25 1e-05";
}

void ClientTests::testMakeSolveTaskCommandAllFunctions()
{
    QFETCH(int, id);
    QFETCH(double, a);
    QFETCH(double, b);
    QFETCH(double, eps);
    QFETCH(QString, expected);
    QCOMPARE(ClientController::getInstance()->makeSolveTaskCommand(id, a, b, eps), expected);
}

void ClientTests::testSolveCommandHasFiveParts()
{
    // Сервер ожидает ровно: solve <funcId> <a> <b> <eps>, и числа должны читаться обратно
    QString cmd = ClientController::getInstance()->makeSolveTaskCommand(2, -1.5, 3.25, 0.0005);
    QStringList parts = cmd.split(' ');
    QCOMPARE(parts.size(), 5);
    QCOMPARE(parts[0], QString("solve"));
    QCOMPARE(parts[1].toInt(), 2);
    QCOMPARE(parts[2].toDouble(), -1.5);
    QCOMPARE(parts[3].toDouble(), 3.25);
    QCOMPARE(parts[4].toDouble(), 0.0005);
}

void ClientTests::testParseRealServerResponses_data()
{
    // Ответы, которые реально отправляет MathServer (заглавными буквами)
    QTest::addColumn<QString>("response");
    QTest::addColumn<QString>("expected");

    QTest::newRow("SUCCESS")         << "SUCCESS"           << "SUCCESS";
    QTest::newRow("FAILED")          << "FAILED"            << "FAILED";
    QTest::newRow("ALREADY_EXISTS")  << "ALREADY_EXISTS"    << "ALREADY_EXISTS";
    QTest::newRow("результат solve") << "2.000122|2.000000" << "2.000122|2.000000";
    QTest::newRow("UNKNOWN COMMAND") << "UNKNOWN COMMAND"   << "UNKNOWN COMMAND";
    QTest::newRow("пустой ответ")    << ""                  << "";
}

void ClientTests::testParseRealServerResponses()
{
    QFETCH(QString, response);
    QFETCH(QString, expected);
    QCOMPARE(ClientController::getInstance()->parseServerResponse(response), expected);
}

void ClientTests::testParseResultSplitsIntoTwoRoots()
{
    // Так MainForm разбирает ответ solve (REQ-MATH-7)
    QStringList parts = QString("FAILED|0.693147").split('|');
    QCOMPARE(parts.size(), 2);
    QCOMPARE(parts[0], QString("FAILED"));
    QCOMPARE(parts[1].toDouble(), 0.693147);
}

void ClientTests::testMakeNewtonStepCommand()
{
    QCOMPARE(ClientController::getInstance()->makeNewtonStepCommand(0, 3.0, 6.0),
             QString("step 0 3 6"));
    QCOMPARE(ClientController::getInstance()->makeNewtonStepCommand(2, -1.5, 0.25),
             QString("step 2 -1.5 0.25"));
}

void ClientTests::testMakeNewtonStepCommandAutoDerivative()
{
    // Без производной команда состоит из трёх слов
    QString cmd = ClientController::getInstance()->makeNewtonStepCommand(1, 2.0);
    QCOMPARE(cmd, QString("step 1 2"));
    QCOMPARE(cmd.split(' ').size(), 3);
}

QTEST_MAIN(ClientTests)
#include "main.moc"
