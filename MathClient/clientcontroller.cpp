#include "clientcontroller.h"

ClientController* ClientController::instance = nullptr;
ClientControllerDestroyer ClientController::destroyer;

ClientControllerDestroyer::~ClientControllerDestroyer() { delete instance; }

QString ClientController::makeLoginCommand(QString login, QString password) {
    return QString("auth %1 %2").arg(login).arg(password);
}

QString ClientController::makeRegisterCommand(QString login, QString password) {
    return QString("reg %1 %2").arg(login).arg(password);
}

QString ClientController::makeSolveTaskCommand(int functionId, double a, double b, double eps) {
    return QString("solve %1 %2 %3 %4").arg(functionId).arg(a).arg(b).arg(eps);
}

QString ClientController::makeNewtonStepCommand(int functionId, double x0, double dfx0) {
    return QString("step %1 %2 %3").arg(functionId).arg(x0).arg(dfx0);
}

QString ClientController::makeNewtonStepCommand(int functionId, double x0) {
    return QString("step %1 %2").arg(functionId).arg(x0);
}

QString ClientController::parseServerResponse(QString response) {
    if (response.contains("успешно") || response.contains("success")) {
        return "SUCCESS";
    }
    if (response.contains("не удалась") || response.contains("failed")) {
        return "FAILED";
    }
    if (response == "ALREADY_EXISTS") {
        return "ALREADY_EXISTS";
    }
    return response;
}
