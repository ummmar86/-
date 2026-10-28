#include "parsing.h"
#include <QStringList>
#include <QDebug>

CommandParsing::CommandParsing(QObject *parent) : QObject(parent)
{
    my_database = Database::getInstance();
    my_database->openDatabase("mathserver.db");
    my_database->createTables();
    currentLogin = "anonymous";
}

QString CommandParsing::Command(const QString &dataStr)
{
    QStringList parts = dataStr.split(" ");
    if (parts.isEmpty()) return "FAILED";
    
    QString command = parts[0];
    QString res = "";
    
    if (command == "auth") {
        if (parts.size() < 3) return "FAILED";
        QString login = parts[1];
        QString pass = parts[2];
        if (my_database->authoUser(login, pass)) {
            currentLogin = login; // Save for history logging
            res = "SUCCESS";
        } else {
            res = "FAILED";
        }
    } 
    else if (command == "reg") {
        if (parts.size() < 3) return "FAILED";
        QString login = parts[1];
        QString pass = parts[2];
        if (my_database->registerUser(login, pass)) {
            currentLogin = login; // Automatically log the user in on the server
            res = "SUCCESS";
        } else {
            res = "ALREADY_EXISTS";
        }
    }
    else if (command == "solve") {
        // solve <functionId> <a> <b> <eps>
        if (parts.size() < 5) return "FAILED";
        
        int funcId = parts[1].toInt();
        double a = parts[2].toDouble();
        double b = parts[3].toDouble();
        double eps = parts[4].toDouble();
        
        auto f = MathSolver::getFunction(funcId);
        auto df = MathSolver::getDerivative(funcId);
        
        bool bisectSuccess = false;
        double bisectRes = MathSolver::solveBisection(f, a, b, eps, bisectSuccess);
        
        bool newtonSuccess = false;
        double newtonRes = MathSolver::solveNewton(f, df, (a+b)/2.0, eps, newtonSuccess);
        
        // Save to history
        my_database->saveTaskResult(currentLogin, funcId, a, b, eps, bisectRes, newtonRes);
        
        // Return format: bisectionResult|newtonResult
        // Using large negative numbers as an error flag for simplicity
        QString brStr = bisectSuccess ? QString::number(bisectRes, 'f', 6) : "FAILED";
        QString nrStr = newtonSuccess ? QString::number(newtonRes, 'f', 6) : "FAILED";
        
        res = brStr + "|" + nrStr;
    }
    else if (command == "step") {
        // step <functionId> <x0> [<f'(x0)>]  ->  x1 = x0 - f(x0)/f'(x0)
        if (parts.size() < 3 || parts.size() > 4) return "FAILED";

        bool okId = false, okX = false, okD = true;
        int funcId = parts[1].toInt(&okId);
        double x0 = parts[2].toDouble(&okX);
        if (!okId || !okX || funcId < 0 || funcId > 2) return "FAILED";

        auto f = MathSolver::getFunction(funcId);
        // Если производная не передана, сервер считает её сам.
        double dfx0 = (parts.size() == 4) ? parts[3].toDouble(&okD)
                                          : MathSolver::getDerivative(funcId)(x0);
        if (!okD) return "FAILED";

        bool success = false;
        double x1 = MathSolver::newtonStep(f, x0, dfx0, success);
        res = success ? QString::number(x1, 'f', 6) : "FAILED";
    }
    else {
        res = "UNKNOWN COMMAND";
    }
    
    // Add the \x01 character so the client's getMsg() stops reading, 
    // exactly how it is expected by `MyTcpClient::getMsg()` inside `MathClient`.
    // Wait, the client's `getMsg()` looks for `\x01` byte to clear the buffer?
    // Let me check `mytcpclient.cpp` of the client.
    // Client has:
    // if(array == "\x01") { res = ""; } else { res.append(array); }
    // It doesn't break, it just clears. Oh, `mTcpSocket->waitForReadyRead();` in client reads data, and if the data is just the response, it's fine.
    
    return res;
}
