#include "authregform.h"
#include "ui_authregform.h"
#include "clientcontroller.h"
#include "mytcpclient.h"
#include <QMessageBox>

AuthRegForm::AuthRegForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AuthRegForm)
{
    ui->setupUi(this);
    mainForm = new MainForm();
    connect(mainForm, &MainForm::logout, this, &AuthRegForm::onMainFormClosed);
}

AuthRegForm::~AuthRegForm()
{
    delete ui;
    delete mainForm;
}

void AuthRegForm::on_loginButton_clicked()
{
    QString login = ui->loginEdit->text();
    QString password = ui->passwordEdit->text();

    QString cmd = ClientController::getInstance()->makeLoginCommand(login, password);
    MyTcpClient::getInstance()->sendMessage(cmd);
    
    QString response = MyTcpClient::getInstance()->getMsg();
    QString parsed = ClientController::getInstance()->parseServerResponse(response);
    
    if (parsed == "SUCCESS") {
        this->hide();
        mainForm->show();
    } else {
        QMessageBox::warning(this, "Error", "Login failed");
    }
}

void AuthRegForm::on_registerButton_clicked()
{
    QString login = ui->loginEdit->text();
    QString password = ui->passwordEdit->text();

    QString cmd = ClientController::getInstance()->makeRegisterCommand(login, password);
    MyTcpClient::getInstance()->sendMessage(cmd);
    
    QString response = MyTcpClient::getInstance()->getMsg();
    QString parsed = ClientController::getInstance()->parseServerResponse(response);
    
    if (parsed == "SUCCESS") {
        QMessageBox::information(this, "Успех", "Регистрация прошла успешно!");
        this->hide();
        mainForm->show();
    } else if (parsed == "ALREADY_EXISTS") {
        QMessageBox::warning(this, "Ошибка", "Такой пользователь уже существует!");
    } else {
        QMessageBox::warning(this, "Ошибка", "Ошибка регистрации!");
    }
}

void AuthRegForm::onMainFormClosed()
{
    mainForm->hide();
    this->show();
    ui->passwordEdit->clear();
}
