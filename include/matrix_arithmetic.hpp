#pragma once
#include "matrix_container.hpp"

namespace Matrix
{

template<typename T>
concept is_abs_available = requires(const T& arg) {std::abs(arg);};

namespace detail
{
template<typename T>
struct DefaultAbs
{
    auto operator()(const T& arg) const requires is_abs_available<T> {return std::abs(arg);}
    int  operator()(const T& arg) const {return 0;}
};

}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = detail::DefaultAbs<T>>
/*
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * IsDivArithm - is division arritmetical correct.                               |
 * If for all a, b, c of type T: b != null_obj -> (a / b == c) == (b * c == a)   |
 * then I say that division operation for T defined arrithmetical correct        |
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
class MatrixArithmetic : public MatrixContainer<T> 
{
public:
    using base = MatrixContainer<T>; 

    using typename base::size_type;
    using typename base::value_type;
    using typename base::reference;
    using typename base::const_reference;
    using typename base::pointer;
    using typename base::const_pointer;

    using typename base::Row;

    using typename base::RowIterator;
    using typename base::RowConstIterator;
    using typename base::Iterator;
    using typename base::ConstIterator;
    using typename base::ReverseIterator;
    using typename base::ConstReverseIterator;

protected:    
    static constexpr bool is_div_arithmetical = IsDivArithm;
    Cmp cmp {};
    Abs abs {};

public:
//--------------------------------=| Ctors start |=-----------------------------------------------------
    MatrixArithmetic(): base() {}

    MatrixArithmetic(size_type h, size_type w): base(h, w) {}

    MatrixArithmetic(size_type h, size_type w, const_reference val)
    :base(h, w, val)
    {}

    template<std::input_iterator InpIt>
    MatrixArithmetic(size_type h, size_type w, InpIt begin, InpIt end)
    :base(h, w, begin, end)
    {}

    MatrixArithmetic(const_reference val)
    :base(val)
    {}

    MatrixArithmetic(std::initializer_list<value_type> onedim_list)
    :base(onedim_list)
    {}

    MatrixArithmetic(std::initializer_list<std::initializer_list<value_type>> twodim_list)
    :base(twodim_list)
    {}
//--------------------------------=| Ctors end |=-------------------------------------------------------

//--------------------------------=| Algorithm fucntions start |=---------------------------------------
protected:
    size_type row_with_max_fst(size_type iteration)
    {
        size_type res = iteration;
        for (size_type i = iteration; i < this->height(); i++)
            if (abs(this->to(i, iteration)) > abs(this->to(res, iteration)))
                res = i;
        return res;
    }

    // method for types with non aritmetic division by Bareiss algorithm Bareiss 
    value_type make_upper_triangular_square(size_type side_of_square) 
    {
        if (side_of_square > std::min(this->height(), this->width()))
            throw std::invalid_argument{"try to make upper triangular square that no inside matrix"};

        value_type div_coef {1};
        value_type sign {1};
        value_type null_obj {};
        for (size_type i = 0; i < side_of_square - 1; i++)
        {
            auto row_to_swap = row_with_max_fst(i);
            if (row_to_swap != i)
            {
                this->swap_row(i, row_to_swap);
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
    value_type make_upper_triangular_square(size_type side_of_square) requires is_div_arithmetical
    {
        if (side_of_square > std::min(this->height(), this->width()))
            throw std::invalid_argument{"try to make upper triangular square that no inside matrix"};
    
        value_type sign {1};
        value_type null_obj {};
        for (size_type i = 0; i < side_of_square - 1; i++)
        {
            auto row_to_swap = row_with_max_fst(i);
            if (row_to_swap != i)
            {
                this->swap_row(i, row_with_max_fst(i));
                sign *= value_type{-1};
            }
            if (!cmp(this->to(i, i), null_obj))
                for (size_type j = i + 1; j < side_of_square; j++)
                {
                    value_type coef = this->to(j, i) / this->to(i, i);
                    for (size_type k = i; k < this->width(); k++)
                        this->to(j, k) -= coef * this->to(i, k);
                }
        }
        return sign;
    }

    void make_eye_square_from_upper_triangular_square(size_type side_of_square) requires is_div_arithmetical
    {
        for (size_type i = side_of_square - 1; static_cast<long long>(i) >= 0; i--)
        {
            auto coef = this->to(i, i);
            for (size_type j = i; j < this->width(); j++)
                this->to(i, j) /= coef;
        }
        
        for (size_type i = side_of_square - 1; static_cast<long long>(i) >= 0; i--)
            for (size_type j = 0; j < i; j++)
            {
                auto coef = this->to(j, i);
                for(size_type k = i; k < this->width(); k++)
                    this->to(j, k) -= this->to(i, k) * coef;
            }
    }

    value_type determinant_for_upper_triangular() const requires is_div_arithmetical
    {
        value_type res {1};
        for (size_type i = 0; i < this->height(); i++)
            res *= this->to(i, i);
        return res;
    }
//--------------------------------=| Algorithm fucntions end |=-----------------------------------------

//--------------------------------=| Public methods start |=--------------------------------------------
public:
    value_type determinant() const requires is_div_arithmetical
    {
        if (!this->is_square())
            throw std::invalid_argument{"try to get determinant() of no square matrix"};

        MatrixArithmetic cpy {*this};
        value_type sign = cpy.make_upper_triangular_square(this->height());
        return sign * cpy.determinant_for_upper_triangular();
    }

    value_type determinant() const
    {
        if (!this->is_square())
            throw std::invalid_argument{"try to get determinant() of no square matrix"};

        MatrixArithmetic cpy (*this);
        return cpy.make_upper_triangular_square(this->height());
    }

    std::pair<bool, MatrixArithmetic> inverse_pair() const requires is_div_arithmetical
    {
        if (!this->is_square())
            throw std::invalid_argument{"try to get inverse matrix of no square matrix"};

        MatrixArithmetic extended_mat (this->height(), 2 * this->height());
        for (size_type i = 0; i < this->height(); i++)
            for (size_type j = 0; j < this->height(); j++)
                extended_mat.to(i, j) = this->to(i, j);
        
        for (size_type i = 0; i < this->height(); i++)
            extended_mat.to(i, i + this->height()) = value_type{1};

        extended_mat.make_upper_triangular_square(extended_mat.height());

        if (extended_mat.determinant_for_upper_triangular() == value_type{})
            return {false, MatrixArithmetic{value_type{0}}};

        extended_mat.make_eye_square_from_upper_triangular_square(extended_mat.height());
        
        MatrixArithmetic res (this->height(), this->height());
        for (size_type i = 0; i < this->height(); i++)
            for (size_type j = 0; j < this->height(); j++)
                res.to(i, j) = extended_mat.to(i, j + this->height());
        
        return {true, res};
    }

    MatrixArithmetic inverse() const requires is_div_arithmetical
    {
        auto res_pair = inverse_pair();
        if (!res_pair.first)
            throw std::invalid_argument{"try to get inverse matrix for matrix with determinant equal to zero"};

        return res_pair.second;
    }

    MatrixArithmetic transpos() const
    {
        MatrixArithmetic res (this->width(), this->height());
        for (size_type i = 0; i < this->height(); i++)
            for (size_type j = 0; j < this->width(); j++)
                res.to(j, i) = this->to(i, j);
        return res;
    }
//--------------------------------=| Public methods end |=----------------------------------------------

//--------------------------------=| Compare start |=---------------------------------------------------
    bool equal_to(const MatrixArithmetic& rhs) const
    {
        if (this->height() != rhs.height() || this->width() != rhs.width())
            return false;

        for (size_type i = 0; i < this->height(); i++)
            for (size_type j = 0; j < this->width(); j++)
                if (!cmp(this->to(i, j), rhs.to(i, j)))
                    return false;                  
        return true;
    }
//--------------------------------=| Compare end |=-----------------------------------------------------

//--------------------------------=| Basic arithmetic start |=------------------------------------------
    MatrixArithmetic& operator+=(const MatrixArithmetic& rhs)
    {
        if (this->height() != rhs.height() || this->width() != rhs.width())
            throw std::invalid_argument{"Try to add matrixes with different height() * width()"};

        for (size_type i = 0; i < this->height(); i++)
            for (size_type j = 0; j < this->width(); j++)
                this->to(i, j) += rhs.to(i, j);

        return *this;
    }

    MatrixArithmetic& operator-=(const MatrixArithmetic& rhs)
    {
        if (this->height() != rhs.height() || this->width() != rhs.width())
            throw std::invalid_argument{"Try to sub matrixes with different height() * width()"};

        for (size_type i = 0; i < this->height(); i++)
            for (size_type j = 0; j < this->width(); j++)
                this->to(i, j) -= rhs.to(i, j);

        return *this;
    }

    MatrixArithmetic operator-() const
    {
        MatrixArithmetic res (this->height(), this->width());

        for (size_type i = 0; i < this->height(); i++)
            for (size_type j = 0; j < this->width(); j++)
                res.to(i, j) = -this->to(i, j);

        return res;
    }

    MatrixArithmetic& operator*=(const_reference rhs)
    {
        for (auto& row: *this)
            for (auto& elem: row)
                elem *= rhs;
        return *this;
    }

    MatrixArithmetic& operator/=(const_reference rhs)
    {
        for (auto& row: *this)
            for (auto& elem: row)
                elem /= rhs;
        return *this;
    }
//--------------------------------=| Basic arithmetic end |=--------------------------------------------

//--------------------------------=| Specific static ctors start |=-------------------------------------
    static MatrixArithmetic square(size_type sz)
    {
        return MatrixArithmetic(sz, sz);
    }

    static MatrixArithmetic square(size_type sz, const_reference val)
    {
        return MatrixArithmetic(sz, sz, val);
    }

    template<std::input_iterator InpIt>
    static MatrixArithmetic square(size_type sz, InpIt begin, InpIt end)
    {
        return MatrixArithmetic(sz, sz, begin, end);
    }

    template<std::input_iterator InpIt>
    static MatrixArithmetic diag(size_type sz, InpIt begin, InpIt end)
    {
        MatrixArithmetic result (square(sz));
        auto itr = begin;
        for (size_type i = 0; i < sz && itr != end; ++itr, i++)
            result.to(i, i) = *itr;
        return result;
    }

    template<std::input_iterator InpIt>
    static MatrixArithmetic diag(InpIt begin, InpIt end)
    {
        size_type sz = 0;
        for (auto itr = begin; itr != end; ++itr, sz++) {}
        return diag(sz, begin, end);
    }

    static MatrixArithmetic diag(size_type sz, const_reference val)
    {
        MatrixArithmetic result (square(sz));
        for (size_type i = 0; i < sz; i++)
            result.to(i, i) = val;
        return result;
    }

    static MatrixArithmetic eye(size_type sz)
    {
        return diag(sz, value_type{1});
    }
//--------------------------------=| Specific static ctors end |=---------------------------------------
}; // class MatrixArithmetic

//--------------------------------=| Cast to scalar start |=--------------------------------------------
template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = detail::DefaultAbs<T>>
T scalar_cast(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& mat)
{
    if (!mat.is_scalar())
        throw std::invalid_argument{"Try to cast MatrixArithmetic in value_type, but matrix isnt scalar"};
    return mat.to(0, 0);
}
//--------------------------------=| Cast to scalar end |=----------------------------------------------

//--------------------------------=| Wrappers arounf methods start |=-----------------------------------
template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = detail::DefaultAbs<T>>
T determinant(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& mat)
{
    return mat.determinant();
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = detail::DefaultAbs<T>>
std::pair<bool, MatrixArithmetic<T, IsDivArithm, Cmp, Abs>> inverse_pair(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& mat)
{
    return mat.inverse_pair();
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = detail::DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> inverse(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& mat)
{
    return mat.inverse();
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = detail::DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> transpos(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& mat)
{
    return mat.transpos();
}
//--------------------------------=| Wrappers arounf methods end |=-------------------------------------

//--------------------------------=| Arrithmetical operators start |=-----------------------------------
template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = detail::DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> product(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& lhs, const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& rhs)
{
    if (lhs.is_scalar())
    {
        MatrixArithmetic res (rhs);
        const auto& scalar = scalar_cast(lhs);
        for (auto& row: res)
            for (auto& elem: row)
                elem *= scalar;
        return res;
    }
    if (rhs.is_scalar())
    {
        MatrixArithmetic res (lhs);
        const auto& scalar = scalar_cast(rhs);
        for (auto& row: res)
            for (auto& elem: row)
                elem *= scalar;
        return res;
    }
    if (lhs.width() != rhs.height())
        throw std::invalid_argument{"in product: lhs.width() != rhs.height()"};

    MatrixArithmetic<T, IsDivArithm, Cmp, Abs> res (lhs.height(), rhs.width());

    using size_type = typename MatrixArithmetic<T, IsDivArithm, Cmp, Abs>::size_type;

    for (size_type i = 0; i < lhs.height(); i++)
        for (size_type j = 0; j < rhs.width(); j++)
            for (size_type k = 0; k < lhs.width(); k++)
                res[i][j] += lhs[i][k] * rhs[k][j];

    return res; 
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = detail::DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> power(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& mat, long long pow)
    {
        if (!mat.is_square())
            throw std::invalid_argument{"Try to make matrix in some power but this matrix is not square"};

        if (pow == 0)
            return MatrixArithmetic<T, IsDivArithm, Cmp, Abs>::eye(mat.height());

        MatrixArithmetic<T, IsDivArithm, Cmp, Abs> res (mat);

        if (pow < 0)
        {
            res = res.inverse();
            pow = -pow;
        }
        
        for (long long i = 1; i < pow; i++)
            res = product(mat, res);
        
        return res;
    }

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = detail::DefaultAbs<T>>
bool operator==(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& lhs, const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& rhs)
{
    return lhs.equal_to(rhs);
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = detail::DefaultAbs<T>>
bool operator!=(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& lhs, const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& rhs)
{
    return !lhs.equal_to(rhs);
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = detail::DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> operator+(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& lhs, const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& rhs)
{
    MatrixArithmetic<T, IsDivArithm, Cmp, Abs> lhs_cpy (lhs);
    return (lhs_cpy += rhs);
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = detail::DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> operator-(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& lhs, const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& rhs)
{
    MatrixArithmetic<T, IsDivArithm, Cmp, Abs> lhs_cpy (lhs);
    return (lhs_cpy -= rhs);
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = detail::DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> operator*(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& lhs, const T& rhs)
{
    MatrixArithmetic<T, IsDivArithm, Cmp, Abs> lhs_cpy (lhs);
    return (lhs_cpy *= rhs);
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = detail::DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> operator*(const T& lhs, const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& rhs)
{
    return rhs * lhs;
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = detail::DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> operator/(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& lhs, const T& rhs)
{
    MatrixArithmetic<T, IsDivArithm, Cmp, Abs> lhs_cpy (lhs);
    return (lhs_cpy /= rhs);
}
//--------------------------------=| Arrithmetical operators end |=-------------------------------------
} // namespace Matrix