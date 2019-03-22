/*
 *  Описание тестов.
 * 
 *  Модуль "Ordinary tests":
 *      #1 - Проверка правильного определения размера
 *      #2 - Проверка работоспособности оператора [][] на чтение и запись
 *      #3 - Проверка оператора ==
 *      #4 - Проверка оператора *=
 *      #5 - Проверка оператора != на разницу в первом индексе
 *      #6 - Проверка оператора != на разницу во втором индексе
 *      #7 - Проверка оператора != на разницу в элементах
 *      #8 - Проверка выбрасывания исключения на большой первый индекс
 *      #9 - Проверка выбрасывания исключения на большой второй индекс
 * 
 *  Модуль "Const tests":
 *      #1 - Проверка правильного определения размера
 *      #2 - Проверка работоспособности оператора [][] на чтение
 *      #3 - Проверка оператора ==
 *      #4 - Проверка оператора != на разницу в первом индексе
 *      #5 - Проверка оператора != на разницу во втором индексе
 *      #6 - Проверка оператора != на разницу в элементах
 *      #7 - Проверка выбрасывания исключения на большой первый индекс
 *      #8 - Проверка выбрасывания исключения на большой второй индекс
 * 
 */
#include "matrix.h"
#include <iostream>

using std::cout;
using std::out_of_range;

void show_res(bool& res, unsigned int num)
{
    if(!res) cout << "    Failed test #" << num << std::endl;
    else cout << "    Test #" << num << " passed\n";
    res = true;
}

void RunAllTests()
{
    bool current = true;
    int fail = 0, success = 0;
    cout << "Running tests...\n";
    cout << "Ordinary tests:\n";

    Matrix m(7,9);
    current = (m.getColumns() == 9 and m.getRows() == 7);
    current ? success++ : fail++;
    show_res(current, 1);

    try
    {
        for(std::size_t i = 0; i < m.getRows(); i++)
            for(std::size_t j = 0; j < m.getColumns(); j++) m[i][j] = 1 + i + j;
    }
    catch(const std::out_of_range& e)
    {
        current = false;
    }
    (m[3][4] != 8) ? current = false : current = current;
    current ? success++ : fail++;
    show_res(current, 2);

    Matrix toCmp1(7,9), toCmp2(7,9), toCmp3(8,9), toCmp4(7,8);
    for(std::size_t i = 0; i < toCmp2.getRows(); i++)
            for(std::size_t j = 0; j < toCmp2.getColumns(); j++) toCmp2[i][j] = 3 * (1 + i + j);
    for(std::size_t i = 0; i < toCmp1.getRows(); i++)
            for(std::size_t j = 0; j < toCmp1.getColumns(); j++) toCmp1[i][j] = 1 + i + j;
    current = (m == toCmp1);
    current ? success++ : fail++;
    show_res(current, 3);

    m *= 3;
    current = (m == toCmp2);
    current ? success++ : fail++;
    show_res(current, 4);

    current = (m != toCmp3);
    current ? success++ : fail++;
    show_res(current, 5);

    current = (m != toCmp4);
    current ? success++ : fail++;
    show_res(current, 6);

    current = (m != toCmp1);
    current ? success++ : fail++;
    show_res(current, 7);

    current = false;
    try
    {
        m[10][0] = 1;
    }
    catch(const std::out_of_range& e)
    {
        current = true;
    }
    current ? success++ : fail++;
    show_res(current, 8);

    current = false;
    try
    {
        m[0][10] = 1;
    }
    catch(const std::out_of_range& e)
    {
        current = true;
    }
    current ? success++ : fail++;
    show_res(current, 9);

    cout << "Ordinary tests finished with " << fail << " fail(s) of " << fail + success;
    fail = success = 0;
    cout << "\nConst tests:\n";

    const Matrix mc(m);
    current = (mc.getColumns() == 9 and mc.getRows() == 7);
    current ? success++ : fail++;
    show_res(current, 1);

    try
    {
        for(std::size_t i = 0; i < mc.getRows(); i++)
            for(std::size_t j = 0; j < mc.getColumns(); j++) mc[i][j];
    }
    catch(const std::out_of_range& e)
    {
        current = false;
    }
    (mc[3][4] != 24) ? current = false : current = current;
    current ? success++ : fail++;
    show_res(current, 2);

    current = (mc == toCmp2);
    current ? success++ : fail++;
    show_res(current, 3);

    current = (mc != toCmp3);
    current ? success++ : fail++;
    show_res(current, 4);

    current = (mc != toCmp4);
    current ? success++ : fail++;
    show_res(current, 5);

    current = (mc != toCmp1);
    current ? success++ : fail++;
    show_res(current, 6);

    current = false;
    try
    {
        mc[10][0];
    }
    catch(const std::out_of_range& e)
    {
        current = true;
    }
    current ? success++ : fail++;
    show_res(current, 7);

    current = false;
    try
    {
        mc[0][10];
    }
    catch(const std::out_of_range& e)
    {
        current = true;
    }
    current ? success++ : fail++;
    show_res(current, 8);
    cout << "Const tests finished with " << fail << " fail(s) of " << fail + success << std::endl;
}