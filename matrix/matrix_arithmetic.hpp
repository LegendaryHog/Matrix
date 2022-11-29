#include "matrix_container.hpp"

namespace Matrix
{

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
class MatrixArithmetic: public MatrixContainer<T>
{
    using base       = MatrixContainer<T>; 
    using size_type  = typename std::size_t;
    using value_type = T;
    
    static constexpr bool ar_div = ArDiv;
    Cmp cmp {};

public:
//----------------------------=| Ctors start |=------------------------------
    MatrixArithmetic(size_type h, size_type w, value_type val = value_type{})
    :base::MatrixContainer(h, w, val)
    {}

    template<std::input_iterator it>
    MatrixArithmetic(size_type h, size_type w, it begin, it end)
    :base::MatrixContainer(h, w, begin, end)
    {}

    MatrixArithmetic(value_type val = value_type{})
    :base::MatrixContainer(val)
    {}

    MatrixArithmetic(std::initializer_list<value_type> onedim_list)
    :base::MatrixContainer(onedim_list)
    {}

    MatrixArithmetic(std::initializer_list<std::initializer_list<value_type>> twodim_list)
    :base::MatrixContainer(twodim_list)
    {}
//----------------------------=| Ctors end |=--------------------------------

//----------------------------=| Compare start |=----------------------------
    bool equal_to(const MatrixArithmetic& rhs) const
    {
        if (this->height_ != rhs.height_ || this->width_ != rhs.width_)
            return false;

        if (this->data_ == rhs.data_)
            return true;

        for (size_t i = 0; i < this->height_; i++)
            for (size_t j = 0; j < this->width_; j++)
                if (!cmp(this->to(i, j), rhs.to(i, j)))
                    return false;                  
        return true;
    }
//----------------------------=| Compare end |=------------------------------
};

} //Matrix