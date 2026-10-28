/**
 * @file mainform.h
 * @brief Главное окно клиента: выбор уравнения, ввод параметров, результаты.
 */
#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>

namespace Ui {
class MainForm;
}

/**
 * @brief Окно вычислений (UI-2).
 *
 * Содержит список уравнений functionCombo, поля aEdit, bEdit, epsEdit,
 * кнопку Solve и поля только для чтения bisectResult и newtonResult.
 */
class MainForm : public QWidget
{
    Q_OBJECT

public:
    /// Создаёт окно и заполняет список уравнений.
    explicit MainForm(QWidget *parent = nullptr);
    ~MainForm();

signals:
    /// Пользователь нажал Logout.
    void logout();

private slots:
    /**
     * @brief Отправляет "solve <funcId> <a> <b> <eps>" и показывает ответ "x1|x2" (REQ-MATH-2…7).
     * @bug Дефект №9: нечисловой ввод (например, "abc") без предупреждения
     *      превращается в 0, и считается не та задача, которую ввёл пользователь.
     */
    void on_solveButton_clicked();
    /**
     * @brief Один шаг метода Ньютона: x1 = x0 - f(x0)/f'(x0).
     *
     * Берёт уравнение из functionCombo, x0 из x0Edit и (необязательно) f'(x0) из dfx0Edit,
     * отправляет "step <funcId> <x0> [<f'(x0)>]" и показывает x1 в stepResult.
     * Если поле f'(x0) пустое, производную считает сервер.
     * Некорректный ввод не отправляется на сервер.
     */
    void on_stepButton_clicked();
    /// Испускает сигнал logout().
    void on_logoutButton_clicked();

private:
    Ui::MainForm *ui; ///< Элементы формы из mainform.ui.
};

#endif // MAINFORM_H
