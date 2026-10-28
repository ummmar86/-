/**
 * @file mathsolver.h
 * @brief Численные методы поиска корней нелинейных уравнений (сервер).
 */
#ifndef MATHSOLVER_H
#define MATHSOLVER_H

#include <QString>
#include <functional>

/**
 * @brief Математическое ядро сервера: метод половинного деления и метод Ньютона.
 *
 * Все методы статические, объект класса создавать не нужно.
 * Поддерживаемые уравнения (REQ-MATH-1):
 * | funcId | Уравнение       | Производная     |
 * |--------|-----------------|-----------------|
 * | 0      | x^2 - 4         | 2x              |
 * | 1      | sin(x) - x/2    | cos(x) - 1/2    |
 * | 2      | e^x - 2         | e^x             |
 */
class MathSolver
{
public:
    /**
     * @brief Находит корень методом половинного деления (дихотомии).
     *
     * Отрезок [a, b] делится пополам, пока его длина не станет меньше @p eps.
     * На концах отрезка функция должна иметь разные знаки (BR-4).
     *
     * @param f       Функция, корень которой ищется.
     * @param a       Левая граница отрезка.
     * @param b       Правая граница отрезка.
     * @param eps     Требуемая точность (длина итогового отрезка).
     * @param[out] success true — корень найден; false — f(a)·f(b) >= 0.
     * @return Приближённое значение корня (0, если @p success == false).
     *
     * @bug Дефект №4: при a > b цикл не выполняется ни разу и возвращается a
     *      как «корень» (например, для x^2-4 на [5, 0] возвращается 5).
     * @bug Дефект №5: при eps <= 0 возвращается число вместо статуса FAILED
     *      (требование ROB-1, BR-3).
     */
    static double solveBisection(std::function<double(double)> f, double a, double b, double eps, bool& success);

    /**
     * @brief Находит корень методом Ньютона (касательных): x_{n+1} = x_n - f(x_n)/f'(x_n).
     *
     * @param f        Функция.
     * @param df       Производная функции.
     * @param x0       Начальное приближение (сервер передаёт (a+b)/2, BR-5).
     * @param eps      Точность: итерации прекращаются, когда |x_{n+1} - x_n| <= eps.
     * @param[out] success true — метод сошёлся; false — производная близка к нулю
     *                 (|f'(x)| < 1e-12) или превышено число итераций.
     * @param maxIter  Максимальное число итераций (по умолчанию 1000).
     * @return Приближённое значение корня.
     */
    static double solveNewton(std::function<double(double)> f, std::function<double(double)> df, double x0, double eps, bool& success, int maxIter = 1000);

    /**
     * @brief Один шаг метода Ньютона: x1 = x0 - f(x0) / f'(x0).
     *
     * Дано: функция, начальное приближение x0 и значение производной f'(x0).
     * Найти: следующее приближение x1.
     *
     * @param f        Функция.
     * @param x0       Начальное приближение.
     * @param dfx0     Значение производной в точке x0.
     * @param[out] success false, если |f'(x0)| < 1e-12 (делить на ноль нельзя).
     * @return Следующее приближение x1 (0, если @p success == false).
     */
    static double newtonStep(std::function<double(double)> f, double x0, double dfx0, bool& success);

    /**
     * @brief Возвращает уравнение по его идентификатору.
     * @param id Идентификатор уравнения (0, 1 или 2).
     * @return Функция f(x).
     * @bug Дефект №8: для неизвестного id молча возвращается f(x) = x,
     *      сервер отвечает "0.000000|0.000000" вместо FAILED.
     */
    static std::function<double(double)> getFunction(int id);

    /**
     * @brief Возвращает производную уравнения по его идентификатору.
     * @param id Идентификатор уравнения (0, 1 или 2).
     * @return Производная f'(x); для неизвестного id — константа 1.
     */
    static std::function<double(double)> getDerivative(int id);

    /**
     * @brief Возвращает текстовую запись уравнения.
     * @param id Идентификатор уравнения.
     * @return Строка вида "f(x) = x^2 - 4" или "Unknown".
     */
    static QString getFunctionName(int id);
};

#endif // MATHSOLVER_H
