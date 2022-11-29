#include "matrix_container.hpp"

namespace Matrix
{

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
class MatrixArithmetic: public MatrixContainer<T>
{
    static constexpr bool ar_div = ArDiv;
    Cmp cmp;

public:
//----------------------------=| Compare start |=---------------------------
    bool equal_to(const MatrixArithmetic& rhs) const
    {
        if (height_ != rhs.height_ || width_ != rhs.width_)
            return false;

        if (data_ == rhs.data_)
            return true;

        for (size_t i = 0; i < height_; i++)
            for (size_t j = 0; j < width_; j++)
                if (!cmp(to(i, j), rhs.to(i, j)))
                    return false;
                    
        return true;
    }
};

} //Matrix