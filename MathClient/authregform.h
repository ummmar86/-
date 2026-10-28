/**
 * @file authregform.h
 * @brief Окно входа и регистрации (стартовое окно клиента).
 */
#ifndef AUTHREGFORM_H
#define AUTHREGFORM_H

#include <QWidget>
#include "mainform.h"

namespace Ui {
class AuthRegForm;
}

/**
 * @brief Стартовое окно: поля логина и пароля, кнопки Login и Register (UI-1).
 *
 * После успешного входа или регистрации окно скрывается и открывается MainForm.
 * При нажатии Logout в MainForm окно показывается снова, поле пароля очищается.
 */
class AuthRegForm : public QWidget
{
    Q_OBJECT

public:
    /// Создаёт форму и главное окно MainForm (пока скрытое).
    explicit AuthRegForm(QWidget *parent = nullptr);
    ~AuthRegForm();

private slots:
    /// Отправляет "auth <login> <password>"; при SUCCESS открывает MainForm (REQ-AUTH-1, 2).
    void on_loginButton_clicked();
    /// Отправляет "reg <login> <password>"; обрабатывает SUCCESS и ALREADY_EXISTS (REQ-REG-1…5).
    void on_registerButton_clicked();
    /// Вызывается по сигналу MainForm::logout(): скрывает MainForm и показывает окно входа (UI-3).
    void onMainFormClosed();

private:
    Ui::AuthRegForm *ui; ///< Элементы формы из authregform.ui.
    MainForm *mainForm;  ///< Главное окно вычислений.
};

#endif // AUTHREGFORM_H
