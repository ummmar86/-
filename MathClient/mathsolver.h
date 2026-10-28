#ifndef MATHSOLVER_H
#define MATHSOLVER_H

#include <QString>
#include <functional>

class MathSolver
{
public:
    static double solveBisection(std::function<double(double)> f, double a, double b, double eps, bool& success);
    static double solveNewton(std::function<double(double)> f, std::function<double(double)> df, double x0, double eps, bool& success, int maxIter = 1000);
    
    // Helper to evaluate standard functions
    static std::function<double(double)> getFunction(int id);
    static std::function<double(double)> getDerivative(int id);
    static QString getFunctionName(int id);
};

#endif // MATHSOLVER_H
