#include <gtest/gtest.h>

#include <list>
#include <vector>
#include <set>
#include <array>

#include "matrix_arithmetic.hpp"
#include "container.hpp"

//#define PRINT

using namespace Matrix;

TEST(Container_Array, ctor_size_acces_opers_big_five)
{
    Container::Array<int> arr1 (4);
    EXPECT_EQ(arr1.size(), 4);

    for(std::size_t i = 0; i < arr1.size(); i++)
    {
        EXPECT_EQ(arr1[i], 0);
        EXPECT_EQ(arr1.at(i), 0);
    }
    EXPECT_THROW(arr1.at(4), std::out_of_range);

    arr1[0] = 3;
    arr1[1] = 56;
    arr1[2] = 34;
    arr1[3] = 7;

    const Container::Array carr2 {arr1};
    EXPECT_EQ(carr2.size(), arr1.size());
    for (std::size_t i = 0; i < arr1.size(); i++)
        EXPECT_EQ(arr1[i], carr2[i]);

    Container::Array arr3 {std::move(arr1)};
    Container::Array arr4 {std::move(carr2)};

    EXPECT_EQ(arr3.size(), arr4.size());
    EXPECT_EQ(arr3.size(), 4);
    for (std::size_t i = 0; i < arr3.size(); i++)
        EXPECT_EQ(arr3[i], arr4[i]);

    Container::Array<int> arr5 {3, 56, 34, 7, 8};
    EXPECT_EQ(arr5.size(), 5);

    arr5 = arr3;
    EXPECT_EQ(arr5.size(), arr3.size());
    for (std::size_t i = 0; i < arr5.size(); i++)
        EXPECT_EQ(arr3[i], arr5[i]);

    Container::Array<int> arr6 {13, 56, 87, 189, 789};
    arr5 = std::move(arr6);

    Container::Array<int> arr7 {13, 56, 87, 189, 789};
    EXPECT_EQ(arr7.size(), arr5.size());
    EXPECT_EQ(arr7.size(), 5);
    
    for (std::size_t i = 0; i < 5; i++)
    {
        EXPECT_EQ(arr5[i], arr7[i]);
        EXPECT_EQ(arr5.at(i), arr7.at(i));
    }

    std::array<int, 6> sarr {135, 45, 6, 5, 89, 90};
    Container::Array<int> arr8 (sarr.begin(), sarr.end());
    EXPECT_EQ(sarr.size(), arr8.size());
    for (std::size_t i = 0; i < 6; i++)
        EXPECT_EQ(sarr[i], arr8[i]);
}

TEST(Container_Array, Iterators)
{
    static_assert(std::random_access_iterator<Container::Array<>::Iterator>);
    static_assert(std::random_access_iterator<Container::Array<>::ConstIterator>);

    Container::Array<int> arr {23, 46, 78, 89, 90};
    std::array<int, 5> cpy_arr {23, 46, 78, 89, 90};

    EXPECT_EQ(arr.end() - arr.begin(), arr.size());
    auto itr = arr.begin();
    itr++;
    EXPECT_EQ(++itr, arr.begin() + 2);
    EXPECT_LT(itr, arr.end());
    EXPECT_GT(arr.end(), itr);
    EXPECT_LE(itr += 2, arr.end());
    EXPECT_LE(itr, --arr.end());
    int i = 0;
    for (auto& elem: arr)
    {
        EXPECT_EQ(elem, cpy_arr[i++]);
    }
    EXPECT_EQ(arr.begin()[3], 89);

    EXPECT_EQ(arr.cend() - arr.cbegin(), arr.size());
    auto citr = arr.cbegin();
    citr++;
    EXPECT_EQ(++citr, arr.cbegin() + 2);
    EXPECT_LT(citr, arr.cend());
    EXPECT_GT(arr.cend(), citr);
    EXPECT_LE(citr += 2, arr.cend());
    EXPECT_LE(citr, --arr.cend());
    int j = 0;
    for (auto elem: arr)
    {
        EXPECT_EQ(elem, cpy_arr[j++]);
    }
    EXPECT_EQ(arr.cbegin()[2], 78);

    Container::Array<std::pair<int, int>> arr_p = {{1, 56}, {2, 4}, {31, 3}, {5, 8}, {7, 9}};
    EXPECT_EQ((arr_p.begin() + 1)->first, 2);
    EXPECT_EQ((arr_p.end() - 1)->second, 9);

    EXPECT_EQ((arr_p.cbegin() + 1)->first, 2);
    EXPECT_EQ((arr_p.cend() - 1)->second, 9);
}

TEST(Constructors, by_1_val)
{
    MatrixArithmetic<int> mat1 (1, 1, 1);
    MatrixArithmetic<int> mat2 (3, 3, 2);
    MatrixArithmetic<int> mat3 (3, 3);
    MatrixArithmetic<int> mat4 (1, 1);
    MatrixArithmetic<int> mat5;

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

    MatrixArithmetic<int> mat1 (2, 3, vec.begin(), vec.end());
    MatrixArithmetic<int> mat2 (2, 2, vec.begin(), vec.end());
    MatrixArithmetic<int> mat3 (3, 2, vec.begin(), vec.end());
    MatrixArithmetic<int> mat4 (2, 3, list.begin(), list.end());
    MatrixArithmetic<int> mat5 (2, 2, list.begin(), list.end());
    MatrixArithmetic<int> mat6 (4, 4, list.begin(), list.end());

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
    MatrixArithmetic<int> mat1 {2};
    MatrixArithmetic<int> mat2 (3);
    MatrixArithmetic<int> mat3 = 4;
    MatrixArithmetic<int> mat4 {2, 3, 2};
    MatrixArithmetic<int> mat5 = {2, 3, 2};

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
    MatrixArithmetic<int> mat1 {{1, 0}, {0, 1}};
    MatrixArithmetic<int> mat2 {{1}, {0, 1}};
    MatrixArithmetic<int> mat3 {{1, 2}, {5, 6, 7, 8}, {}};
    MatrixArithmetic<int> mat4 {{1, 1, 2}, {}};
    MatrixArithmetic<int> mat5 {{1, 1, 2}};

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
    MatrixArithmetic<int> mat1 {{1}, {0, 1}, {0, 0, 1}};
    MatrixArithmetic<int> mat2 {mat1};
    MatrixArithmetic<int> mat3 = mat1;
    MatrixArithmetic<int> mat4 = mat1;
    mat4 = mat2;

    #ifdef PRINT
    std::cout << "Test copy:" << std::endl;
    std::cout << "mat1:        " << mat1 << std::endl;
    std::cout << "mat2 {mat1}: " << mat2 << std::endl;
    std::cout << "mat3 = mat1: " << mat3 << std::endl;
    std::cout << "mat4 = mat2: " << mat4 << std::endl << std::endl;
    #endif
}

MatrixArithmetic<int> ret_tmp()
{
    return MatrixArithmetic<int>(2, 2, 1);
}

TEST(Constructors, move_ctor_and_move_assignment)
{
    MatrixArithmetic<int> mat1 {ret_tmp()};
    MatrixArithmetic<int> mat2 (3, 3, 1);
    MatrixArithmetic<int> mat3 (2, 1, 3);
    mat3 = std::move(mat2);

    #ifdef PRINT
    std::cout << "Test move:" << std::endl;
    std::cout << "mat1 ctored by tmp (2, 2, 1):     " << mat1 << std::endl;
    std::cout << "mat2 (3, 3, 1) mat3 = move(mat2): " << mat3 << std::endl << std::endl;
    #endif
}

TEST(Methods, to)
{
    MatrixArithmetic<int> bigmat = {{1, 23, 56, 78, 93}, {13, -78, 72, 5667, -9}, {67, 89, 1, 34}};

    EXPECT_EQ(bigmat.to(0, 0), 1);
    EXPECT_EQ(bigmat.to(0, 4), 93);
    EXPECT_EQ(bigmat.to(1, 1), -78);
    EXPECT_EQ(bigmat.to(2, 4), 0);
    EXPECT_EQ(bigmat.to(1, 3), 5667);
    EXPECT_NE(bigmat.to(2, 0), 0);
}

TEST(Methods, swap_row)
{
    MatrixArithmetic<int> bigmat = {{34, -6, -8, 90, 55}, {12, 4, 7}, {-1, 3, 8, 9, 78}};
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
    MatrixArithmetic<int> mat1 = {{1, 1, 2}, {23, 56, 78}, {24, 7, -9}};
    MatrixArithmetic<int> mat2 = {{1, 1, 2}, {23, 56, 78}, {24, 7, -9}};
    MatrixArithmetic<int> mat3 = MatrixArithmetic<int>::square(3, 1);
    MatrixArithmetic<int> mat4 = MatrixArithmetic<int>::diag(2, -7);

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
    const MatrixArithmetic<int> c_bigmat = {{1, 23, 56, 78, 93}, {13, -78, 72, 5667, -9}, {67, 89, 1, 34}};
    MatrixArithmetic<int> bigmat = {{1, 23, 56, 78, 93}, {13, -78, 72, 5667, -9}, {67, 89, 1, 34}};

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
    MatrixArithmetic<double, true, DblCmp> mat1 = {{1, 12, 3}, {23, 56.8, 78}, {43, 32, 7}};
    MatrixArithmetic<double, true, DblCmp> mat2 = {{1, 12, 4.7, -0.3}, {-78, 0.8, 9.6, 87}, {-5, -0.9, 4.7, 21.8}, {0, 2, 7, 9}};
    MatrixArithmetic<double, true, DblCmp> mat3 = MatrixArithmetic<double, true, DblCmp>::diag(5, 1);
    MatrixArithmetic<double, true, DblCmp> mat4 (4, 4, 5.6);
    MatrixArithmetic<double, true, DblCmp> mat5 = {{313, 0, 75, 6, 790, 3},
                             {-517, 78, 0.8, 51, 67, -9},
                             {0, 34, 7, -9, -32, -3.3},
                             {-8, 90, 56, 74, 28496, -1},
                             {34567, 2347, 0, 0, 34, 1},
                             {2, 5, 89, 1, 0, 2289}};

    MatrixArithmetic<double, true, DblCmp> mat6 = {{1, 0, 34},
                                     {23, 0, -11},
                                     {2, 0, 2}};

    MatrixArithmetic<double, true, DblCmp> mat7 = {{0.0, 1832.25, -2427.0}, {0.0, -945.0, 1242.0}, {0.0, 1911.0, -2523.0}};

    EXPECT_TRUE(dbl_cmp(mat1.determinant(), 31098.4));
    EXPECT_TRUE(dbl_cmp(mat2.determinant(), -57462.22));
    EXPECT_TRUE(dbl_cmp(mat3.determinant(), 1.0));
    EXPECT_TRUE(dbl_cmp(mat4.determinant(), 0.0));
    EXPECT_TRUE(dbl_cmp(mat5.determinant(), 480076249541075524.76));
    EXPECT_TRUE(dbl_cmp(mat6.determinant(), 0.0));
    EXPECT_TRUE(dbl_cmp(mat7.determinant(), -0.0));
}

TEST(Methods, inverse)
{
    MatrixArithmetic<double, true, DblCmp> mat1 = {{1, 12, 3}, {23, 56.8, 78}, {43, 32, 7}};
    MatrixArithmetic<double, true, DblCmp> mat2 = {{4, 9}, {1, 2}};
    MatrixArithmetic eye_mat {MatrixArithmetic<double, true, DblCmp>::eye(6)};

    EXPECT_EQ(mat1.inverse().inverse(), mat1);
    EXPECT_EQ(mat2.inverse().inverse(), mat2);
    EXPECT_EQ(eye_mat.inverse(), eye_mat);
}

TEST(Methods, det_for_other)
{
    MatrixArithmetic mat1 = MatrixArithmetic<>::diag(11, 1);
    MatrixArithmetic mat2 = {{1,  0, 1},
                     {23, 0, 13},
                     {3,  0, 4}};

    MatrixArithmetic mat3 = {{12, -3, 5}, {7, 8, 9}, {4, -7, 8}};
    MatrixArithmetic mat4 = {{2, 3}, {2, 17}};
    MatrixArithmetic mat5 = {{12, 0}, {2, 0}};
    MatrixArithmetic mat6 = {{0, 12}, {0, 2}};
    MatrixArithmetic mat7 = {{0, 12},{2, 0}};
    MatrixArithmetic mat8 = {{12, 3, 0, 0}, {3, -5}, {1, -9}, {3, -7}};
    MatrixArithmetic mat9 = {{1, 2, 0, 0, 3},
                             {3, -4, 0, 0, 3},
                             {2, -8, 0, 0, 9},
                             {15, -17, 0, 0, 5}, 
                             {32, 31, 0, 0, 1}};

    EXPECT_EQ(mat1.determinant(), 1);
    EXPECT_EQ(mat2.determinant(), 0);
    EXPECT_EQ(mat3.determinant(), 1179);
    EXPECT_EQ(mat4.determinant(), 28);
    EXPECT_EQ(mat5.determinant(), 0);
    EXPECT_EQ(mat6.determinant(), 0);
    EXPECT_EQ(mat7.determinant(), -24);
    EXPECT_EQ(mat8.determinant(), 0);
    EXPECT_EQ(mat9.determinant(), 0);
}

/*TEST(Methods, rang_with_no_floating_points_types)
{
    MatrixArithmetic mat1 = MatrixArithmetic<>::diag(11, 1);
    MatrixArithmetic mat2 = {{1,  0, 1},
                     {23, 0, 13},
                     {3,  0, 4}};

    MatrixArithmetic mat3 = {{12, -3, 5}, {7, 8, 9}, {4, -7, 8}};
    MatrixArithmetic mat4 = {{2, 3}, {2, 17}};
    MatrixArithmetic mat5 = {{12, 0}, {2, 0}};
    MatrixArithmetic mat6 = {{0, 12}, {0, 2}};
    MatrixArithmetic mat7 = {{0, 12},{2, 0}};
    MatrixArithmetic mat8 = {{12, 3, 0, 0}, {3, -5}, {1, -9}, {3, -7}};

    MatrixArithmetic mat9 = {{1,   2,  0, 0, 3},
                     {3,  -4,  0, 0, 3},
                     {2,  -8,  0, 0, 9},
                     {15, -17, 0, 0, 5}, 
                     {32,  31, 0, 0, 1}};

    MatrixArithmetic mat10 = {{1,  2,   0, 3, 0},
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
}*/

TEST(Operators, _operator_plus_)
{
    MatrixArithmetic lhs1 = {{12, -3, 4}, {31, -5, 8}};
    MatrixArithmetic rhs1 = {{1, 0, 3}, {12, 3, 4}};
    MatrixArithmetic lhs2 {lhs1};
    MatrixArithmetic rhs2 {rhs1};
    MatrixArithmetic res1 = lhs1 + rhs1;
    MatrixArithmetic res1_example = {{13, -3, 7}, {43, -2, 12}};

    EXPECT_EQ(res1_example, res1);
    EXPECT_EQ(res1_example, lhs2 + rhs2);
}

TEST(Operators, operator_unary_minus)
{
    MatrixArithmetic mat = {{-3, 7, -9}, {14, -5, -8}, {21, -4, -7}};
    MatrixArithmetic minus_mat = {{3, -7, 9}, {-14, 5, 8}, {-21, 4, 7}};

    EXPECT_EQ(-mat, minus_mat);
}

TEST(Operators, operator_minus)
{
    MatrixArithmetic lhs1 = {{12, -3, 4}, {31, -5, 8}};
    MatrixArithmetic rhs1 = {{1, 0, 3}, {12, 3, 4}};
    MatrixArithmetic res1 = lhs1 - rhs1;
    MatrixArithmetic res1_example = {{11, -3, 1}, {19, -8, 4}};

    EXPECT_EQ(res1_example, res1);
}

TEST(Operators, operator_mul)
{
    MatrixArithmetic mat = {{12, -5, 1}, {3, 9, -5}};
    MatrixArithmetic two_mat = {{24, -10, 2}, {6, 18, -10}};

    EXPECT_EQ(two_mat, 2 * mat);
    EXPECT_EQ(two_mat, mat * 2);
}

TEST(Operators, operator_div)
{
    MatrixArithmetic half_mat = {{12, -5, 1}, {3, 9, -5}};
    MatrixArithmetic mat      = {{24, -10, 2}, {6, 18, -10}};

    EXPECT_EQ(half_mat, mat / 2);
}

TEST(Operators, cast_to_scalar)
{
    MatrixArithmetic scalar_mat {4};
    MatrixArithmetic mat (2, 2, 1);
    auto value_int = 5;

    EXPECT_EQ(scalar_mat + value_int, 9);
    EXPECT_EQ(value_int + scalar_mat, 9);
    EXPECT_THROW(value_int = mat + 5, std::invalid_argument);
    EXPECT_THROW(value_int = 5 + mat, std::invalid_argument);
}

TEST(Methods, power)
{
    MatrixArithmetic eye_mat {MatrixArithmetic<double, true, DblCmp>::eye(10)};
    try {EXPECT_EQ(power(eye_mat, -5), eye_mat);}
    catch (std::invalid_argument) {std::cerr << "first" << std::endl; throw;}
    try {EXPECT_EQ(power(eye_mat, 5), eye_mat);}
    catch (std::invalid_argument) {std::cerr << "second" << std::endl; throw;}
}

TEST(Iterators, Iterator_and_ConstIterator)
{
    static_assert(std::random_access_iterator<MatrixArithmetic<>::Iterator>);
    static_assert(std::random_access_iterator<MatrixArithmetic<>::ConstIterator>);
    
    MatrixArithmetic mat = {{1, 2, 3},
                            {4, 5, 6},
                            {7, 8, 9}};
    mat.swap_col(0, 2);
    // 3 2 1
    // 6 5 4 
    // 9 8 7
    mat.swap_row(1, 2);
    // 3 2 1
    // 9 8 7
    // 6 5 4
    
    Container::Array arr = {3, 2, 1, 9, 8, 7, 6, 5, 4};

    for (auto i = 0; i < 9; i++)
    {
        std::cerr << i << ":" << std::endl;
        EXPECT_EQ(mat.begin()[i], arr[i]);
        EXPECT_EQ(mat.cbegin()[i], arr[i]);
    }

    auto itr = mat.begin();
    auto citr = mat.cbegin();

    itr++;
    citr++;

    EXPECT_EQ(*itr, 2);
    EXPECT_EQ(*citr, 2);

    EXPECT_EQ(*(++itr), 1);
    EXPECT_EQ(*(++citr), 1);

    EXPECT_EQ(itr - 2, mat.begin());
    EXPECT_EQ(citr - 2, mat.cbegin());
    EXPECT_GE(itr - 2, mat.begin());
    EXPECT_GE(citr - 2, mat.cbegin());
    EXPECT_GE(itr, mat.begin());
    EXPECT_GE(citr, mat.cbegin());
    EXPECT_LE(mat.begin(), itr - 2);
    EXPECT_LE(mat.cbegin(), citr - 2);
    EXPECT_LE(mat.begin(), itr);
    EXPECT_LE(mat.cbegin(), citr);

    EXPECT_EQ(mat.end() - mat.begin(), 9);
    EXPECT_EQ(mat.cend() - mat.cbegin(), 9);

    MatrixArithmetic<std::pair<int, int>> mat_p = {{{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}};
    EXPECT_EQ((mat_p.begin() + 2)->first, 5);
    EXPECT_EQ((mat_p.end() - 1)->second, 8);

    EXPECT_EQ((mat_p.cbegin() + 2)->first, 5);
    EXPECT_EQ((mat_p.cend() - 1)->second, 8);
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
