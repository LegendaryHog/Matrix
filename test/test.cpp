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
    std::cout << "(1, 1, 1): " << mat1 << std::endl;
    std::cout << "(3, 3, 3): " << mat2 << std::endl;
    std::cout << "(3, 3):    " << mat3 << std::endl;
    std::cout << "(1, 1):    " << mat4 << std::endl;
    std::cout << "():        " << mat5 << std::endl << std::endl;
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
    std::cout << "(2, 3, vec): " << mat1 << std::endl;
    std::cout << "(2, 2, vec): " << mat2 << std::endl;
    std::cout << "(3, 2, vec): " << mat3 << std::endl;
    std::cout << "(2, 3, lst): " << mat4 << std::endl;
    std::cout << "(2, 2, lst): " << mat5 << std::endl;
    std::cout << "(4, 4, lst): " << mat6 << std::endl << std::endl;
    #endif
}

TEST(Constructors, _1x1_by_val_and_one_dim_list)
{
    matrix_t<int> mat1 {2};
    matrix_t<int> mat2 (3);
    matrix_t<int> mat3 = 4;
    matrix_t<int> mat4 {2, 3, 2};
    matrix_t<int> mat5 = {2, 3, 2};

    #ifdef PRINT
    std::cout << "Ctor 1x1 by val and by onedim list:" << std::endl;
    std::cout << "{2}: " << mat1 << std::endl;
    std::cout << "(3): " << mat2 << std::endl;
    std::cout << "= 4: " << mat3 << std::endl;
    std::cout << "{2, 3, 2}: " << mat4 << std::endl;
    std::cout << "= {2, 3, 2}: " << mat5 << std::endl << std::endl;
    #endif
}

TEST(Constructors, by_two_dim_list)
{
    matrix_t<int> mat1 {{1, 0}, {0, 1}};
    matrix_t<int> mat2 {{1}, {0, 1}};
    matrix_t<int> mat3 {{1, 2}, {5, 6, 7, 8}, {}};
    matrix_t<int> mat4 {{1, 1, 2}, {}};
    matrix_t<int> mat5 {{1, 1, 2}};

    #ifdef PRINT
    std::cout << "Ctor by twodim list:" << std::endl;
    std::cout << "{{1, 0}, {0, 1}}:          " << mat1 << std::endl;
    std::cout << "{{1}, {0, 1}}:             " << mat2 << std::endl;
    std::cout << "{{1, 2}, {5, 6, 7, 8}, {}}:" << mat3 << std::endl;
    std::cout << "{{1, 1, 2},{}}:            " << mat4 << std::endl;
    std::cout << "{{1, 1, 2}}:               " << mat5 << std::endl << std::endl;
    #endif
}

TEST(Constructors, copy_ctor_and_copy_assignment)
{
    matrix_t<int> mat1 {{1}, {0, 1}, {0, 0, 1}};
    matrix_t<int> mat2 {mat1};
    matrix_t<int> mat3 = mat1;
    matrix_t<int> mat4 = mat1;
    mat4 = mat2;

    #ifdef PRINT
    std::cout << "Test copy:" << std::endl;
    std::cout << "mat1:        " << mat1 << std::endl;
    std::cout << "mat2 {mat1}: " << mat2 << std::endl;
    std::cout << "mat3 = mat1: " << mat3 << std::endl;
    std::cout << "mat4 = mat2: " << mat4 << std::endl << std::endl;
    #endif
}

matrix_t<int> ret_tmp()
{
    return matrix_t<int>(2, 2, 1);
}

TEST(Constructors, move_ctor_and_move_assignment)
{
    matrix_t<int> mat1 {ret_tmp()};
    matrix_t<int> mat2 (3, 3, 1);
    matrix_t<int> mat3 (2, 1, 3);
    mat3 = std::move(mat2);

    #ifdef PRINT
    std::cout << "Test move:" << std::endl;
    std::cout << "mat1 ctored by tmp (2, 2, 1):     " << mat1 << std::endl;
    std::cout << "mat2 (3, 3, 1) mat3 = move(mat2): " << mat3 << std::endl << std::endl;
    #endif
}

TEST(Constructors, diag_ctors)
{
    std::vector<int> vec {1, 12, 34, -5};
    std::list<int>   lst {37, 65, 91};

    matrix_t<int> mat1 {matrix_t<int>::diag(4, 1)};
    matrix_t<int> mat2 {matrix_t<int>::diag(2, vec.begin(), vec.end())};
    matrix_t<int> mat3 {matrix_t<int>::diag(6, vec.begin(), vec.end())};
    matrix_t<int> mat4 {matrix_t<int>::diag(vec.begin(), vec.end())};
    matrix_t<int> mat5 {matrix_t<int>::diag(lst.begin(), lst.end())};

    #ifdef PRINT
    std::cout << "Diag ctor:" << std::endl;
    std::cout << "{diag(4, 1)}:                      " << mat1 << std::endl;
    std::cout << "{diag(2, vec.begin(), vec.end())}: " << mat2 << std::endl;
    std::cout << "{diag(6, vec.begin(), vec.end())}: " << mat3 << std::endl;
    std::cout << "{diag(vec.begin(), vec.end())}:    " << mat4 << std::endl;
    std::cout << "{diag(lst.begin(), lst.end())}:    " << mat5 << std::endl << std::endl;
    #endif
}


int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}