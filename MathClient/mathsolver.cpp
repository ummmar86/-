#include "mathsolver.h"
#include <cmath>
#include <stdexcept>

double MathSolver::solveBisection(std::function<double(double)> f, double a, double b, double eps, bool& success) {
    if (f(a) * f(b) >= 0) {
        success = false;
        return 0;
    }
    
    double c = a;
    while ((b - a) >= eps) {
        c = (a + b) / 2;
        if (f(c) == 0.0) {
            break;
        } else if (f(c) * f(a) < 0) {
            b = c;
        } else {
            a = c;
        }
    }
    success = true;
    return c;
}

double MathSolver::solveNewton(std::function<double(double)> f, std::function<double(double)> df, double x0, double eps, bool& success, int maxIter) {
    double x = x0;
    for (int i = 0; i < maxIter; ++i) {
        double y = f(x);
        double yprime = df(x);
        
        if (std::abs(yprime) < 1e-12) {
            success = false;
            return 0; // Derivative is too small
        }
        
        double nextX = x - y / yprime;
        if (std::abs(nextX - x) <= eps) {
            success = true;
            return nextX;
        }
        x = nextX;
    }
    success = false;
    return x;
}

std::function<double(double)> MathSolver::getFunction(int id) {
    switch(id) {
        case 0: return [](double x) { return x * x - 4.0; };
        case 1: return [](double x) { return std::sin(x) - x / 2.0; };
        case 2: return [](double x) { return std::exp(x) - 2.0; };
        default: return [](double x) { return x; };
    }
}

std::function<double(double)> MathSolver::getDerivative(int id) {
    switch(id) {
        case 0: return [](double x) { return 2.0 * x; };
        case 1: return [](double x) { return std::cos(x) - 0.5; };
        case 2: return [](double x) { return std::exp(x); };
        default: return [](double) { return 1.0; };
    }
}

QString MathSolver::getFunctionName(int id) {
    switch(id) {
        case 0: return "f(x) = x^2 - 4";
        case 1: return "f(x) = sin(x) - x/2";
        case 2: return "f(x) = e^x - 2";
        default: return "Unknown";
    }
}
