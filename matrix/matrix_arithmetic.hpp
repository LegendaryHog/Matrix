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
//----------------------------=| Ctors start |=-------------------------------
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
//----------------------------=| Ctors end |=---------------------------------------

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++                                                                         
 *----------------------------------------------------------------------------*
 *      ________________________________________________________________      *
 *---==| BE CAREFUL, THIS OPERATIONS INVALIDATE ITERATORS AND REFERENCES|==---*
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~      *
 *----------------------------------------------------------------------------*
 */                               
//----------------------------=| Swap rows and columns start |=---------------------
    void swap_row(size_type ind1, size_type ind2)
    {
        if (ind1 >= this->height_ || ind2 >= this->height_)
            throw std::out_of_range{"try to swap rows with indexis out of range"};

        std::swap(this->row_order_[ind1], this->row_order_[ind2]);
    }

    void swap_col(size_type ind1, size_type ind2)
    {
        if (ind1 >= this->width_ || ind2 >= this->width_)
            throw std::out_of_range{"try to swap columns with indexis out of range"};

        std::swap(this->col_order_[ind1], this->col_order_[ind2]);
    }
//----------------------------=| Swap rows and columns end |=-----------------------

//----------------------------=| Algorithm fucntions start |=-----------------------
private:
    size_type row_with_max_fst(size_type iteration)
    {
        size_type res = iteration;
        for (size_type i = iteration; i < this->height_; i++)
            if (std::abs(this->to(i, iteration)) > std::abs(this->to(res, iteration)))
                res = i;
        return res;
    }

    // method for types with non aritmetic division by Bareiss algorithm Bareiss 
    value_type make_upper_triangular_square(size_type side_of_square) 
    {
        if (side_of_square > std::min(this->height_, this->width_))
            throw std::invalid_argument{"try to make upper triangular square that no inside matrix"};

        value_type div_coef {1};
        value_type sign {1};
        value_type null_obj {};
        for (std::size_t i = 0; i < side_of_square - 1; i++)
        {
            auto row_to_swap = row_with_max_fst(i);
            if (row_to_swap != i)
            {
                swap_row(i, row_with_max_fst(i));
                sign *= value_type{-1};
            }
            if (!cmp(this->to(i, i), null_obj))
            {
                for (size_type j = i + 1; j < side_of_square; j++)
                    for (size_type k = i + 1; k < side_of_square; k++)
                        this->to(j, k) = (this->to(j, k) * this->to(i, i) - this->to(j, i) * this->to(i, k)) / div_coef;
                div_coef = this->to(i, i);
            }
            else
                sign = null_obj;
        }
        return this->to(side_of_square - 1, side_of_square - 1) * sign;
    }
    
    // method for types with arithmetic division by Gauss algorithm
    value_type make_upper_triangular_square(size_type side_of_square) requires (ar_div == true)
    {
        if (side_of_square > std::min(this->height_, this->width_))
            throw std::invalid_argument{"try to make upper triangular square that no inside matrix"};
    
        value_type sign {1};
        value_type null_obj {};
        for (size_type i = 0; i < side_of_square - 1; i++)
        {
            auto row_to_swap = row_with_max_fst(i);
            if (row_to_swap != i)
            {
                swap_row(i, row_with_max_fst(i));
                sign *= value_type{-1};
            }
            if (!cmp(this->to(i, i), null_obj))
                for (size_type j = i + 1; j < side_of_square; j++)
                {
                    value_type coef = this->to(j, i) / this->to(i, i);
                    for (size_type k = i; k < this->width_; k++)
                        this->to(j, k) -= coef * this->to(i, k);
                }
        }
        return sign;
    }

    void make_eye_square_from_upper_triangular_square(size_type side_of_square) requires (ar_div == true)
    {
        for (size_type i = side_of_square - 1; static_cast<long long>(i) >= 0; i--)
        {
            auto coef = this->to(i, i);
            for (size_type j = i; j < this->width_; j++)
                this->to(i, j) = this->to(i, j) / coef;
        }
        
        for (size_type i = side_of_square - 1; static_cast<long long>(i) >= 0; i--)
            for (std::size_t j = 0; j < i; j++)
            {
                auto coef = this->to(j, i);
                for(size_type k = i; k < this->width_; k++)
                    this->to(j, k) -= this->to(i, k) * coef;
            }
    }

    size_type rang_for_upper_triangular() const
    {
        size_type rang_mat  = 0;
        size_type square_sz = std::min(this->height_, this->width_);
        value_type null_obj {};
        for (size_type i = 0; i < square_sz; i++)
            if (!cmp(this->to(i, i), null_obj))
                rang_mat++;
        return rang_mat;      
    }
//----------------------------=| Algorithm fucntions end |=-------------------------

//----------------------------=| Public methods start |=----------------------------
public:
    /*size_type rang() const
    {
        MatrixArithmetic cpy {*this};
        cpy.make_upper_triangular_square(std::min(this->height_, this->width_));
        return cpy.rang_for_upper_triangular();
    }*/

    bool is_square() const {return this->height_ == this->width_;}

    value_type det() const requires (ar_div == true)
    {
        if (!this->is_square())
            throw std::invalid_argument{"try to get det() of no square matrix"};

        MatrixArithmetic cpy {*this};
        value_type sign = cpy.make_upper_triangular_square(this->height_);
        value_type res {1};
        for (size_type i = 0; i < this->height_; i++)
            res *= cpy.to(i, i);
        return sign * res;
    }

    value_type det() const
    {
        if (!this->is_square())
            throw std::invalid_argument{"try to get det() of no square matrix"};

        MatrixArithmetic cpy {*this};
        return cpy.make_upper_triangular_square(this->height_);
    }

    std::pair<bool, MatrixArithmetic> inverse_pair() const requires (ar_div == true)
    {
        if (!this->is_square())
            throw std::invalid_argument{"try to get inverse matrix of no square matrix"};

        MatrixArithmetic extended_mat (this->height_, 2 * this->height_);
        for (size_type i = 0; i < this->height_; i++)
            for (size_type j = 0; j < this->height_; j++)
                extended_mat.to(i, j) = this->to(i, j);
        
        for (size_type i = 0; i < this->height_; i++)
            extended_mat.to(i, i + this->height_) = value_type{1};

        extended_mat.make_upper_triangular_square(extended_mat.height_);

        if (extended_mat.rang_for_upper_triangular() != this->height_)
            return {false, MatrixArithmetic{*this}};

        extended_mat.make_eye_square_from_upper_triangular_square(extended_mat.height_);
        
        MatrixArithmetic res (this->height_, this->height_);
        for (size_type i = 0; i < this->height_; i++)
            for (size_type j = 0; j < this->height_; j++)
                res.to(i, j) = extended_mat.to(i, j + this->height_);
        
        return {true, res};
    }

    MatrixArithmetic inverse() const requires (ar_div == true)
    {
        auto res_pair = inverse_pair();
        if (!res_pair.first)
            throw std::invalid_argument{"try to get inverse matrix for matrix with det equal to zero"};

        return res_pair.second;
    }

    MatrixArithmetic& transpos_it()
    {
        if (!this->is_square())
            throw std::invalid_argument{"try to transpos no square matrix"};

        for (size_type i = 0; i < this->height_; i++)
            for (size_type j = i; j < this->width_; j++)
                std::swap(this->to(i, j), this->to(j, i));

        return *this;
    }

    MatrixArithmetic transpos() const
    {
        MatrixArithmetic res {*this};
        return res.transpos_it();
    }
//----------------------------=| Public methods end |=------------------------------

//----------------------------=| Compare start |=-----------------------------------
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
//----------------------------=| Compare end |=-------------------------------------

//----------------------------=| Basic arithmetic start |=--------------------------
    MatrixArithmetic& operator+=(const MatrixArithmetic& rhs)
    {
        if (this->height_ != rhs.height_ || this->width_ != rhs.width_)
            throw std::invalid_argument("Try to add matrixes with different height_ * width_");

        for (std::size_t i = 0; i < this->height_; i++)
            for (std::size_t j = 0; j < this->width_; j++)
                this->to(i, j) += rhs.to(i, j);

        return *this;
    }

    MatrixArithmetic& operator-=(const MatrixArithmetic& rhs)
    {
        if (this->height_ != rhs.height_ || this->width_ != rhs.width_)
            throw std::invalid_argument("Try to sub matrixes with different height_ * width_");

        for (std::size_t i = 0; i < this->height_; i++)
            for (std::size_t j = 0; j < this->width_; j++)
                this->to(i, j) -= rhs.to(i, j);

        return *this;
    }

    MatrixArithmetic operator-() const
    {
        MatrixArithmetic res (this->height_, this->width_);

        for (std::size_t i = 0; i < this->height_; i++)
            for (std::size_t j = 0; j < this->width_; j++)
                res.to(i, j) = -this->to(i, j);

        return res;
    }

    MatrixArithmetic& operator*=(const T& rhs)
    {
        for (std::size_t i = 0; i < this->height_ * this->width_; i++)
            this->data_[i] *= rhs;
        return *this;
    }

    MatrixArithmetic& operator/=(const T& rhs)
    {
        for (std::size_t i = 0; i < this->height_ * this->width_; i++)
            this->data_[i] /= rhs;
        return *this;
    }
//----------------------------=| Basic arithmetic end |=----------------------------

//----------------------------=| Specific static ctors start |=---------------------
    static MatrixArithmetic square(size_type sz, value_type val = value_type{})
    {
        return MatrixArithmetic(sz, sz, val);
    }

    template<std::input_iterator it>
    static MatrixArithmetic square(size_type sz, it begin, it end)
    {
        return MatrixArithmetic(sz, sz, begin, end);
    }

    template<std::input_iterator it>
    static MatrixArithmetic diag(size_type sz, it begin, it end)
    {
        MatrixArithmetic result {square(sz)};
        auto itr = begin;
        for (auto i = 0; i < sz && itr != end; ++itr, i++)
            result.data_[i * sz + i] = *itr;
        return result;
    }

    template<std::input_iterator it>
    static MatrixArithmetic diag(it begin, it end)
    {
        auto sz = 0;
        for (auto itr = begin; itr != end; ++itr, sz++) {;}
        return diag(sz, begin, end);
    }

    static MatrixArithmetic diag(std::size_t sz, T val = T{})
    {
        MatrixArithmetic result {square(sz)};
        for (auto i = 0; i < sz; i++)
            result.data_[i * sz + i] = val;
        return result;
    }
//----------------------------=| Specific static ctors end |=-----------------------
};

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
T det(const MatrixArithmetic<T, Ardiv, Cmp>& mat)
{
    return mat.det();
}

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
MatrixArithmetic<T, ArDiv, Cmp> inverse(const MatrixArithmetic<T, ArDiv, Cmp>& mat)
{
    return mat.inverse();
}

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
MatrixArithmetic<T, ArDiv, Cmp> product(const MatrixArithmetic<T, ArDiv, Cmp>& lhs, const MatrixArithmetic<T, ArDiv, Cmp>& rhs)
{
    if (lhs.width_ != rhs.height_)
        throw std::invalid_argument{"in product: lhs.width_ != rhs.height_"};

    MatrixArithmetic<T, ArDiv, Cmp> res (lhs.height_, rhs.width_);

    for (std::size_t i = 0; i < lhs.height(); i++)
        for (std::size_t j = 0; j < rhs.width(); j++)
            for (std::size_t k = 0; k < lhs.width(); k++)
                res[i][j] += lhs[i][k] * rhs[k][j];

    return res; 
}

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
bool operator==(const MatrixArithmetic<T, ArDiv, Cmp>& lhs, const MatrixArithmetic<T, ArDiv, Cmp>& rhs)
{
    return lhs.equal_to(rhs);
}

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
bool operator!=(const MatrixArithmetic<T, ArDiv, Cmp>& lhs, const MatrixArithmetic<T, ArDiv, Cmp>& rhs)
{
    return !lhs.equal_to(rhs);
}

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
MatrixArithmetic<T, ArDiv, Cmp> operator+(const MatrixArithmetic<T, ArDiv, Cmp>& lhs, const MatrixArithmetic<T, ArDiv, Cmp>& rhs)
{
    MatrixArithmetic<T, ArDiv, Cmp> lhs_cpy {lhs};
    return (lhs_cpy += rhs);
}

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
MatrixArithmetic<T, ArDiv, Cmp> operator-(const MatrixArithmetic<T, ArDiv, Cmp>& lhs, const MatrixArithmetic<T, ArDiv, Cmp>& rhs)
{
    MatrixArithmetic<T, ArDiv, Cmp> lhs_cpy {lhs};
    return (lhs_cpy -= rhs);
}

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
MatrixArithmetic<T, ArDiv, Cmp> operator*(const MatrixArithmetic<T, ArDiv, Cmp>& lhs, const T& rhs)
{
    MatrixArithmetic<T, ArDiv, Cmp> lhs_cpy {lhs};
    return (lhs_cpy *= rhs);
}

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
MatrixArithmetic<T, ArDiv, Cmp> operator*(const T& lhs, const MatrixArithmetic<T, ArDiv, Cmp>& rhs)
{
    return rhs * lhs;
}

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
MatrixArithmetic<T, ArDiv, Cmp> operator/(const MatrixArithmetic<T, ArDiv, Cmp>& lhs, const T& rhs)
{
    MatrixArithmetic<T, ArDiv, Cmp> lhs_cpy {lhs};
    return (lhs_cpy /= rhs);
}

} //Matrix