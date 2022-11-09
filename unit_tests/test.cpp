#include <gtest/gtest.h>

#include <list>
#include <vector>
#include <set>

#include "matrix.hpp"


//#define PRINT

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

TEST(Methods, to)
{
    matrix_t<int> bigmat = {{1, 23, 56, 78, 93}, {13, -78, 72, 5667, -9}, {67, 89, 1, 34}};

    EXPECT_EQ(bigmat.to(0, 0), 1);
    EXPECT_EQ(bigmat.to(0, 4), 93);
    EXPECT_EQ(bigmat.to(1, 1), -78);
    EXPECT_EQ(bigmat.to(2, 4), 0);
    EXPECT_EQ(bigmat.to(1, 3), 5667);
    EXPECT_NE(bigmat.to(2, 0), 0);
}

TEST(Methods, swap_row)
{
    matrix_t<int> bigmat = {{34, -6, -8, 90, 55}, {12, 4, 7}, {-1, 3, 8, 9, 78}};
    std::vector<int> origin_row0 = {34, -6, -8, 90, 55};
    std::vector<int> origin_row1 = {12, 4, 7, 0, 0};
    std::vector<int> origin_row2 = {-1, 3, 8, 9, 78};

    bigmat.swap_row(0, 1);
    for (auto i = 0; i < 5; i++)
        EXPECT_EQ(bigmat.to(1, i), origin_row0[i]);
    
    for (auto i = 0; i < 5; i++)
        EXPECT_EQ(bigmat.to(0, i), origin_row1[i]);

    for (auto i = 0; i < 5; i++)
        EXPECT_EQ(bigmat.to(2, i), origin_row2[i]);
}

TEST(Methods, operator_eq)
{
    matrix_t<int> mat1 = {{1, 1, 2}, {23, 56, 78}, {24, 7, -9}};
    matrix_t<int> mat2 = {{1, 1, 2}, {23, 56, 78}, {24, 7, -9}};
    matrix_t<int> mat3 = matrix_t<int>::square(3, 1);
    matrix_t<int> mat4 = matrix_t<int>::diag(2, -7);

    EXPECT_TRUE(mat1 == mat1);
    EXPECT_TRUE(mat2 == mat2);
    EXPECT_TRUE(mat3 == mat3);
    EXPECT_TRUE(mat4 == mat4);

    EXPECT_TRUE(mat1 == mat2);

    EXPECT_FALSE(mat1 == mat3);
    EXPECT_FALSE(mat2 == mat3);
    EXPECT_FALSE(mat3 == mat4);
}

TEST(Methods, square_braces)
{
    const matrix_t<int> c_bigmat = {{1, 23, 56, 78, 93}, {13, -78, 72, 5667, -9}, {67, 89, 1, 34}};
    matrix_t<int> bigmat = {{1, 23, 56, 78, 93}, {13, -78, 72, 5667, -9}, {67, 89, 1, 34}};

    EXPECT_EQ(bigmat[0][0], 1);
    EXPECT_EQ(bigmat[0][4], 93);
    EXPECT_EQ(bigmat[1][1], -78);
    EXPECT_EQ(bigmat[2][4], 0);
    EXPECT_EQ(bigmat[1][3], 5667);
    EXPECT_NE(bigmat[2][0], 0);

    EXPECT_EQ(c_bigmat[0][0], 1);
    EXPECT_EQ(c_bigmat[0][4], 93);
    EXPECT_EQ(c_bigmat[1][1], -78);
    EXPECT_EQ(c_bigmat[2][4], 0);
    EXPECT_EQ(c_bigmat[1][3], 5667);
    EXPECT_NE(c_bigmat[2][0], 0);
}

struct DblCmp {
    bool operator()(double lhs, double rhs) const
    {
        return std::abs(lhs - rhs) <= 0.5*(std::abs(lhs) + std::abs(rhs))*1e-10;
    }
};

TEST(Methods, det_for_double)
{
    DblCmp dbl_cmp {};
    matrix_t<double, true, DblCmp> mat1 = {{1, 12, 3}, {23, 56.8, 78}, {43, 32, 7}};
    matrix_t<double, true, DblCmp> mat2 = {{1, 12, 4.7, -0.3}, {-78, 0.8, 9.6, 87}, {-5, -0.9, 4.7, 21.8}, {0, 2, 7, 9}};
    matrix_t<double, true, DblCmp> mat3 = matrix_t<double, true, DblCmp>::diag(5, 1);
    matrix_t<double, true, DblCmp> mat4 (4, 4, 5.6);
    matrix_t<double, true, DblCmp> mat5 = {{313, 0, 75, 6, 790, 3},
                             {-517, 78, 0.8, 51, 67, -9},
                             {0, 34, 7, -9, -32, -3.3},
                             {-8, 90, 56, 74, 28496, -1},
                             {34567, 2347, 0, 0, 34, 1},
                             {2, 5, 89, 1, 0, 2289}};

    matrix_t<double, true, DblCmp> mat6 = {{1, 0, 34},
                                     {23, 0, -11},
                                     {2, 0, 2}};

    EXPECT_TRUE(dbl_cmp(mat1.det(), 31098.4));
    EXPECT_TRUE(dbl_cmp(mat2.det(), -57462.22));
    EXPECT_TRUE(dbl_cmp(mat3.det(), 1.0));
    EXPECT_TRUE(dbl_cmp(mat4.det(), 0.0));
    EXPECT_TRUE(dbl_cmp(mat5.det(), 480076249541075524.76));
    EXPECT_TRUE(dbl_cmp(mat6.det(), 0.0));
}

TEST(Methods, inverse)
{
    matrix_t<double, true, DblCmp> mat1 = {{1, 12, 3}, {23, 56.8, 78}, {43, 32, 7}};
    matrix_t<double, true, DblCmp> mat2 = {{4, 9}, {1, 2}};

    EXPECT_EQ(mat1.inverse().inverse(), mat1);
    EXPECT_EQ(mat2.inverse().inverse(), mat2);
}

TEST(Methods, det_for_other)
{
    matrix_t mat1 = matrix_t<>::diag(11, 1);
    matrix_t mat2 = {{1,  0, 1},
                     {23, 0, 13},
                     {3,  0, 4}};

    matrix_t mat3 = {{12, -3, 5}, {7, 8, 9}, {4, -7, 8}};
    matrix_t mat4 = {{2, 3}, {2, 17}};
    matrix_t mat5 = {{12, 0}, {2, 0}};
    matrix_t mat6 = {{0, 12}, {0, 2}};
    matrix_t mat7 = {{0, 12},{2, 0}};
    matrix_t mat8 = {{12, 3, 0, 0}, {3, -5}, {1, -9}, {3, -7}};
    matrix_t mat9 = {{1, 2, 0, 0, 3},
                     {3, -4, 0, 0, 3},
                     {2, -8, 0, 0, 9},
                     {15, -17, 0, 0, 5}, 
                     {32, 31, 0, 0, 1}};

    EXPECT_EQ(mat1.det(), 1);
    EXPECT_EQ(mat2.det(), 0);
    EXPECT_EQ(mat3.det(), 1179);
    EXPECT_EQ(mat4.det(), 28);
    EXPECT_EQ(mat5.det(), 0);
    EXPECT_EQ(mat6.det(), 0);
    EXPECT_EQ(mat7.det(), -24);
    EXPECT_EQ(mat8.det(), 0);
    EXPECT_EQ(mat9.det(), 0);
}

TEST(Methods, rang_with_no_floating_points_types)
{
    matrix_t mat1 = matrix_t<>::diag(11, 1);
    matrix_t mat2 = {{1,  0, 1},
                     {23, 0, 13},
                     {3,  0, 4}};

    matrix_t mat3 = {{12, -3, 5}, {7, 8, 9}, {4, -7, 8}};
    matrix_t mat4 = {{2, 3}, {2, 17}};
    matrix_t mat5 = {{12, 0}, {2, 0}};
    matrix_t mat6 = {{0, 12}, {0, 2}};
    matrix_t mat7 = {{0, 12},{2, 0}};
    matrix_t mat8 = {{12, 3, 0, 0}, {3, -5}, {1, -9}, {3, -7}};

    matrix_t mat9 = {{1,   2,  0, 0, 3},
                     {3,  -4,  0, 0, 3},
                     {2,  -8,  0, 0, 9},
                     {15, -17, 0, 0, 5}, 
                     {32,  31, 0, 0, 1}};

    matrix_t mat10 = {{1,  2,   0, 3, 0},
                      {3,  -4,  0, 3, 0},
                      {2,  -8,  0, 9, 0},
                      {15, -17, 0, 5, 0},
                      {32,  32, 0, 1, 0}};


    EXPECT_EQ(mat1.rang(), 11);
    EXPECT_EQ(mat2.rang(), 2);
    EXPECT_EQ(mat3.rang(), 3);
    EXPECT_EQ(mat4.rang(), 2);
    EXPECT_EQ(mat5.rang(), 1);
    EXPECT_EQ(mat6.rang(), 1);
    EXPECT_EQ(mat7.rang(), 2);
    EXPECT_EQ(mat8.rang(), 2);
    EXPECT_EQ(mat9.rang(), 3);
    EXPECT_EQ(mat10.rang(), 3);
}

TEST(Operators, _operator_plus_)
{
    matrix_t lhs1 = {{12, -3, 4}, {31, -5, 8}};
    matrix_t rhs1 = {{1, 0, 3}, {12, 3, 4}};
    matrix_t res1 = lhs1 + rhs1;
    matrix_t res1_example = {{13, -3, 7}, {43, -2, 12}};

    EXPECT_EQ(res1_example, res1);
}

TEST(Operators, operator_unary_minus)
{
    matrix_t mat = {{-3, 7, -9}, {14, -5, -8}, {21, -4, -7}};
    matrix_t minus_mat = {{3, -7, 9}, {-14, 5, 8}, {-21, 4, 7}};

    EXPECT_EQ(-mat, minus_mat);
}

TEST(Operators, operator_minus)
{
    matrix_t lhs1 = {{12, -3, 4}, {31, -5, 8}};
    matrix_t rhs1 = {{1, 0, 3}, {12, 3, 4}};
    matrix_t res1 = lhs1 - rhs1;
    matrix_t res1_example = {{11, -3, 1}, {19, -8, 4}};

    EXPECT_EQ(res1_example, res1);
}

TEST(Operators, operator_mul)
{
    matrix_t mat = {{12, -5, 1}, {3, 9, -5}};
    matrix_t two_mat = {{24, -10, 2}, {6, 18, -10}};

    EXPECT_EQ(two_mat, 2 * mat);
    EXPECT_EQ(two_mat, mat * 2);
}

TEST(Operators, operator_div)
{
    matrix_t half_mat = {{12, -5, 1}, {3, 9, -5}};
    matrix_t mat = {{24, -10, 2}, {6, 18, -10}};

    EXPECT_EQ(half_mat, mat / 2);
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}