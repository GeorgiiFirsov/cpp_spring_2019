#include "matrix.h"
#include "testing.h"
#include <iostream>

using std::cout;
using std::out_of_range;

int main() 
{
    RunAllTests();
    Matrix matr(6,7);
    for(std::size_t i = 0; i < matr.getRows(); i++)
        for(std::size_t j = 0; j < matr.getColumns(); j++) matr[i][j] = rand() % 10;
    const Matrix cmatr(matr);
    matr *= 2;
    cout << "\nMatrix:\n" << matr;
    cout << "\nConst matrix:\n" << cmatr;
    return 0;
}