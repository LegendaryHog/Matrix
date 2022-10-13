#include <gtest/gtest.h>
#include "../matrix.hpp"

using namespace matrix;

TEST(Constructors, by_1_val)
{
    matrix_t<int> mat1 (1, 1, 1);
    matrix_t<int> mat2 (3, 3, 2);
    matrix_t<int> mat3 (3, 3);
    matrix_t<int> mat4 (1, 1);

    std::cout << mat1 << std::endl << mat2 << std::endl << mat3 << std::endl << mat4 << std::endl;
}

int main()
{
    return RUN_ALL_TEST();
}