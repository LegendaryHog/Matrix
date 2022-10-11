#pragma once
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <stdexcept>

namespace matrix{

template<typename T>
class matrix_t
{
    int height = 0, width = 0;
    T** data = nullptr;

    class proxy_row
    {
        int len = 0;
        T* row = nullptr;

        public:
        proxy_row(int l, T* arr): len {l}, row {arr} {}

        T& operator[] (int ind) {return row[ind];}
        const T& operator[] (int ind) const {return row[ind];}

        T& at(int ind)
        {
            if (ind >= len)
                throw std::out_of_range{"request access to elem out of row"};
            return row[ind];
        }
        const T& at (int ind) const
        {
            if (ind >= len)
                throw std::out_of_range{"request access to elem out of row"};
            return row[ind];
        }
    };

    public:
    matrix_t(int h, int w, T val = T{})
    :height {h}, width {w}, data {new T*[height] {new T[width] {}}}
    {
        if (height < 0 || width < 0)
            throw std::length_error{"In constructor of matrix: height or width less null"};
        for (auto i = 0; i < height; i++)
            for (auto j = 0; j < width; j++)
                data[i][j] = val;
    }

    template<std::input_iterator it>
    matrix_t(int h, int w, it begin, it end)
    :height {h}, width {w}, data {new T*[height] {new T[width] {}}}
    {
        if (height < 0 || width < 0)
            throw std::length_error{"In constructor of matrix: height or width less null"};
        
        for (auto i = 0, itr = begin; i < height; i++)
            for (auto j = 0; itr != end && j < width; j++, ++itr)
                data[i][j] = *itr++;    
    }

    matrix_t(T val = T{}): height {1}, width {1}, data {new T*[height] {new T[width] {val}}} {}

    matrix_t(std::initializer_list<T> onedim_list)
    :height {onedim_list.size()}, width {1}, data{new T*[height] {new T[width] {}}}
    {
        auto i = 0;
        for (auto elem: onedim_list)
            data[i++][0] = elem;
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
        data = new T*[height] {new T[width] {}};

        std::size_t i = 0, j = 0;
        for (auto row: twodim_list)
        {
            for (auto elem: row)
                data[i][j++] = elem;
            i++;
        }
    }

    matrix_t(matrix_t& rhs)
    :height {rhs.height}, width {rhs.width}, data {new T*[height]{new T[width] {}}}
    {
        for (auto i = 0; i < height; i++)
            for (auto j = 0; j < width; j++)
                rhs.data[i][j] = data[i][j];
    }

    matrix_t(matrix_t&& rhs)
    :height {rhs.height}, width {rhs.width}, data {rhs.data}
    {}

    matrix_t operator= (matrix_t& rhs) &
    {
        height = rhs.height;
        width  = rhs.width;
        data = new T*[height]{new T[width] {}};

        for (auto i = 0; i < height; i++)
            for (auto j = 0; j < width; j++)
                rhs.data[i][j] = data[i][j];
        return *this;
    }

    matrix_t operator= (matrix_t&& rhs)
    {
        height = rhs.height;
        width  = rhs.width;
        std::swap(data, rhs.data);
        return *this;
    }

    ~matrix_t()
    {
        for (auto i = 0; i < height; i++)
            delete[] data[i];
        delete[] data;
    }

    proxy_row operator[] (int ind) {return proxy_row{width, data[ind]};}
    const proxy_row operator[] (int ind) const {return proxy_row{width, data[ind]};}

    proxy_row at(int ind)
    {
        if (ind >= height)
                throw std::out_of_range{"request access to row out of matrix"};
        return proxy_row{width, data[ind]};
    }
    const proxy_row at(int ind) const
    {
        if (ind >= height)
                throw std::out_of_range{"request access to row out of matrix"};
        return proxy_row{width, data[ind]};
    }    
};

} //namespace
