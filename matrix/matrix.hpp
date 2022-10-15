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
    int height = 1, width = 1;
    T* data = new T[1]{T{}};

    int* row_order = new int[1]{0};
    int* col_order = new int[1]{0};

    void init_orders()
    {
        for (auto i = 0; i < height; i++)
            row_order[i] = i;
        for (auto i = 0; i < width; i++)
            col_order[i] = i;
    }

    public:
    matrix_t(int h, int w, T val = T{})
    :height {h}, width {w}, data {new T[height * width]},
     row_order {new int[height]}, col_order {new int[width]}
    {
        if (height < 0 || width < 0)
            throw std::length_error{"In constructor of matrix: height or width less null"};

        for (auto i = 0; i < height * width; i++)
            data[i] = val;
        init_orders();
    }

    template<std::input_iterator it>
    matrix_t(int h, int w, it begin, it end)
    :height {h}, width {w}, data {new T[height * width]},
     row_order {new int[height]}, col_order {new int[width]}
    {
        if (height < 0 || width < 0)
            throw std::length_error{"In constructor of matrix: height or width less null"};
        
        auto i = 0;
        for (auto itr = begin; i < height * width && itr != end; i++, ++itr)
            data[i] = *itr;
        for (; i < height * width; i++)
            data[i] = T{};
        init_orders();    
    }

    matrix_t(T val = T{})
    :height {1}, width {1}, data {new T[1]{val}},
     row_order {new int[1]{0}}, col_order {new int[1]{0}} {} 

    matrix_t(std::initializer_list<T> onedim_list)
    :height {static_cast<int>(onedim_list.size())}, width {1}, data {new T[height * width]},
     row_order {new int[height]}, col_order {new int[width]}
    {
        auto i = 0;
        std::copy(onedim_list.begin(), onedim_list.end(), data);
        init_orders();
    }

    matrix_t(std::initializer_list<std::initializer_list<T>> twodim_list)
    :height {static_cast<int>(twodim_list.size())}
    {
        if (height == 0)
            throw std::logic_error{"Never know why - Ozzy Osbourne"};
        
        std::size_t max_width = 0;
        for (auto row: twodim_list)
            if (row.size() > max_width)
                max_width = row.size();

        width = static_cast<int>(max_width);
        data = new T[height * width];
        auto act_row = 0;
        for (auto row: twodim_list)
        {
            std::copy(row.begin(), row.end(), data + act_row * width);
            if (static_cast<int>(row.size()) < width);
                for (auto i = static_cast<int>(row.size()); i < width; i++)
                    data[act_row * width + i] = T{};
            act_row++;
        }
        init_orders();
    }

    matrix_t(const matrix_t& rhs)
    :height {rhs.height}, width {rhs.width}, data {new T[height * width]},
     row_order {new int[height]}, col_order {new int[width]}
    {
        std::copy(rhs.data, rhs.data + height * width, data);
        std::copy(rhs.row_order, rhs.row_order + height, row_order);
        std::copy(rhs.col_order, rhs.col_order + width, col_order);
    }

    matrix_t(matrix_t&& rhs)
    :height {rhs.height}, width {rhs.width}, data {rhs.data},
     row_order (rhs.row_order), col_order {rhs.col_order}
    {}

    matrix_t& operator=(const matrix_t& rhs)
    {
        height = rhs.height;
        width  = rhs.width;
        data = new T[height * width];
        row_order = new int[height];
        col_order = new int[width];

        std::copy(rhs.data, rhs.data + height * width, data);
        std::copy(rhs.row_order, rhs.row_order + height, row_order);
        std::copy(rhs.col_order, rhs.col_order + width, col_order);
        return *this;
    }

    matrix_t& operator=(matrix_t&& rhs)
    {
        if (this == &rhs)
            return *this;

        height = rhs.height;
        width  = rhs.width;
        data   = rhs.data;
        row_order = rhs.row_order;
        col_order = rhs.col_order;

        return *this;
    }

    ~matrix_t()
    {
        delete[] data;
        delete[] row_order;
        delete[] col_order;
    }

    static matrix_t quad(int sz, T val = T{})
    {
        return matrix_t{sz, sz, val};
    }

    template<std::input_iterator it>
    static matrix_t quad(int sz, it begin, it end)
    {
        return matrix_t{sz, sz, begin, end};
    }

    template<std::input_iterator it>
    static matrix_t diag(int sz, it begin, it end)
    {
        matrix_t result {quad(sz)};
        for (auto i = 0, itr = begin; i < sz && itr != end; ++itr, i++)
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

    static matrix_t diag(int sz, T val = T{})
    {
        matrix_t result {quad(sz)};
        for (auto i = 0; i < sz; i++)
            result.data[i * sz + i] = val;
        return result;
    }

    T& at(int row_ind, int col_ind) & noexcept
    {
        return data[row_order[row_ind] * width + col_order[col_ind]];
    }

    const T& at(int row_ind, int col_ind) const & noexcept
    {
        return data[row_order[row_ind] * width + col_order[col_ind]];
    }

    T at(int row_ind, int col_ind) const && noexcept
    {
        return data[row_order[row_ind] * width + col_order[col_ind]];
    }

    void swap_row(int ind1, int ind2) &
    {
        std::swap(row_order[ind1], row_order[ind2]);
    }

    void swap_col(int ind1, int ind2) &
    {
        std::swap(col_order[ind1], col_order[ind2]);
    }

    bool operator==(const matrix_t& rhs) const
    {
        if (height != rhs.height)
            return false;
        if (width != rhs.width)
            return false;
        for (auto i = 0; i < height; i++)
            for (auto j = 0; j < width; j++)
                if (at(i, j) != rhs.at(i, j))
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
                out << at(i, j) << ' ';
            out << at(i, width - 1) << '}';
            
        }
        out << '}';
        return out;
    }    
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const matrix_t<T>& mat)
{
    return mat.dump(out);
}

} //namespace
