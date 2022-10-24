#pragma once
#include <initializer_list>
#include <iostream>
#include <ostream>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace matrix{

template<typename T = double>
class matrix_t
{
    std::size_t height = 0, width = 0;
    T*  data = nullptr;
    T** rows = nullptr;

    void init_rows()
    {
        for (std::size_t i = 0; i < height; i++)
            rows[i] = data + i * width;
    }

    public:
    matrix_t(std::size_t h, std::size_t w, T val = T{})
    :height {h}, width {w}, data {new T[height * width]}, rows {new T*[height]}
    {
        for (auto i = 0; i < height * width; i++)
            data[i] = val;

        init_rows();
    }

    template<std::input_iterator it>
    matrix_t(std::size_t h, std::size_t w, it begin, it end)
    :height {h}, width {w}, data {new T[height * width]}, rows {new T*[height]}
    {
        if (height < 0 || width < 0)
            throw std::length_error{"In constructor of matrix: height or width less null"};
        
        auto i = 0;
        for (auto itr = begin; i < height * width && itr != end; i++, ++itr)
            data[i] = *itr;
        for (; i < height * width; i++)
            data[i] = T{};

        init_rows();    
    }

    matrix_t(T val = T{})
    :height {1}, width {1}, data {new T[1]{val}}, rows {new T*[1]{data}} {}

    matrix_t(std::initializer_list<T> onedim_list)
    :height {onedim_list.size()}, width {1}, data {new T[height * width]}, rows {new T*[height]}
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
        data = new T[height * width];
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
    :height {rhs.height}, width {rhs.width}, data {new T[height * width]}, rows {new T*[height]}
    {
        std::copy(rhs.data, rhs.data + height * width, data);
        std::copy(rhs.rows, rhs.rows + height, rows);
    }

    matrix_t(matrix_t&& rhs)
    :height {rhs.height}, width {rhs.width}, data {rhs.data}, rows {rhs.rows}
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
        data = new T[height * width];
        rows = new T*[height];

        std::copy(rhs.data, rhs.data + height * width, data);
        std::copy(rhs.rows, rhs.rows + height, rows);
        return *this;
    }

    matrix_t& operator=(matrix_t&& rhs)
    {
        if (this == &rhs)
            return *this;

        std::swap(height, rhs.height);
        std::swap(width, rhs.width);
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

    private:
    bool dbl_cmp(double d1, double d2) const
    {
        return std::abs(d1 - d2) <= 0.5*(std::abs(d1) + std::abs(d2))*1e-10;
    }

    public:
    bool operator==(const matrix_t& rhs) const
    {
        if (height != rhs.height || width != rhs.width)
            return false;
        for (auto i = 0; i < height; i++)
            for (auto j = 0; j < width; j++)
                if (!dbl_cmp(static_cast<double>(rows[i][j]), static_cast<double>(rhs.rows[i][j])))
                    return false;
        return true;
    }

    bool operator!=(const matrix_t& rhs) const
    {
        return !(*this == rhs);
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
            if (std::abs(static_cast<double>(to(i, iteration))) > std::abs(static_cast<double>(to(res, iteration))))
                res = i;
        return res;
    }
    bool dbl_cmp_zero(double d1) const
    {
        return std::abs(d1) < 1e-6;
    }

    using sign_t = double;
    sign_t make_upper_triangular_square(std::size_t side_of_square)
    {
        if (side_of_square > std::min(height, width))
            throw std::invalid_argument{"try to make upper triangular square that no inside matrix"};
    
        sign_t sign = 1.0;
        for (std::size_t i = 0; i < side_of_square - 1; i++)
        {
            auto row_to_swap = row_with_max_fst(i);
            if (row_to_swap != i)
            {
                swap_row(i, row_with_max_fst(i));
                sign *= -1;
            }
            if (!dbl_cmp_zero(rows[i][i]))
                for (std::size_t j = i + 1; j < side_of_square; j++)
                {
                    double coef = rows[j][i] / rows[i][i];
                    for (std::size_t k = i; k < width; k++)
                        rows[j][k] -= coef * rows[i][k];
                }
        }
        return sign;
    }

    void make_eye_square_from_upper_triangular_square(std::size_t side_of_square)
    {
        for (std::size_t i = side_of_square - 1; static_cast<long long>(i) >= 0; i--)
        {
            double coef = rows[i][i];
            for (std::size_t j = i; j < width; j++)
                rows[i][j] = rows[i][j] / coef;
        }
        
        for (std::size_t i = side_of_square - 1; static_cast<long long>(i) >= 0; i--)
            for (std::size_t j = 0; j < i; j++)
            {
                double coef = rows[j][i];
                for(std::size_t k = i; k < width; k++)
                    rows[j][k] -= rows[i][k] * coef;
            }
    }

    std::size_t rang_for_upper_triangular() const
    {
        std::size_t rang_mat = 0;
        std::size_t square_sz  = std::min(height, width);
        for (std::size_t i = 0; i < square_sz; i++)
            if (!dbl_cmp_zero(static_cast<double>(rows[i][i])))
                rang_mat++;
        return rang_mat;      
    }

    std::szie_t factorial(std::size_t num) const
    {
        std::size_t res = 0;
        for (std::size_t i = 1; i <= num; i++)
            res *= i;
        return res;
    }

    T recursive_det(T** mat_arr, std::size_t sz)
    {
        T res {};
        
    }

    T iterartive_det (matrix_t mat)
    {
        for (std::size_t ind = 0, end = factorial(height); ind < end; ind++)
        {

        }
    }

/*int determinant( int matrix[10][10], int n)
{
    int det = 0;
    int submatrix[10][10];
    if (n == 2)
        return ((matrix[0][0] * matrix[1][1]) - (matrix[1][0] * matrix[0][1]));
    else
    {
        for (int x = 0; x < n; x++)
        {
            int subi = 0;
            for (int i = 1; i < n; i++)
            {
                int subj = 0;
                for (int j = 0; j < n; j++)
                {
                    if (j == x)
                        continue;
                    submatrix[subi][subj] = matrix[i][j];
                    subj++;
                }
                subi++;
            }
            det += (pow(-1, x) * matrix[0][x] * determinant(submatrix, n - 1));
        }
    }
    return det;
}*/

    public:
    std::size_t rang() const
    {
        make_upper_triangular_square(std::min(height, width));
        return rang_for_upper_triangular();
    }

    T det() const requires std::is_floating_point_v<T>
    {
        if (height != width)
            throw std::invalid_argument{"try to get det() of no square matrix"};

        matrix_t cpy {*this};
        sign_t sign = cpy.make_upper_triangular_square(height);
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
        return T{1};
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

        std::cout << res << std::endl;
        
        return {true, res};
    }

    matrix_t inverse() const
    {
        auto res_pair = inverse_pair();
        if (!res_pair.first)
            throw std::invalid_argument{"try to get inverse matrix for matrix with det equal to zero"};

        return res_pair.second;
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const matrix_t<T>& mat)
{
    return mat.dump(out);
}

} //namespace
