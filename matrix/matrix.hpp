#pragma once
#include <initializer_list>
#include <iostream>
#include <ostream>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace matrix{

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
/*
 * ArDiv: eg=qual to true if division for your type defined correctly, else false
 * Correctly: (b != null_obj && a / b == c) <=> (bc == a)
 */
class matrix_t
{
    std::size_t height_ = 0, width_ = 0;
    T*  data_ = nullptr;
    T** rows_ = nullptr;
    Cmp cmp;
    static const bool ar_div = ArDiv;

    void init_rows()
    {
        for (std::size_t i = 0; i < height_; i++)
            rows_[i] = data_ + i * width_;
    }

public:
    matrix_t(std::size_t h, std::size_t w, T val = T{})
    :height_ {h}, width_ {w}, data_ {new T[height_ * width_]}, rows_ {new T*[height_]}
    {
        for (auto i = 0; i < height_ * width_; i++)
            data_[i] = val;

        init_rows();
    }

    template<std::input_iterator it>
    matrix_t(std::size_t h, std::size_t w, it begin, it end)
    :height_ {h}, width_ {w}, data_ {new T[height_ * width_]}, rows_ {new T*[height_]}
    {
        if (height_ < 0 || width_ < 0)
            throw std::length_error{"In constructor of matrix: height_ or width_ less null"};
        
        auto i = 0;
        for (auto itr = begin; i < height_ * width_ && itr != end; i++, ++itr)
            data_[i] = *itr;
        for (; i < height_ * width_; i++)
            data_[i] = T{};

        init_rows();    
    }

    matrix_t(T val = T{})
    :height_ {1}, width_ {1}, data_ {new T[1]{val}}, rows_ {new T*[1]{data_}} {}

    matrix_t(std::initializer_list<T> onedim_list)
    :height_ {onedim_list.size()}, width_ {1}, data_ {new T[height_ * width_]}, rows_ {new T*[height_]}
    {
        std::copy(onedim_list.begin(), onedim_list.end(), data_);
        init_rows();
    }

    matrix_t(std::initializer_list<std::initializer_list<T>> twodim_list)
    :height_ {twodim_list.size()}
    {
        if (height_ == 0)
            throw std::logic_error{"Never know why - Ozzy Osbourne"};
        
        std::size_t max_width_ = 0;
        for (auto row: twodim_list)
            if (row.size() > max_width_)
                max_width_ = row.size();

        width_ = max_width_;
        data_ = new T[height_ * width_];
        rows_ = new T*[height_];
        std::size_t act_row = 0;
        for (auto row: twodim_list)
        {
            std::copy(row.begin(), row.end(), data_ + act_row * width_);
            if (row.size() < width_)
                for (auto i = row.size(); i < width_; i++)
                    data_[act_row * width_ + i] = T{};
            act_row++;
        }

        init_rows();
    }

    matrix_t(const matrix_t& rhs)
    :height_ {rhs.height_}, width_ {rhs.width_}, data_ {new T[height_ * width_]}, rows_ {new T*[height_]}
    {
        std::copy(rhs.data_, rhs.data_ + height_ * width_, data_);
        init_rows();
    }

    matrix_t(matrix_t&& rhs)
    :height_ {rhs.height_}, width_ {rhs.width_}, data_ {rhs.data_}, rows_ {rhs.rows_}
    {
        rhs.height_ = 0;
        rhs.width_  = 0;
        rhs.data_ = nullptr;
        rhs.rows_ = nullptr;
    }

    matrix_t& operator=(const matrix_t& rhs)
    {
        height_ = rhs.height_;
        width_  = rhs.width_;
        data_ = new T[height_ * width_];
        rows_ = new T*[height_];

        std::copy(rhs.data_, rhs.data_ + height_ * width_, data_);
        init_rows();
        return *this;
    }

    matrix_t& operator=(matrix_t&& rhs)
    {
        if (this == &rhs)
            return *this;

        std::swap(height_, rhs.height_);
        std::swap(width_, rhs.width_);
        std::swap(data_, rhs.data_);
        std::swap(rows_, rhs.rows_);

        return *this;
    }

    ~matrix_t()
    {
        delete[] data_;
        delete[] rows_;   
    }

    static matrix_t square(std::size_t sz, T val = T{})
    {
        return matrix_t(sz, sz, val);
    }

    template<std::input_iterator it>
    static matrix_t square(std::size_t sz, it begin, it end)
    {
        return matrix_t(sz, sz, begin, end);
    }

    template<std::input_iterator it>
    static matrix_t diag(std::size_t sz, it begin, it end)
    {
        matrix_t result {square(sz)};
        auto itr = begin;
        for (auto i = 0; i < sz && itr != end; ++itr, i++)
            result.data_[i * sz + i] = *itr;
        return result;
    }

    template<std::input_iterator it>
    static matrix_t diag(it begin, it end)
    {
        auto sz = 0;
        for (auto itr = begin; itr != end; ++itr, sz++) {;}
        return diag(sz, begin, end);
    }

    static matrix_t diag(std::size_t sz, T val = T{})
    {
        matrix_t result {square(sz)};
        for (auto i = 0; i < sz; i++)
            result.data_[i * sz + i] = val;
        return result;
    }

    std::size_t height() const {return height_;}
    std::size_t width() const {return width_;}

    T& to(std::size_t row_ind, std::size_t col_ind) & noexcept
    {
        return rows_[row_ind][col_ind];
    }

    const T& to(std::size_t row_ind, std::size_t col_ind) const & noexcept
    {
        return rows_[row_ind][col_ind];
    }

    T to(std::size_t row_ind, std::size_t col_ind) const && noexcept
    {
        return rows_[row_ind][col_ind];
    }

    void swap_row(std::size_t ind1, std::size_t ind2) &
    {
        std::swap(rows_[ind1], rows_[ind2]);
    }

    std::ostream& dump(std::ostream& out) const 
    {
        out << '{';
        for (auto i = 0; i < height_; i++)
        {
            out << '{';
            for (auto j = 0; j < width_ - 1; j++)
                out << rows_[i][j] << ' ';
            out << rows_[i][width_ - 1] << '}';
        }
        out << '}';
        return out;
    }
private:
    class proxy_row {
        T* data_ = nullptr;
        std::size_t sz = 0;

    public:
        proxy_row(const matrix_t& mat, std::size_t row_ind)
        :data_ {mat.rows_[row_ind]}, sz {mat.width_} {}

        T&       operator[](std::size_t ind)       & noexcept {return data_[ind];}
        const T& operator[](std::size_t ind) const & noexcept {return data_[ind];}
        T        operator[](std::size_t ind)      && noexcept {return data_[ind];}

        T& at(std::size_t ind) &
        {
            if (ind >= sz)
                throw std::out_of_range{"ind in row more than width_ of matrix"};
            return data_[ind];
        }

        const T& at(std::size_t ind) const &
        {
            if (ind >= sz)
                throw std::out_of_range{"ind in row more than width_ of matrix"};
            return data_[ind];
        }

        T at(std::size_t ind) &&
        {
            if (ind >= sz)
                throw std::out_of_range{"ind in row more than width_ of matrix"};
            return data_[ind];
        }
    };

public:
    proxy_row operator[](std::size_t ind) noexcept {return proxy_row{*this, ind};}
    const proxy_row operator[](std::size_t ind) const noexcept {return proxy_row{*this, ind};}

    proxy_row at(std::size_t ind)
    {
        if (ind >= height_)
            throw std::out_of_range{"ind in matrix more than height_ of matrix"};
        return proxy_row{*this, ind};
    }

    const proxy_row at(std::size_t ind) const
    {
        if (ind >= height_)
            throw std::out_of_range{"ind in matrix more than height_ of matrix"};
        return proxy_row{*this, ind};
    }

private:
    std::size_t row_with_max_fst(std::size_t iteration)
    {
        std::size_t res = iteration;
        for (std::size_t i = iteration; i < height_; i++)
            if (std::abs(to(i, iteration)) > std::abs(to(res, iteration)))
                res = i;
        return res;
    }

//------------------------------- FOR FLOATING POINT TYPES ---------------------------------------------
    T make_upper_triangular_square(std::size_t side_of_square) requires (ar_div == true)
    {
        if (side_of_square > std::min(height_, width_))
            throw std::invalid_argument{"try to make upper triangular square that no inside matrix"};
    
        T sign {1};
        for (std::size_t i = 0; i < side_of_square - 1; i++)
        {
            auto row_to_swap = row_with_max_fst(i);
            if (row_to_swap != i)
            {
                swap_row(i, row_with_max_fst(i));
                sign *= T{-1};
            }
            if (!cmp(rows_[i][i], T{}))
                for (std::size_t j = i + 1; j < side_of_square; j++)
                {
                    double coef = rows_[j][i] / rows_[i][i];
                    for (std::size_t k = i; k < width_; k++)
                        rows_[j][k] -= coef * rows_[i][k];
                }
        }
        return sign;
    }

//------------------------------------------------------------------------------------------------------

//------------------------------- FOR OTHER TYPES ------------------------------------------------------
    T make_upper_triangular_square(std::size_t side_of_square) 
    {
        if (side_of_square > std::min(height_, width_))
            throw std::invalid_argument{"try to make upper triangular square that no inside matrix"};

        T div_coef {1};
        T sign {1};
        for (std::size_t i = 0; i < side_of_square - 1; i++)
        {
            auto row_to_swap = row_with_max_fst(i);
            if (row_to_swap != i)
            {
                swap_row(i, row_with_max_fst(i));
                sign *= T{-1};
            }
            if (!cmp(rows_[i][i], T{}))
            {
                for (std::size_t j = i + 1; j < side_of_square; j++)
                {
                    for (std::size_t k = i + 1; k < side_of_square; k++)
                        rows_[j][k] = (rows_[j][k] * rows_[i][i] - rows_[j][i] * rows_[i][k]) / div_coef;
                }
                div_coef = rows_[i][i];
            }
            else
                sign = T{};
        }
        return rows_[side_of_square - 1][side_of_square - 1] * sign;
    }
//------------------------------------------------------------------------------------------------------

    void make_eye_square_from_upper_triangular_square(std::size_t side_of_square)
    {
        for (std::size_t i = side_of_square - 1; static_cast<long long>(i) >= 0; i--)
        {
            auto coef = rows_[i][i];
            for (std::size_t j = i; j < width_; j++)
                rows_[i][j] = rows_[i][j] / coef;
        }
        
        for (std::size_t i = side_of_square - 1; static_cast<long long>(i) >= 0; i--)
            for (std::size_t j = 0; j < i; j++)
            {
                auto coef = rows_[j][i];
                for(std::size_t k = i; k < width_; k++)
                    rows_[j][k] -= rows_[i][k] * coef;
            }
    }

    std::size_t rang_for_upper_triangular() const
    {
        std::size_t rang_mat = 0;
        std::size_t square_sz  = std::min(height_, width_);
        for (std::size_t i = 0; i < square_sz; i++)
            if (!cmp(rows_[i][i], T{}))
                rang_mat++;
        return rang_mat;      
    }

public:
    std::size_t rang() const
    {
        matrix_t cpy {*this};
        cpy.make_upper_triangular_square(std::min(height_, width_));
        return cpy.rang_for_upper_triangular();
    }

    T det() const requires std::is_floating_point_v<T>
    {
        if (height_ != width_)
            throw std::invalid_argument{"try to get det() of no square matrix"};

        matrix_t cpy {*this};
        T sign = cpy.make_upper_triangular_square(height_);
        T res {1};
        for (std::size_t i = 0; i < height_; i++)
            res *= cpy.rows_[i][i];
        return sign * res;
    }

    T det() const
    {
        if (height_ != width_)
            throw std::invalid_argument{"try to get det() of no square matrix"};

        matrix_t cpy {*this};
        return cpy.make_upper_triangular_square(height_);
    }

    std::pair<bool, matrix_t> inverse_pair() const requires (ar_div == true)
    {
        if (height_ != width_)
            throw std::invalid_argument{"try to get inverse matrix of no square matrix"};

        matrix_t extended_mat (height_, 2*height_);
        for (std::size_t i = 0; i < height_; i++)
            for (std::size_t j = 0; j < height_; j++)
                extended_mat.rows_[i][j] = rows_[i][j];
        
        for (std::size_t i = 0; i < height_; i++)
            extended_mat.rows_[i][i + height_] = T{1};

        extended_mat.make_upper_triangular_square(extended_mat.height_);

        if (extended_mat.rang_for_upper_triangular() != height_)
            return {false, matrix_t{*this}};

        extended_mat.make_eye_square_from_upper_triangular_square(extended_mat.height_);
        
        matrix_t res (height_, height_);
        for (std::size_t i = 0; i < height_; i++)
            for (std::size_t j = 0; j < height_; j++)
                res.rows_[i][j] = extended_mat.rows_[i][j + height_];
        
        return {true, res};
    }

    matrix_t inverse() const requires (ar_div == true)
    {
        auto res_pair = inverse_pair();
        if (!res_pair.first)
            throw std::invalid_argument{"try to get inverse matrix for matrix with det equal to zero"};

        return res_pair.second;
    }

    bool equal_to(const matrix_t& rhs) const
    {
        if (height_ != rhs.height_ || width_ != rhs.width_)
            return false;

        if (data_ == rhs.data_)
            return true;

        for (std::size_t i = 0; i < height_; i++)
            for (std::size_t j = 0; j < width_; j++)
                if (!cmp(to(i, j), rhs.to(i, j)))
                    return false;
                    
        return true;
    }

    matrix_t& operator+=(const matrix_t& rhs)
    {
        if (height_ != rhs.height_ || width_ != rhs.width_)
            throw std::invalid_argument("Try to add matrixes with different height_ * width_");

        for (std::size_t i = 0; i < height_; i++)
            for (std::size_t j = 0; j < width_; j++)
                to(i, j) += rhs.to(i, j);

        return *this;
    }

    matrix_t& operator-=(const matrix_t& rhs)
    {
        if (height_ != rhs.height_ || width_ != rhs.width_)
            throw std::invalid_argument("Try to sub matrixes with different height_ * width_");

        for (std::size_t i = 0; i < height_; i++)
            for (std::size_t j = 0; j < width_; j++)
                to(i, j) -= rhs.to(i, j);

        return *this;
    }

    matrix_t operator-() const
    {
        matrix_t res (height_, width_);

        for (std::size_t i = 0; i < height_; i++)
            for (std::size_t j = 0; j < width_; j++)
                res.to(i, j) = -to(i, j);

        return res;
    }

    matrix_t& operator*=(const T& rhs)
    {
        for (std::size_t i = 0; i < height_ * width_; i++)
            data_[i] *= rhs;
        return *this;
    }

    matrix_t& operator/=(const T& rhs)
    {
        for (std::size_t i = 0; i < height_ * width_; i++)
            data_[i] /= rhs;
        return *this;
    }
};

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
matrix_t<T, ArDiv, Cmp> product(const matrix_t<T, ArDiv, Cmp>& lhs, const matrix_t<T, ArDiv, Cmp>& rhs)
{
    if (lhs.width_ != rhs.height_)
        throw std::invalid_argument{"in product: lhs.width_ != rhs.height_"};

    matrix_t<T, ArDiv, Cmp> res (lhs.height_, rhs.width_);

    for (std::size_t i = 0; i < lhs.height_; i++)
        for (std::size_t j = 0; j < rhs.width_; j++)
            for (std::size_t k = 0; k < lhs.width_; k++)
                res[i][j] += lhs[i][k] * rhs[k][j];

    return res; 
}

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
bool operator==(const matrix_t<T, ArDiv, Cmp>& lhs, const matrix_t<T, ArDiv, Cmp>& rhs)
{
    return lhs.equal_to(rhs);
}

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
bool operator!=(const matrix_t<T, ArDiv, Cmp>& lhs, const matrix_t<T, ArDiv, Cmp>& rhs)
{
    return !lhs.equal_to(rhs);
}

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
matrix_t<T, ArDiv, Cmp> operator+(const matrix_t<T, ArDiv, Cmp>& lhs, const matrix_t<T, ArDiv, Cmp>& rhs)
{
    matrix_t<T, ArDiv, Cmp> lhs_cpy {lhs};
    return (lhs_cpy += rhs);
}

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
matrix_t<T, ArDiv, Cmp> operator-(const matrix_t<T, ArDiv, Cmp>& lhs, const matrix_t<T, ArDiv, Cmp>& rhs)
{
    matrix_t<T, ArDiv, Cmp> lhs_cpy {lhs};
    return (lhs_cpy -= rhs);
}

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
matrix_t<T, ArDiv, Cmp> operator*(const matrix_t<T, ArDiv, Cmp>& lhs, const T& rhs)
{
    matrix_t<T, ArDiv, Cmp> lhs_cpy {lhs};
    return (lhs_cpy *= rhs);
}

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
matrix_t<T, ArDiv, Cmp> operator*(const T& lhs, const matrix_t<T, ArDiv, Cmp>& rhs)
{
    return rhs * lhs;
}

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
matrix_t<T, ArDiv, Cmp> operator/(const matrix_t<T, ArDiv, Cmp>& lhs, const T& rhs)
{
    matrix_t<T, ArDiv, Cmp> lhs_cpy {lhs};
    return (lhs_cpy /= rhs);
}

template<typename T = int, bool ArDiv = false, class Cmp = std::equal_to<T>>
std::ostream& operator<<(std::ostream& out, const matrix_t<T, ArDiv, Cmp>& mat)
{
    return mat.dump(out);
}

} //namespace
