#include "matrix_arithmetic.hpp"
#include <vector>

struct DblCmp
{
    bool operator()(double d1, double d2)
    {
        return std::abs(d1 - d2) <= (std::abs(d1) + std::abs(d2)) * 1e-10;
    }
};

using namespace Matrix;
using MatrixT = MatrixArithmetic<double, true, DblCmp>;

int main()
{
    std::size_t mat_height = 0;
    std::cin >> mat_height;
    std::size_t mat_sz = mat_height * mat_height;

    try
    {
        std::vector<double> mat_data (mat_sz);
        for (auto& elem: mat_data)
        std::cin >> elem;

        MatrixT matrix {MatrixT::square(mat_height, mat_data.cbegin(), mat_data.cend())};
        std::cout << matrix.determinant() << std::endl;
    }
    catch (std::bad_alloc)
    {
        std::cout << "Bad size" << std::endl;
    }
    return 0;
}