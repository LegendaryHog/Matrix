#include <gtest/gtest.h>
#include "matrix.hpp"
#include <list>
#include <vector>
#include <set>

#define PRINT

using namespace matrix;

TEST(Constructors, by_1_val)
{
    matrix_t<int> mat1 (1, 1, 1);
    matrix_t<int> mat2 (3, 3, 2);
    matrix_t<int> mat3 (3, 3);
    matrix_t<int> mat4 (1, 1);
    matrix_t<int> mat5;

    #ifdef PRINT
    std::cout << "Ctor by 1 val:" << std::endl;
    std::cout << "(1, 1, 1):" << mat1 << std::endl;
    std::cout << "(3, 3, 3):" << mat2 << std::endl;
    std::cout << "(3, 3):   " << mat3 << std::endl;
    std::cout << "(1, 1):   " << mat4 << std::endl;
    std::cout << "():       " << mat5 << std::endl << std::endl;
    #endif
}

TEST(Constructors, by_conteiner)
{
    std::vector<int> vec = {1, 2, 3, 4, 5, 6};
    std::list<int> list = {7, 8, 9, 10, 11, 12};

    matrix_t<int> mat1 (2, 3, vec.begin(), vec.end());
    matrix_t<int> mat2 (2, 2, vec.begin(), vec.end());
    matrix_t<int> mat3 (3, 2, vec.begin(), vec.end());
    matrix_t<int> mat4 (2, 3, list.begin(), list.end());
    matrix_t<int> mat5 (2, 2, list.begin(), list.end());
    matrix_t<int> mat6 (4, 4, list.begin(), list.end());

    #ifdef PRINT
    std::cout << "Ctor by cont:" << std::endl;
    std::cout << "(2, 3, vec):" << mat1 << std::endl;
    std::cout << "(2, 2, vec):" << mat2 << std::endl;
    std::cout << "(3, 2, vec):" << mat3 << std::endl;
    std::cout << "(2, 3, lst):" << mat4 << std::endl;
    std::cout << "(2, 2, lst):" << mat5 << std::endl;
    std::cout << "(4, 4, lst):" << mat6 << std::endl << std::endl;
    #endif
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}