#pragma once
#include <initializer_list>
#include <iostream>
#include <ostream>
#include <iterator>
#include <stdexcept>

namespace matrix{

template<typename T>
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

    static matrix_t quad(std::size_t sz, T val = T{})
    {
        return matrix_t(sz, sz, val);
    }

    template<std::input_iterator it>
    static matrix_t quad(std::size_t sz, it begin, it end)
    {
        return matrix_t(sz, sz, begin, end);
    }

    template<std::input_iterator it>
    static matrix_t diag(std::size_t sz, it begin, it end)
    {
        matrix_t result {quad(sz)};
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
        matrix_t result {quad(sz)};
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

    bool operator==(const matrix_t& rhs) const
    {
        if (height != rhs.height || width != rhs.width)
            return false;
        for (auto i = 0; i < height; i++)
            for (auto j = 0; j < width; j++)
                if (rows[i][j] != rhs.rows[i][j])
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
        proxy_row(matrix_t mat, std::size_t row_ind)
        :data {mat.rows[row_ind]}, sz {rhs.width} {}

        T&       operator[](std::size_t ind)       & noexcept {return data[ind];}
        const T& operator[](std::size_t ind) const & noexcept {return data[ind];}
        T        operator[](std::size_t ind)      && noexcept {return data[ind];}

        T& at(std::size_t ind) &
        {
            if (ind >= width)
                throw std::out_of_range{"ind in row more than width of matrix"};
            return data[ind];
        }

        const T& at(std::size_t ind) const &
        {
            if (ind >= width)
                throw std::out_of_range{"ind in row more than width of matrix"};
            return data[ind];
        }

        T at(std::size_t ind) &&
        {
            if (ind >= width)
                throw std::out_of_range{"ind in row more than width of matrix"};
            return data[ind];
        }
    };

    public:
    proxy_row operator[](std::size_t ind) noexcept {return proxy_row{rows[ind], width};}
    const proxy_row operator[](std::size_t ind) const noexcept {return proxy_row{rows[ind], width}};

    proxy_row at(std::size_t ind)
    {
        if (ind >= height)
            throw std::out_of_range{"ind in matrix more than height of matrix"};
        return proxy_row{rows[ind], width};
    }

    const proxy_row at(std::size_t ind)
    {
        if (ind >= height)
            throw std::out_of_range{"ind in matrix more than height of matrix"};
        return proxy_row{rows[ind], width};
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const matrix_t<T>& mat)
{
    return mat.dump(out);
}

} //namespace
