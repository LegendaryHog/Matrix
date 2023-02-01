#include "matrix_container.hpp"

namespace Matrix
{

template<typename T>
concept is_abs_available = requires(const T& arg) {std::abs(arg);};

namespace Detail
{
template<typename T>
struct DefaultAbs
{
    auto operator()(const T& arg) const requires is_abs_available<T> {return std::abs(arg);}
    int operator()(const T& arg) const {return 0;};
};

}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = Detail::DefaultAbs<T>>
/*
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * IsDivArithm - is division arritmetical correct.                               |
 * If for all a, b, c of type T: b != null_obj -> (a / b == c) == (b * c == a)   |
 * then I say that division operation for T defined arrithmetical correct        |
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
class MatrixArithmetic : public MatrixContainer<T> 
{
    using base       = MatrixContainer<T>; 
    using size_type  = typename std::size_t;
    using value_type = T;
    
    static constexpr bool is_div_arithmetical = IsDivArithm;
    Cmp cmp {};
    Abs abs {};

public:
//--------------------------------=| Ctors start |=-----------------------------------------------------
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
//--------------------------------=| Ctors end |=-------------------------------------------------------

//--------------------------------=| Virtual dtor for inheritance start |=------------------------------
    virtual ~MatrixArithmetic() {}
//--------------------------------=| Virtual dtor for inheritance end |=--------------------------------

//--------------------------------=| Types start |=-----------------------------------------------------
    bool is_empty()  const {return base::height() == 0;}
    bool is_row()    const {return base::height() == 1;}
    bool is_column() const {return base::width() == 1;}
    bool is_scalar() const {return base::height() == 1 && base::width() == 1;}
    bool is_square() const {return base::height() == base::width();}
//--------------------------------=| Types end |=-------------------------------------------------------

//--------------------------------=| Algorithm fucntions start |=---------------------------------------
protected:
    size_type row_with_max_fst(size_type iteration)
    {
        size_type res = iteration;
        for (size_type i = iteration; i < base::height(); i++)
            if (abs(base::to(i, iteration)) > abs(base::to(res, iteration)))
                res = i;
        return res;
    }

    // method for types with non aritmetic division by Bareiss algorithm Bareiss 
    value_type make_upper_triangular_square(size_type side_of_square) 
    {
        if (side_of_square > std::min(base::height(), base::width()))
            throw std::invalid_argument{"try to make upper triangular square that no inside matrix"};

        value_type div_coef {1};
        value_type sign {1};
        value_type null_obj {};
        for (std::size_t i = 0; i < side_of_square - 1; i++)
        {
            auto row_to_swap = row_with_max_fst(i);
            if (row_to_swap != i)
            {
                base::swap_row(i, row_with_max_fst(i));
                sign *= value_type{-1};
            }
            if (!cmp(base::to(i, i), null_obj))
            {
                for (size_type j = i + 1; j < side_of_square; j++)
                    for (size_type k = i + 1; k < side_of_square; k++)
                        base::to(j, k) = (base::to(j, k) * base::to(i, i) - base::to(j, i) * base::to(i, k)) / div_coef;
                div_coef = base::to(i, i);
            }
            else
                sign = null_obj;
        }
        return base::to(side_of_square - 1, side_of_square - 1) * sign;
    }
    
    // method for types with arithmetic division by Gauss algorithm
    value_type make_upper_triangular_square(size_type side_of_square) requires (is_div_arithmetical == true)
    {
        if (side_of_square > std::min(base::height(), base::width()))
            throw std::invalid_argument{"try to make upper triangular square that no inside matrix"};
    
        value_type sign {1};
        value_type null_obj {};
        for (size_type i = 0; i < side_of_square - 1; i++)
        {
            auto row_to_swap = row_with_max_fst(i);
            if (row_to_swap != i)
            {
                base::swap_row(i, row_with_max_fst(i));
                sign *= value_type{-1};
            }
            if (!cmp(base::to(i, i), null_obj))
                for (size_type j = i + 1; j < side_of_square; j++)
                {
                    value_type coef = base::to(j, i) / base::to(i, i);
                    for (size_type k = i; k < base::width(); k++)
                        base::to(j, k) -= coef * base::to(i, k);
                }
        }
        return sign;
    }

    void make_eye_square_from_upper_triangular_square(size_type side_of_square) requires (is_div_arithmetical == true)
    {
        for (size_type i = side_of_square - 1; static_cast<long long>(i) >= 0; i--)
        {
            auto coef = base::to(i, i);
            for (size_type j = i; j < base::width(); j++)
                base::to(i, j) /= coef;
        }
        
        for (size_type i = side_of_square - 1; static_cast<long long>(i) >= 0; i--)
            for (std::size_t j = 0; j < i; j++)
            {
                auto coef = base::to(j, i);
                for(size_type k = i; k < base::width(); k++)
                    base::to(j, k) -= base::to(i, k) * coef;
            }
    }

    value_type determinant_for_upper_triangular() const requires (is_div_arithmetical == true)
    {
        value_type res {1};
        for (size_type i = 0; i < base::height(); i++)
            res *= base::to(i, i);
        return res;
    }
//--------------------------------=| Algorithm fucntions end |=-----------------------------------------

//--------------------------------=| Public methods start |=--------------------------------------------
public:
    value_type determinant() const requires (is_div_arithmetical == true)
    {
        if (!is_square())
            throw std::invalid_argument{"try to get determinant() of no square matrix"};

        MatrixArithmetic cpy {*this};
        value_type sign = cpy.make_upper_triangular_square(base::height());
        return sign * cpy.determinant_for_upper_triangular();
    }

    value_type determinant() const
    {
        if (!is_square())
            throw std::invalid_argument{"try to get determinant() of no square matrix"};

        MatrixArithmetic cpy {*this};
        return cpy.make_upper_triangular_square(base::height());
    }

    std::pair<bool, MatrixArithmetic> inverse_pair() const requires (is_div_arithmetical == true)
    {
        if (!is_square())
            throw std::invalid_argument{"try to get inverse matrix of no square matrix"};

        MatrixArithmetic extended_mat (base::height(), 2 * base::height());
        for (size_type i = 0; i < base::height(); i++)
            for (size_type j = 0; j < base::height(); j++)
                extended_mat.to(i, j) = base::to(i, j);
        
        for (size_type i = 0; i < base::height(); i++)
            extended_mat.to(i, i + base::height()) = value_type{1};

        extended_mat.make_upper_triangular_square(extended_mat.height());

        if (extended_mat.determinant_for_upper_triangular() == value_type{})
            return {false, MatrixArithmetic{value_type{0}}};

        extended_mat.make_eye_square_from_upper_triangular_square(extended_mat.height());
        
        MatrixArithmetic res (base::height(), base::height());
        for (size_type i = 0; i < base::height(); i++)
            for (size_type j = 0; j < base::height(); j++)
                res.to(i, j) = extended_mat.to(i, j + base::height());
        
        return {true, res};
    }

    MatrixArithmetic inverse() const requires (is_div_arithmetical == true)
    {
        auto res_pair = inverse_pair();
        if (!res_pair.first)
            throw std::invalid_argument{"try to get inverse matrix for matrix with determinant equal to zero"};

        return res_pair.second;
    }

    MatrixArithmetic transpos() const
    {
        MatrixArithmetic res {base::width(), base::height()};
        for (size_type i = 0; i < base::height(); i++)
            for (size_type j = 0; j < base::width(); j++)
                res.to(j, i) = base::to(i, j);
        return res;
    }
//--------------------------------=| Public methods end |=----------------------------------------------

//--------------------------------=| Compare start |=---------------------------------------------------
    bool equal_to(const MatrixArithmetic& rhs) const
    {
        if (base::height() != rhs.height() || base::width() != rhs.width())
            return false;

        for (size_t i = 0; i < base::height(); i++)
            for (size_t j = 0; j < base::width(); j++)
                if (!cmp(base::to(i, j), rhs.to(i, j)))
                    return false;                  
        return true;
    }
//--------------------------------=| Compare end |=-----------------------------------------------------

//--------------------------------=| Basic arithmetic start |=------------------------------------------
    MatrixArithmetic& operator+=(const MatrixArithmetic& rhs)
    {
        if (base::height() != rhs.height() || base::width() != rhs.width())
            throw std::invalid_argument{"Try to add matrixes with different height() * width()"};

        for (std::size_t i = 0; i < base::height(); i++)
            for (std::size_t j = 0; j < base::width(); j++)
                base::to(i, j) += rhs.to(i, j);

        return *this;
    }

    MatrixArithmetic& operator-=(const MatrixArithmetic& rhs)
    {
        if (base::height() != rhs.height() || base::width() != rhs.width())
            throw std::invalid_argument{"Try to sub matrixes with different height() * width()"};

        for (std::size_t i = 0; i < base::height(); i++)
            for (std::size_t j = 0; j < base::width(); j++)
                base::to(i, j) -= rhs.to(i, j);

        return *this;
    }

    MatrixArithmetic operator-() const
    {
        MatrixArithmetic res (base::height(), base::width());

        for (std::size_t i = 0; i < base::height(); i++)
            for (std::size_t j = 0; j < base::width(); j++)
                res.to(i, j) = -base::to(i, j);

        return res;
    }

    MatrixArithmetic& operator*=(const T& rhs)
    {
        for (auto& elem: *this)
            elem *= rhs;
        return *this;
    }

    MatrixArithmetic& operator/=(const T& rhs)
    {
        for (auto& elem: *this)
            elem /= rhs;
        return *this;
    }
//--------------------------------=| Basic arithmetic end |=--------------------------------------------

//--------------------------------=| Specific static ctors start |=-------------------------------------
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
        for (size_type i = 0; i < sz && itr != end; ++itr, i++)
            result.to(i, i) = *itr;
        return result;
    }

    template<std::input_iterator it>
    static MatrixArithmetic diag(it begin, it end)
    {
        auto sz = 0;
        for (auto itr = begin; itr != end; ++itr, sz++) {;}
        return diag(sz, begin, end);
    }

    static MatrixArithmetic diag(size_type sz, value_type val = value_type{})
    {
        MatrixArithmetic result {square(sz)};
        for (size_type i = 0; i < sz; i++)
            result.to(i, i) = val;
        return result;
    }

    static MatrixArithmetic eye(size_type sz)
    {
        return diag(sz, value_type{1});
    }
//--------------------------------=| Specific static ctors end |=---------------------------------------
};

//--------------------------------=| Wrappers arounf methods start |=-----------------------------------
template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = Detail:: DefaultAbs<T>>
T determinant(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& mat)
{
    return mat.determinant();
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = Detail:: DefaultAbs<T>>
std::pair<bool, MatrixArithmetic<T, IsDivArithm, Cmp, Abs>> inverse_pair(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& mat)
{
    return mat.inverse_pair();
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = Detail:: DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> inverse(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& mat)
{
    return mat.inverse();
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = Detail:: DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> transpos(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& mat)
{
    return mat.transpos();
}
//--------------------------------=| Wrappers arounf methods end |=-------------------------------------

//--------------------------------=| Arrithmetical operators start |=-----------------------------------
template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = Detail:: DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> product(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& lhs, const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& rhs)
{
    if (lhs.is_scalar())
    {
        MatrixArithmetic res {rhs};
        const T& scalar = scalar_cast(lhs);
        for (auto elem: res)
            elem *= scalar;
        return res;
    }
    if (rhs.is_scalar())
    {
        MatrixArithmetic res {lhs};
        const T& scalar = scalar_cast(rhs);
        for (auto elem: res)
            elem *= scalar;
        return res;
    }
    if (lhs.width() != rhs.height())
        throw std::invalid_argument{"in product: lhs.width() != rhs.height()"};

    MatrixArithmetic<T, IsDivArithm, Cmp, Abs> res (lhs.height(), rhs.width());

    for (std::size_t i = 0; i < lhs.height(); i++)
        for (std::size_t j = 0; j < rhs.width(); j++)
            for (std::size_t k = 0; k < lhs.width(); k++)
                res[i][j] += lhs[i][k] * rhs[k][j];

    return res; 
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = Detail:: DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> power(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& mat, long long pow)
    {
        if (!mat.is_square())
            throw std::invalid_argument{"Try to make matrix in some power but this matrix is not square"};

        if (pow == 0)
            return MatrixArithmetic<T, IsDivArithm, Cmp, Abs>::eye(mat.height());

        MatrixArithmetic<T, IsDivArithm, Cmp, Abs> res {mat};

        if (pow < 0)
        {
            res = res.inverse();
            pow = -pow;
        }
        
        for (long long i = 1; i < pow; i++)
            res = product(mat, res);
        
        return res;
    }

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = Detail:: DefaultAbs<T>>
bool operator==(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& lhs, const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& rhs)
{
    return lhs.equal_to(rhs);
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = Detail:: DefaultAbs<T>>
bool operator!=(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& lhs, const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& rhs)
{
    return !lhs.equal_to(rhs);
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = Detail:: DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> operator+(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& lhs, const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& rhs)
{
    MatrixArithmetic<T, IsDivArithm, Cmp, Abs> lhs_cpy {lhs};
    return (lhs_cpy += rhs);
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = Detail:: DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> operator-(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& lhs, const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& rhs)
{
    MatrixArithmetic<T, IsDivArithm, Cmp, Abs> lhs_cpy {lhs};
    return (lhs_cpy -= rhs);
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = Detail:: DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> operator*(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& lhs, const T& rhs)
{
    MatrixArithmetic<T, IsDivArithm, Cmp, Abs> lhs_cpy {lhs};
    return (lhs_cpy *= rhs);
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = Detail:: DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> operator*(const T& lhs, const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& rhs)
{
    return rhs * lhs;
}

template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = Detail:: DefaultAbs<T>>
MatrixArithmetic<T, IsDivArithm, Cmp, Abs> operator/(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& lhs, const T& rhs)
{
    MatrixArithmetic<T, IsDivArithm, Cmp, Abs> lhs_cpy {lhs};
    return (lhs_cpy /= rhs);
}
//--------------------------------=| Arrithmetical operators end |=-------------------------------------

//--------------------------------=| Cast to scalar start |=--------------------------------------------
template<typename T = int, bool IsDivArithm = false, class Cmp = std::equal_to<T>, class Abs = Detail:: DefaultAbs<T>>
T scalar_cast(const MatrixArithmetic<T, IsDivArithm, Cmp, Abs>& mat)
{
    if (!mat.is_scalar())
        throw std::invalid_argument{"Try to cast MatrixArithmetic in value_type, but matrix isnt scalar"};
    return mat.to(0, 0);
}
//--------------------------------=| Cast to scalar end |=----------------------------------------------
} //Matrix