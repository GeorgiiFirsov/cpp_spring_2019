#include "numbers.dat"
#include <iostream>
#include <vector>
#include <math.h>

/*
 *  Используем решето Эратосфена
 * 
 *  Алгоритм работает быстрее в сравнении с делением числа n на все числа от 3 до sqrt(n)
 * 
 *  Асимптотика алгоритма: O( n*log(log(n)) )
 *  Что лучше, чем O( sqrt(n) ) для перебора делителей
 * 
 */
void make_Esieve(std::vector<bool>& sieve, int limit)
{
    sieve.resize(limit, true);
    sieve[0] = sieve[1] = false;
    int_fast32_t bound = static_cast<int_fast32_t>(sqrt(limit)) + 1;
    for(int_fast32_t i = 2; i < bound; i++)
        if(sieve[i])
            for(int_fast32_t j = 2; i * j < limit; j++) sieve[i * j] = false;
}

int main(int argc, char* argv[])
{
    // проверка количества аргументов
    if (argc % 2 != 1 || argc == 1) return -1;
    
    // заполняем решето Эратосфена
    std::vector<bool> Esieve;
    make_Esieve(Esieve, Size);
    
    // необходимые переменные (счётчик, состояния, границы)
    int counter = 0;
    int lbound = -1, rbound = -1;
    bool state = false, finished = false;

    // для каждой пары аргументов выводим значение
    for(int_fast16_t i = 1; i < argc - 1; i += 2)
    {
        lbound = atoi(argv[i]);
        rbound = atoi(argv[i + 1]);
        counter = 0;
        state = false;
        finished = false;
        for(auto& item : Data)
        {
            if(!state && item == lbound) state = true;
            if( state && Esieve[item]) counter++;
            if( state && item == rbound) 
            {
                finished = true;
                break;
            }
        }
        std::cout << (finished ? counter : 0) << std::endl;
    }
    
    return 0;
}
