#include <QtTest>
#include <QTemporaryDir>
#include <QSqlQuery>
#include "../MathServer/mathsolver.h"
#include "../MathServer/database.h"
#include "../MathServer/parsing.h"
#include <cmath>

/*
 * Unit-тесты сервера: MathSolver, Database, CommandParsing.
 *
 * Тесты, помеченные QEXPECT_FAIL, проверяют поведение по спецификации,
 * которое сейчас нарушено (известные дефекты из Testing_MathSolver_Real.xlsx).
 * Они выводятся как XFAIL и не валят прогон. Когда дефект исправят,
 * тест станет XPASS и упадёт — тогда пометку QEXPECT_FAIL нужно убрать.
 */
class ServerTests : public QObject
{
    Q_OBJECT

private:
    QTemporaryDir tempDir;             // база данных для тестов создаётся здесь, а не в рабочей папке
    CommandParsing* parser = nullptr;

private slots:
    void initTestCase();
    void cleanupTestCase();

    // --- MathSolver: исходные тесты ---
    void testGetFunctionName();
    void testBisectionSuccess();
    void testBisectionFailure();
    void testNewtonSuccess();
    void testNewtonFailure();

    // --- MathSolver: уравнения и производные ---
    void testFunctionValues_data();
    void testFunctionValues();
    void testDerivatives_data();
    void testDerivatives();
    void testGetFunctionNameUnknown();

    // --- MathSolver: методы на всех уравнениях ---
    void testBisectionAllFunctions_data();
    void testBisectionAllFunctions();
    void testNewtonAllFunctions_data();
    void testNewtonAllFunctions();
    void testBisectionPrecision_data();
    void testBisectionPrecision();
    void testBisectionRootOnBoundary();
    void testNewtonMaxIterations();

    // --- MathSolver: известные дефекты ---
    void testBisectionReversedInterval();   // Дефект №4
    void testBisectionNonPositiveEps();     // Дефект №5

    // --- Database ---
    void testDatabaseRegisterAndAuth();
    void testDatabaseDuplicateLogin();
    void testDatabaseSaveTaskResult();

    // --- CommandParsing: протокол ---
    void testSolveRequiresAuth();           // Дефект №2
    void testProtocolRegister();
    void testProtocolAuth();
    void testProtocolSolve();
    void testProtocolSolveInvalidInterval();
    void testProtocolBadCommands();
    void testProtocolSolveSavesHistory();
    void testProtocolUnknownFunctionId();   // Дефект №8
    void testProtocolEmptyLogin();          // Дефект №6
    void testProtocolPasswordWithSpace();   // Дефект №7

    // --- Шаг метода Ньютона: x1 = x0 - f(x0)/f'(x0) ---
    void testNewtonStepFormula_data();
    void testNewtonStepFormula();
    void testNewtonStepZeroDerivative();
    void testProtocolStepWithDerivative();
    void testProtocolStepServerDerivative();
    void testProtocolStepBadInput();
};

// ============================================================
// Подготовка
// ============================================================

void ServerTests::initTestCase()
{
    QVERIFY(tempDir.isValid());
    QDir::setCurrent(tempDir.path());        // CommandParsing открывает "mathserver.db" в текущей папке
    parser = new CommandParsing();           // открывает БД и создаёт таблицы
}

void ServerTests::cleanupTestCase()
{
    delete parser;
}

// ============================================================
// MathSolver: исходные тесты
// ============================================================

void ServerTests::testGetFunctionName()
{
    QCOMPARE(MathSolver::getFunctionName(0), QString("f(x) = x^2 - 4"));
    QCOMPARE(MathSolver::getFunctionName(1), QString("f(x) = sin(x) - x/2"));
}

void ServerTests::testBisectionSuccess()
{
    auto f = MathSolver::getFunction(0); // f(x) = x^2 - 4
    bool success = false;
    double root = MathSolver::solveBisection(f, 0.0, 5.0, 0.001, success);

    QVERIFY(success);
    QVERIFY(std::abs(root - 2.0) <= 0.001);
}

void ServerTests::testBisectionFailure()
{
    auto f = MathSolver::getFunction(0); // f(x) = x^2 - 4
    bool success = true;
    // Interval [3, 5] does not contain a root (both f(3)=5 and f(5)=21 are positive)
    MathSolver::solveBisection(f, 3.0, 5.0, 0.001, success);

    QVERIFY(!success);
}

void ServerTests::testNewtonSuccess()
{
    auto f = MathSolver::getFunction(0); // f(x) = x^2 - 4
    auto df = MathSolver::getDerivative(0); // f'(x) = 2x
    bool success = false;
    double root = MathSolver::solveNewton(f, df, 2.5, 0.001, success);

    QVERIFY(success);
    QVERIFY(std::abs(root - 2.0) <= 0.001);
}

void ServerTests::testNewtonFailure()
{
    auto f = MathSolver::getFunction(0); // f(x) = x^2 - 4
    auto df = MathSolver::getDerivative(0); // f'(x) = 2x
    bool success = true;
    // Initial guess 0.0 leads to derivative = 0 (division by zero)
    MathSolver::solveNewton(f, df, 0.0, 0.001, success);

    QVERIFY(!success);
}

// ============================================================
// MathSolver: уравнения и производные
// ============================================================

void ServerTests::testFunctionValues_data()
{
    QTest::addColumn<int>("id");
    QTest::addColumn<double>("x");
    QTest::addColumn<double>("expected");

    QTest::newRow("x^2-4 при x=0")      << 0 << 0.0 << -4.0;
    QTest::newRow("x^2-4 при x=3")      << 0 << 3.0 << 5.0;
    QTest::newRow("sin(x)-x/2 при x=0") << 1 << 0.0 << 0.0;
    QTest::newRow("sin(x)-x/2 при x=pi")<< 1 << M_PI << -M_PI / 2.0;
    QTest::newRow("e^x-2 при x=0")      << 2 << 0.0 << -1.0;
    QTest::newRow("e^x-2 при x=ln2")    << 2 << std::log(2.0) << 0.0;
}

void ServerTests::testFunctionValues()
{
    QFETCH(int, id);
    QFETCH(double, x);
    QFETCH(double, expected);
    QVERIFY(std::abs(MathSolver::getFunction(id)(x) - expected) < 1e-12);
}

void ServerTests::testDerivatives_data()
{
    QTest::addColumn<int>("id");
    QTest::addColumn<double>("x");

    for (int id = 0; id <= 2; ++id)
        for (double x : {-1.5, 0.3, 2.0})
            QTest::newRow(qPrintable(QString("f%1, x=%2").arg(id).arg(x))) << id << x;
}

void ServerTests::testDerivatives()
{
    // Аналитическая производная должна совпадать с численной (центральная разность)
    QFETCH(int, id);
    QFETCH(double, x);
    auto f = MathSolver::getFunction(id);
    auto df = MathSolver::getDerivative(id);
    const double h = 1e-6;
    double numeric = (f(x + h) - f(x - h)) / (2 * h);
    QVERIFY2(std::abs(df(x) - numeric) < 1e-6,
             qPrintable(QString("df=%1, численно=%2").arg(df(x)).arg(numeric)));
}

void ServerTests::testGetFunctionNameUnknown()
{
    QCOMPARE(MathSolver::getFunctionName(2), QString("f(x) = e^x - 2"));
    QCOMPARE(MathSolver::getFunctionName(3), QString("Unknown"));
    QCOMPARE(MathSolver::getFunctionName(-1), QString("Unknown"));
}

// ============================================================
// MathSolver: методы на всех уравнениях
// ============================================================

void ServerTests::testBisectionAllFunctions_data()
{
    QTest::addColumn<int>("id");
    QTest::addColumn<double>("a");
    QTest::addColumn<double>("b");
    QTest::addColumn<double>("root");

    QTest::newRow("x^2-4 на [0, 5]")         << 0 << 0.0 << 5.0 << 2.0;
    QTest::newRow("x^2-4 на [-5, 0]")        << 0 << -5.0 << 0.0 << -2.0;
    QTest::newRow("sin(x)-x/2 на [1, 3]")    << 1 << 1.0 << 3.0 << 1.895494267033981;
    QTest::newRow("e^x-2 на [0, 2]")         << 2 << 0.0 << 2.0 << std::log(2.0);
}

void ServerTests::testBisectionAllFunctions()
{
    QFETCH(int, id);
    QFETCH(double, a);
    QFETCH(double, b);
    QFETCH(double, root);
    bool success = false;
    double x = MathSolver::solveBisection(MathSolver::getFunction(id), a, b, 1e-6, success);
    QVERIFY(success);
    QVERIFY2(std::abs(x - root) <= 1e-6, qPrintable(QString("получено %1").arg(x, 0, 'f', 9)));
}

void ServerTests::testNewtonAllFunctions_data()
{
    QTest::addColumn<int>("id");
    QTest::addColumn<double>("x0");
    QTest::addColumn<double>("root");

    QTest::newRow("x^2-4, x0=2.5")        << 0 << 2.5 << 2.0;
    QTest::newRow("x^2-4, x0=-3")         << 0 << -3.0 << -2.0;
    QTest::newRow("sin(x)-x/2, x0=2")     << 1 << 2.0 << 1.895494267033981;
    QTest::newRow("e^x-2, x0=1")          << 2 << 1.0 << std::log(2.0);
}

void ServerTests::testNewtonAllFunctions()
{
    QFETCH(int, id);
    QFETCH(double, x0);
    QFETCH(double, root);
    bool success = false;
    double x = MathSolver::solveNewton(MathSolver::getFunction(id), MathSolver::getDerivative(id), x0, 1e-9, success);
    QVERIFY(success);
    QVERIFY2(std::abs(x - root) <= 1e-8, qPrintable(QString("получено %1").arg(x, 0, 'f', 12)));
}

void ServerTests::testBisectionPrecision_data()
{
    QTest::addColumn<double>("eps");
    QTest::newRow("eps=0.1")   << 0.1;
    QTest::newRow("eps=0.001") << 0.001;
    QTest::newRow("eps=1e-8")  << 1e-8;
}

void ServerTests::testBisectionPrecision()
{
    // Ошибка метода половинного деления не превышает eps
    QFETCH(double, eps);
    bool success = false;
    double x = MathSolver::solveBisection(MathSolver::getFunction(2), 0.0, 2.0, eps, success);
    QVERIFY(success);
    QVERIFY(std::abs(x - std::log(2.0)) <= eps);
}

void ServerTests::testBisectionRootOnBoundary()
{
    // Корень x=2 ровно на границе: f(a)*f(b) = 0, условие BR-4 (f(a)*f(b) < 0) не выполнено
    bool success = true;
    MathSolver::solveBisection(MathSolver::getFunction(0), 2.0, 5.0, 0.001, success);
    QVERIFY(!success);
}

void ServerTests::testNewtonMaxIterations()
{
    // Из далёкой точки за 2 итерации метод не успевает сойтись
    bool success = true;
    MathSolver::solveNewton(MathSolver::getFunction(0), MathSolver::getDerivative(0), 1000.0, 1e-9, success, 2);
    QVERIFY(!success);
}

// ============================================================
// MathSolver: известные дефекты
// ============================================================

void ServerTests::testBisectionReversedInterval()
{
    // По словарю данных спецификации b > a. При a > b метод не должен выдавать «корень» 5.
    bool success = true;
    double x = MathSolver::solveBisection(MathSolver::getFunction(0), 5.0, 0.0, 0.001, success);
    bool correct = !success || std::abs(x - 2.0) <= 0.001;
    QEXPECT_FAIL("", "Дефект №4: при a > b возвращается a = 5 вместо корня или FAILED", Continue);
    QVERIFY(correct);
}

void ServerTests::testBisectionNonPositiveEps()
{
    // ROB-1, BR-3: при eps <= 0 метод должен вернуть FAILED (success == false)
    bool success = true;
    MathSolver::solveBisection(MathSolver::getFunction(1), 1.0, 3.0, -0.001, success);
    QEXPECT_FAIL("", "Дефект №5: при eps <= 0 бисекция возвращает число вместо FAILED", Continue);
    QVERIFY(!success);
}

// ============================================================
// Database
// ============================================================

void ServerTests::testDatabaseRegisterAndAuth()
{
    Database* db = Database::getInstance();
    QVERIFY(db->registerUser("db_user", "pass"));
    QVERIFY(db->authoUser("db_user", "pass"));
    QVERIFY(!db->authoUser("db_user", "wrong"));
    QVERIFY(!db->authoUser("no_such_user", "pass"));
}

void ServerTests::testDatabaseDuplicateLogin()
{
    Database* db = Database::getInstance();
    QVERIFY(db->registerUser("dup_user", "1"));
    QVERIFY(!db->registerUser("dup_user", "2"));   // логин — первичный ключ
    QVERIFY(db->authoUser("dup_user", "1"));       // старый пароль не перезаписан
}

void ServerTests::testDatabaseSaveTaskResult()
{
    Database::getInstance()->saveTaskResult("history_user", 2, 0.0, 2.0, 0.001, 0.692383, 0.693148);
    QSqlQuery q;
    QVERIFY(q.exec("SELECT functionId, a, b, eps, bisectionResult, newtonResult FROM History WHERE login = 'history_user'"));
    QVERIFY(q.next());
    QCOMPARE(q.value(0).toInt(), 2);
    QCOMPARE(q.value(1).toDouble(), 0.0);
    QCOMPARE(q.value(2).toDouble(), 2.0);
    QCOMPARE(q.value(3).toDouble(), 0.001);
    QCOMPARE(q.value(4).toDouble(), 0.692383);
    QCOMPARE(q.value(5).toDouble(), 0.693148);
}

// ============================================================
// CommandParsing: протокол
// ============================================================

void ServerTests::testSolveRequiresAuth()
{
    // Этот тест идёт первым среди тестов протокола: в parser ещё никто не входил.
    // SEC-1, BR-2: без авторизации решать уравнения нельзя.
    QString resp = parser->Command("solve 0 0 5 0.001");
    QEXPECT_FAIL("", "Дефект №2: solve выполняется без авторизации", Continue);
    QCOMPARE(resp, QString("FAILED"));
}

void ServerTests::testProtocolRegister()
{
    QCOMPARE(parser->Command("reg proto_user 123"), QString("SUCCESS"));
    QCOMPARE(parser->Command("reg proto_user 123"), QString("ALREADY_EXISTS"));
}

void ServerTests::testProtocolAuth()
{
    QCOMPARE(parser->Command("auth proto_user 123"), QString("SUCCESS"));
    QCOMPARE(parser->Command("auth proto_user 999"), QString("FAILED"));
    QCOMPARE(parser->Command("auth nobody 123"), QString("FAILED"));
}

void ServerTests::testProtocolSolve()
{
    parser->Command("auth proto_user 123");
    QCOMPARE(parser->Command("solve 0 0 5 0.001"), QString("2.000122|2.000000"));
    QCOMPARE(parser->Command("solve 1 1 3 0.001"), QString("1.895508|1.895494"));
    QCOMPARE(parser->Command("solve 2 0 2 0.001"), QString("0.692383|0.693148"));
}

void ServerTests::testProtocolSolveInvalidInterval()
{
    // REQ-MATH-6: если метод не сошёлся, вместо числа — FAILED
    parser->Command("auth proto_user 123");
    QCOMPARE(parser->Command("solve 0 -5 5 0.001"), QString("FAILED|FAILED"));
    QCOMPARE(parser->Command("solve 2 -1 0 0.001").section('|', 0, 0), QString("FAILED"));
}

void ServerTests::testProtocolBadCommands()
{
    QCOMPARE(parser->Command("hello"), QString("UNKNOWN COMMAND"));
    QCOMPARE(parser->Command("auth"), QString("FAILED"));               // нет аргументов
    QCOMPARE(parser->Command("reg only_login"), QString("FAILED"));     // нет пароля
    QCOMPARE(parser->Command("solve 0 1 2"), QString("FAILED"));        // нет eps
}

void ServerTests::testProtocolSolveSavesHistory()
{
    // REQ-MATH-5: расчёт записывается в History под логином текущей сессии
    QCOMPARE(parser->Command("reg history_check 1"), QString("SUCCESS"));
    parser->Command("solve 2 0 2 0.001");
    QSqlQuery q;
    QVERIFY(q.exec("SELECT functionId, a, b FROM History WHERE login = 'history_check'"));
    QVERIFY(q.next());
    QCOMPARE(q.value(0).toInt(), 2);
    QCOMPARE(q.value(1).toDouble(), 0.0);
    QCOMPARE(q.value(2).toDouble(), 2.0);
}

void ServerTests::testProtocolUnknownFunctionId()
{
    // Словарь данных: functionId принимает значения 0, 1, 2
    parser->Command("auth proto_user 123");
    QString resp = parser->Command("solve 7 -1 1 0.001");
    QEXPECT_FAIL("", "Дефект №8: для неизвестного funcId считается f(x)=x, ответ 0.000000|0.000000", Continue);
    QCOMPARE(resp, QString("FAILED|FAILED"));
}

void ServerTests::testProtocolEmptyLogin()
{
    // Словарь данных: login — не пустой
    QString resp = parser->Command("reg  secret");   // два пробела: логин пустой
    QEXPECT_FAIL("", "Дефект №6: пустой логин регистрируется", Continue);
    QCOMPARE(resp, QString("FAILED"));
}

void ServerTests::testProtocolPasswordWithSpace()
{
    QCOMPARE(parser->Command("reg space_user my pass"), QString("SUCCESS"));
    // Вход по части пароля должен быть невозможен
    QString resp = parser->Command("auth space_user my");
    QEXPECT_FAIL("", "Дефект №7: пароль обрезается до первого пробела, вход по части пароля проходит", Continue);
    QCOMPARE(resp, QString("FAILED"));
}

// ============================================================
// Шаг метода Ньютона
// ============================================================

void ServerTests::testNewtonStepFormula_data()
{
    QTest::addColumn<int>("funcId");
    QTest::addColumn<double>("x0");
    QTest::addColumn<double>("expected");

    // x1 = x0 - f(x0)/f'(x0); значения посчитаны вручную
    QTest::newRow("x^2-4, x0=3")   << 0 << 3.0 << 2.1666667;   // 3 - 5/6
    QTest::newRow("x^2-4, x0=2")   << 0 << 2.0 << 2.0;         // корень остаётся на месте
    QTest::newRow("sin-x/2, x0=2") << 1 << 2.0 << 1.9009958;   // 2 - (sin2-1)/(cos2-0.5)
    QTest::newRow("e^x-2, x0=1")   << 2 << 1.0 << 0.7357589;   // 1 - (e-2)/e
}

void ServerTests::testNewtonStepFormula()
{
    QFETCH(int, funcId);
    QFETCH(double, x0);
    QFETCH(double, expected);

    bool ok = false;
    double x1 = MathSolver::newtonStep(MathSolver::getFunction(funcId), x0,
                                       MathSolver::getDerivative(funcId)(x0), ok);
    QVERIFY(ok);
    QVERIFY(qAbs(x1 - expected) < 1e-6);
}

void ServerTests::testNewtonStepZeroDerivative()
{
    bool ok = true;
    double x1 = MathSolver::newtonStep(MathSolver::getFunction(0), 0.0, 0.0, ok);
    QVERIFY(!ok);
    QCOMPARE(x1, 0.0);
}

void ServerTests::testProtocolStepWithDerivative()
{
    QCOMPARE(parser->Command("step 0 3 6"), QString("2.166667"));
    // f'(x0) можно передать любое: формула использует то, что дал пользователь
    QCOMPARE(parser->Command("step 0 3 5"), QString("2.000000"));
    QCOMPARE(parser->Command("step 0 3 0"), QString("FAILED"));
}

void ServerTests::testProtocolStepServerDerivative()
{
    // Без третьего аргумента сервер считает f'(x0) сам
    QCOMPARE(parser->Command("step 0 3"), QString("2.166667"));
    QCOMPARE(parser->Command("step 1 2"), QString("1.900996"));
    QCOMPARE(parser->Command("step 2 1"), QString("0.735759"));
    // f'(0) = 0 для x^2 - 4
    QCOMPARE(parser->Command("step 0 0"), QString("FAILED"));
}

void ServerTests::testProtocolStepBadInput()
{
    QCOMPARE(parser->Command("step"), QString("FAILED"));
    QCOMPARE(parser->Command("step 0"), QString("FAILED"));
    QCOMPARE(parser->Command("step 9 3"), QString("FAILED"));       // неизвестное уравнение
    QCOMPARE(parser->Command("step 0 abc"), QString("FAILED"));     // не число
    QCOMPARE(parser->Command("step 0 3 xyz"), QString("FAILED"));
    QCOMPARE(parser->Command("step 0 3 6 7"), QString("FAILED"));   // лишний аргумент
}

QTEST_GUILESS_MAIN(ServerTests)
#include "main.moc"
