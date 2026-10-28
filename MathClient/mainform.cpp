#include "mainform.h"
#include "ui_mainform.h"
#include "clientcontroller.h"
#include "mytcpclient.h"

MainForm::MainForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);
    
    // Populate functions
    ui->functionCombo->addItem("f(x) = x^2 - 4", 0);
    ui->functionCombo->addItem("f(x) = sin(x) - x/2", 1);
    ui->functionCombo->addItem("f(x) = e^x - 2", 2);
}

MainForm::~MainForm()
{
    delete ui;
}

void MainForm::on_solveButton_clicked()
{
    int funcId = ui->functionCombo->currentData().toInt();
    double a = ui->aEdit->text().toDouble();
    double b = ui->bEdit->text().toDouble();
    double eps = ui->epsEdit->text().toDouble();
    
    // Send command to server 
    QString cmd = ClientController::getInstance()->makeSolveTaskCommand(funcId, a, b, eps);
    MyTcpClient::getInstance()->sendMessage(cmd);
    
    // Read the response from the server which contains the results
    QString response = MyTcpClient::getInstance()->getMsg();
    
    QStringList parts = response.split("|");
    if (parts.size() == 2) {
        ui->bisectResult->setText(parts[0]);
        ui->newtonResult->setText(parts[1]);
    } else {
        ui->bisectResult->setText("Error communicating with server");
        ui->newtonResult->setText("Error communicating with server");
    }
}

void MainForm::on_stepButton_clicked()
{
    int funcId = ui->functionCombo->currentData().toInt();

    bool okX = false;
    double x0 = ui->x0Edit->text().toDouble(&okX);
    if (!okX) {
        ui->stepResult->setText("Введите число в поле x0");
        return;
    }

    QString cmd;
    QString dText = ui->dfx0Edit->text().trimmed();
    if (dText.isEmpty()) {
        cmd = ClientController::getInstance()->makeNewtonStepCommand(funcId, x0);
    } else {
        bool okD = false;
        double dfx0 = dText.toDouble(&okD);
        if (!okD) {
            ui->stepResult->setText("Введите число в поле f'(x0)");
            return;
        }
        cmd = ClientController::getInstance()->makeNewtonStepCommand(funcId, x0, dfx0);
    }

    MyTcpClient::getInstance()->sendMessage(cmd);
    QString response = MyTcpClient::getInstance()->getMsg();

    if (response == "FAILED")
        ui->stepResult->setText("Ошибка: f'(x0) = 0 или неверные данные");
    else
        ui->stepResult->setText("x1 = " + response);
}

void MainForm::on_logoutButton_clicked()
{
    emit logout();
}
