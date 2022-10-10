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
        
        auto itr = begin;
        for (auto i = 0; i < height; i++)
        {
            for (auto j = 0; itr != end && j < width; j++, ++itr)
                data[i][j] = *itr++;
            if (itr == end)
                break;
        }    
    }

    matrix_t(T val = T{}): height {1}, width {1}, data {new T*[height] {new T[width] {val}}} {}

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
        data = new T*[height]{new T[width]{}};

        std::size_t i = 0, j = 0;
        for (auto row: twodim_list)
        {
            for (auto elem: row)
                data[i][j++] = elem;
            i++;
        }
    }
};

} //namespace