#pragma once
#include <initializer_list>
#include <iostream>
#include <ostream>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace matrix{

template<typename T = int, class Cmp = std::equal_to<T>>
class matrix_t
{
    std::size_t height = 0, width = 0, size = 0;
    T*  data = nullptr;
    T** rows = nullptr;
    Cmp cmp;

    void init_rows()
    {
        for (std::size_t i = 0; i < height; i++)
            rows[i] = data + i * width;
    }

public:
    matrix_t(std::size_t h, std::size_t w, T val = T{})
    :height {h}, width {w}, size {h * w}, data {new T[size]}, rows {new T*[height]}
    {
        for (auto i = 0; i < size; i++)
            data[i] = val;

        init_rows();
    }

    template<std::input_iterator it>
    matrix_t(std::size_t h, std::size_t w, it begin, it end)
    :height {h}, width {w}, size {height * width}, data {new T[size]}, rows {new T*[height]}
    {
        if (height < 0 || width < 0)
            throw std::length_error{"In constructor of matrix: height or width less null"};
        
        auto i = 0;
        for (auto itr = begin; i < size && itr != end; i++, ++itr)
            data[i] = *itr;
        for (; i < size; i++)
            data[i] = T{};

        init_rows();    
    }

    matrix_t(T val = T{})
    :height {1}, width {1}, size{1}, data {new T[1]{val}}, rows {new T*[1]{data}} {}

    matrix_t(std::initializer_list<T> onedim_list)
    :height {onedim_list.size()}, width {1}, size {height}, data {new T[size]}, rows {new T*[height]}
    {
        std::copy(onedim_list.begin(), onedim_list.end(), data);
        init_rows();
    }

    matrix_t(std::initializer_list<std::initializer_list<T>> twodim_list)
    :height {twodim_list.size()}
    {
        if (height == 0)
            throw std::logic_error{"Never know why - Ozzy Osbourne"};
        
        std::size_t max_width = 0;
        for (auto row: twodim_list)
            if (row.size() > max_width)
                max_width = row.size();

        width = max_width;
        size = height * width;
        data = new T[size];
        rows = new T*[height];
        auto act_row = 0;
        for (auto row: twodim_list)
        {
            std::copy(row.begin(), row.end(), data + act_row * width);
            if (row.size() < width)
                for (auto i = row.size(); i < width; i++)
                    data[act_row * width + i] = T{};
            act_row++;
        }

        init_rows();
    }

    matrix_t(const matrix_t& rhs)
    :height {rhs.height}, width {rhs.width}, size {rhs.size}, data {new T[size]}, rows {new T*[height]}
    {
        std::copy(rhs.data, rhs.data + size, data);
        std::copy(rhs.rows, rhs.rows + height, rows);
    }

    matrix_t(matrix_t&& rhs)
    :height {rhs.height}, width {rhs.width}, size {rhs.size}, data {rhs.data}, rows {rhs.rows}
    {
        rhs.height = 0;
        rhs.width  = 0;
        rhs.data = nullptr;
        rhs.rows = nullptr;
    }

    matrix_t& operator=(const matrix_t& rhs)
    {
        height = rhs.height;
        width  = rhs.width;
        size = rhs.size;
        data = new T[size];
        rows = new T*[height];

        std::copy(rhs.data, rhs.data + size, data);
        std::copy(rhs.rows, rhs.rows + height, rows);
        return *this;
    }

    matrix_t& operator=(matrix_t&& rhs)
    {
        if (this == &rhs)
            return *this;

        std::swap(height, rhs.height);
        std::swap(width, rhs.width);
        std::swap(size, rhs.size);
        std::swap(data, rhs.data);
        std::swap(rows, rhs.rows);

        return *this;
    }

    ~matrix_t()
    {
        delete[] data;
        delete[] rows;   
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
            result.data[i * sz + i] = *itr;
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
            result.data[i * sz + i] = val;
        return result;
    }

    T& to(std::size_t row_ind, std::size_t col_ind) & noexcept
    {
        return rows[row_ind][col_ind];
    }

    const T& to(std::size_t row_ind, std::size_t col_ind) const & noexcept
    {
        return rows[row_ind][col_ind];
    }

    T to(std::size_t row_ind, std::size_t col_ind) const && noexcept
    {
        return rows[row_ind][col_ind];
    }

    void swap_row(std::size_t ind1, std::size_t ind2) &
    {
        std::swap(rows[ind1], rows[ind2]);
    }

    std::ostream& dump(std::ostream& out) const 
    {
        out << '{';
        for (auto i = 0; i < height; i++)
        {
            out << '{';
            for (auto j = 0; j < width - 1; j++)
                out << rows[i][j] << ' ';
            out << rows[i][width - 1] << '}';
            
        }
        out << '}';
        return out;
    }
private:
    class proxy_row {
        T* data = nullptr;
        std::size_t sz = 0;

    public:
        proxy_row(const matrix_t& mat, std::size_t row_ind)
        :data {mat.rows[row_ind]}, sz {mat.width} {}

        T&       operator[](std::size_t ind)       & noexcept {return data[ind];}
        const T& operator[](std::size_t ind) const & noexcept {return data[ind];}
        T        operator[](std::size_t ind)      && noexcept {return data[ind];}

        T& at(std::size_t ind) &
        {
            if (ind >= sz)
                throw std::out_of_range{"ind in row more than width of matrix"};
            return data[ind];
        }

        const T& at(std::size_t ind) const &
        {
            if (ind >= sz)
                throw std::out_of_range{"ind in row more than width of matrix"};
            return data[ind];
        }

        T at(std::size_t ind) &&
        {
            if (ind >= sz)
                throw std::out_of_range{"ind in row more than width of matrix"};
            return data[ind];
        }
    };

public:
    proxy_row operator[](std::size_t ind) noexcept {return proxy_row{*this, ind};}
    const proxy_row operator[](std::size_t ind) const noexcept {return proxy_row{*this, ind};}

    proxy_row at(std::size_t ind)
    {
        if (ind >= height)
            throw std::out_of_range{"ind in matrix more than height of matrix"};
        return proxy_row{*this, ind};
    }

    const proxy_row at(std::size_t ind) const
    {
        if (ind >= height)
            throw std::out_of_range{"ind in matrix more than height of matrix"};
        return proxy_row{*this, ind};
    }

private:
    std::size_t row_with_max_fst(std::size_t iteration)
    {
        std::size_t res = iteration;
        for (std::size_t i = iteration; i < height; i++)
            if (std::abs(to(i, iteration)) > std::abs(to(res, iteration)))
                res = i;
        return res;
    }

//------------------------------- FOR FLOATING POINT TYPES ---------------------------------------------
    T make_upper_triangular_square(std::size_t side_of_square) requires std::is_floating_point_v<T>
    {
        if (side_of_square > std::min(height, width))
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
            if (!cmp(rows[i][i], T{}))
                for (std::size_t j = i + 1; j < side_of_square; j++)
                {
                    double coef = rows[j][i] / rows[i][i];
                    for (std::size_t k = i; k < width; k++)
                        rows[j][k] -= coef * rows[i][k];
                }
        }
        return sign;
    }

//------------------------------------------------------------------------------------------------------

//------------------------------- FOR OTHER TYPES ------------------------------------------------------
    T make_upper_triangular_square(std::size_t side_of_square) 
    {
        if (side_of_square > std::min(height, width))
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
            if (!cmp(rows[i][i], T{}))
            {
                for (std::size_t j = i + 1; j < side_of_square; j++)
                {
                    for (std::size_t k = i + 1; k < side_of_square; k++)
                        rows[j][k] = (rows[j][k] * rows[i][i] - rows[j][i] * rows[i][k]) / div_coef;
                }
                div_coef = rows[i][i];
            }
            else
                sign = T{};
        }
        return rows[side_of_square - 1][side_of_square - 1] * sign;
    }
//------------------------------------------------------------------------------------------------------

    void make_eye_square_from_upper_triangular_square(std::size_t side_of_square)
    {
        for (std::size_t i = side_of_square - 1; static_cast<long long>(i) >= 0; i--)
        {
            auto coef = rows[i][i];
            for (std::size_t j = i; j < width; j++)
                rows[i][j] = rows[i][j] / coef;
        }
        
        for (std::size_t i = side_of_square - 1; static_cast<long long>(i) >= 0; i--)
            for (std::size_t j = 0; j < i; j++)
            {
                auto coef = rows[j][i];
                for(std::size_t k = i; k < width; k++)
                    rows[j][k] -= rows[i][k] * coef;
            }
    }

    std::size_t rang_for_upper_triangular() const
    {
        std::size_t rang_mat = 0;
        std::size_t square_sz  = std::min(height, width);
        for (std::size_t i = 0; i < square_sz; i++)
            if (!cmp(rows[i][i], T{}))
                rang_mat++;
        return rang_mat;      
    }

public:
    std::size_t rang() const
    {
        matrix_t cpy {*this};
        cpy.make_upper_triangular_square(std::min(height, width));
        return cpy.rang_for_upper_triangular();
    }

    T det() const requires std::is_floating_point_v<T>
    {
        if (height != width)
            throw std::invalid_argument{"try to get det() of no square matrix"};

        matrix_t cpy {*this};
        T sign = cpy.make_upper_triangular_square(height);
        T res {1};
        for (std::size_t i = 0; i < height; i++)
            res *= cpy.rows[i][i];
        return sign * res;
    }

    T det() const
    {
        if (height != width)
            throw std::invalid_argument{"try to get det() of no square matrix"};

        matrix_t cpy {*this};
        return cpy.make_upper_triangular_square(height);
    }

    std::pair<bool, matrix_t> inverse_pair() const
    {
        if (height != width)
            throw std::invalid_argument{"try to get inverse matrix of no square matrix"};

        matrix_t extended_mat (height, 2*height);
        for (std::size_t i = 0; i < height; i++)
            for (std::size_t j = 0; j < height; j++)
                extended_mat.rows[i][j] = rows[i][j];
        
        for (std::size_t i = 0; i < height; i++)
            extended_mat.rows[i][i + height] = T{1};

        extended_mat.make_upper_triangular_square(extended_mat.height);

        if (extended_mat.rang_for_upper_triangular() != height)
            return {false, matrix_t{*this}};

        extended_mat.make_eye_square_from_upper_triangular_square(extended_mat.height);
        
        matrix_t res (height, height);
        for (std::size_t i = 0; i < height; i++)
            for (std::size_t j = 0; j < height; j++)
                res.rows[i][j] = extended_mat.rows[i][j + height];
        
        return {true, res};
    }

    matrix_t inverse() const
    {
        auto res_pair = inverse_pair();
        if (!res_pair.first)
            throw std::invalid_argument{"try to get inverse matrix for matrix with det equal to zero"};

        return res_pair.second;
    }

    const T* begin() const & {return data;}
    T* begin() & {return data;}
    const T* cbegin() const & {return data;}

    const T* end() const & {return data + size;}
    T* end() & {return data + size;}
    const T* cend() const & {return data + size;}

    bool equal_to(const matrix_t& rhs) const
    {
        if (height != rhs.height || width != rhs.width)
            return false;

        if (data == rhs.data)
            return true;

        for (std::size_t i = 0; i < size; i++)
            if (!cmp(data[i], rhs.data[i]))
                return false;
        
        return true;
    }

    matrix_t& operator+=(const matrix_t& rhs)
    {
        if (height != rhs.height || width != rhs.width)
            throw std::invalid_argument("Try to add matrixes with different size");

        for (std::size_t i = 0; i < size; i++)
            data[i] += rhs.data[i];
        return *this;
    }

    matrix_t& operator-=(const matrix_t& rhs)
    {
        if (height != rhs.height || width != rhs.width)
            throw std::invalid_argument("Try to sub matrixes with different size");

        for (std::size_t i = 0; i < size; i++)
            data[i] -= rhs.data[i];
        return *this;
    }

    matrix_t operator-() const
    {
        return matrix_t (height, width, cbegin(), cend());
    }

    matrix_t& operator*=(const T& rhs)
    {
        for (std::size_t i = 0; i < size; i++)
            data[i] *= rhs;
        return *this;
    }

    matrix_t& operator/=(const T& rhs)
    {
        for (std::size_t i = 0; i < size; i++)
            data[i] /= rhs;
        return *this;
    }
};

template<typename T = int, class Cmp = std::equal_to<T>>
bool operator==(const matrix_t<T, Cmp>& lhs, const matrix_t<T, Cmp>& rhs)
{
    return lhs.equal_to(rhs);
}

template<typename T = int, class Cmp = std::equal_to<T>>
bool operator!=(const matrix_t<T, Cmp>& lhs, const matrix_t<T, Cmp>& rhs)
{
    return !lhs.equal_to(rhs);
}

template<typename T = int, class Cmp = std::equal_to<T>>
matrix_t<T, Cmp> operator+(const matrix_t<T, Cmp>& lhs, const matrix_t<T, Cmp>& rhs)
{
    matrix_t<T, Cmp> lhs_cpy {lhs};
    return (lhs_cpy += rhs);
}

template<typename T = int, class Cmp = std::equal_to<T>>
matrix_t<T, Cmp> operator-(const matrix_t<T, Cmp>& lhs, const matrix_t<T, Cmp>& rhs)
{
    matrix_t<T, Cmp> lhs_cpy {lhs};
    return (lhs_cpy -= rhs);
}

template<typename T = int, class Cmp = std::equal_to<T>>
matrix_t<T, Cmp> operator*(const matrix_t<T, Cmp>& lhs, const T& rhs)
{
    matrix_t<T, Cmp> lhs_cpy {lhs};
    return (lhs_cpy *= rhs);
}

template<typename T = int, class Cmp = std::equal_to<T>>
matrix_t<T, Cmp> operator*(const T& lhs, const matrix_t<T, Cmp>& rhs)
{
    matrix_t<T, Cmp> rhs_cpy {rhs};
    return (rhs_cpy *= lhs);
}

template<typename T = int, class Cmp = std::equal_to<T>>
matrix_t<T, Cmp> operator/(const matrix_t<T, Cmp>& lhs, const T& rhs)
{
    matrix_t<T, Cmp> lhs_cpy {lhs};
    return (lhs_cpy /= rhs);
}

template<typename T = int, class Cmp = std::equal_to<T>>
std::ostream& operator<<(std::ostream& out, const matrix_t<T, Cmp>& mat)
{
    return mat.dump(out);
}

} //namespace
